package com.pxy.ar_huawei_app;

import android.app.Activity;
import android.content.Intent;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.Toast;

import com.huawei.hiar.AREnginesApk;
import com.huawei.hiar.ARSession;
import com.huawei.hiar.exceptions.ARUnSupportedConfigurationException;
import com.huawei.hiar.exceptions.ARUnavailableClientSdkTooOldException;
import com.huawei.hiar.exceptions.ARUnavailableServiceApkTooOldException;
import com.huawei.hiar.exceptions.ARUnavailableServiceNotInstalledException;
import com.pxy.ar_huawei_app.rendering.XrRenderManager;
import com.pxy.ar_huawei_app.util.DisplayRotationManager;
import com.pxy.ar_huawei_app.util.GestureEvent;
import com.pxy.ar_huawei_app.util.LogUtil;
import com.pxy.ar_huawei_app.util.PermissionManager;
import com.pxy.cloudlarkxrkit.XrSystem;
import com.pxy.larkcore.Util;

import java.util.concurrent.ArrayBlockingQueue;

public class MainActivity extends Activity {
    private static final String TAG =MainActivity.class.getSimpleName();

    private static final int MOTIONEVENT_QUEUE_CAPACITY = 2;

    private static final int OPENGLES_VERSION = 2;

    private ARSession mArSession;

    private GLSurfaceView mSurfaceView;

    //private WorldRenderManager mWorldRenderManager;
    private XrRenderManager xrRenderManager;

    private GestureDetector mGestureDetector;

    private DisplayRotationManager mDisplayRotationManager;

    private ArrayBlockingQueue<GestureEvent> mQueuedSingleTaps = new ArrayBlockingQueue<>(MOTIONEVENT_QUEUE_CAPACITY);

    private String message = null;

    private boolean isRemindInstall = false;

    // lark xr system
    private XrSystem xrSystem = null;

    private long nativeApplication;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        PermissionManager.checkPermission(this);

        FWBID();
        Init();
        InitXR();
    }

    private void InitXR(){
        xrSystem = new XrSystem();
        xrSystem.init(this, Util.getLocalMacAddress(this));

        nativeApplication=JniInterface.createNativeApplication(this);
        xrRenderManager.setNativeApplication(nativeApplication);
    }

    private void FWBID() {
        mSurfaceView = findViewById(R.id.surfaceview);
    }

    private void Init() {
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        //显示旋转管理器
        mDisplayRotationManager = new DisplayRotationManager(this);
        //初始姿态检测器
        initGestureDetector();
        //设置暂停时保留EGL上下文
        mSurfaceView.setPreserveEGLContextOnPause(true);
        //设置EGL上下文客户端版本
        mSurfaceView.setEGLContextClientVersion(OPENGLES_VERSION);
        //设置EGL配置选择器，包括颜色缓冲区的位数和深度位数。
        mSurfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0);

/*        mWorldRenderManager = new WorldRenderManager(this, this);
        mWorldRenderManager.setDisplayRotationManage(mDisplayRotationManager);
        mWorldRenderManager.setQueuedSingleTaps(mQueuedSingleTaps);*/
        JniInterface.setAssetManager(getAssets());

        xrRenderManager=new XrRenderManager(this,this);
        xrRenderManager.setDisplayRotationManage(mDisplayRotationManager);
        xrRenderManager.setQueuedSingleTaps(mQueuedSingleTaps);
        mSurfaceView.setRenderer(xrRenderManager);
        //连续渲染模式
        mSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
    }

    private void initGestureDetector() {
        //姿态检测器
        mGestureDetector = new GestureDetector(this, new GestureDetector.SimpleOnGestureListener() {
            @Override
            public boolean onDoubleTap(MotionEvent motionEvent) {
                onGestureEvent(GestureEvent.createDoubleTapEvent(motionEvent));
                return true;
            }

            @Override
            public boolean onSingleTapConfirmed(MotionEvent motionEvent) {
                onGestureEvent(GestureEvent.createSingleTapConfirmEvent(motionEvent));
                return true;
            }

            @Override
            public boolean onDown(MotionEvent motionEvent) {
                return true;
            }

            @Override
            public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
                onGestureEvent(GestureEvent.createScrollEvent(e1, e2, distanceX, distanceY));
                return true;
            }
        });

        mSurfaceView.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                return mGestureDetector.onTouchEvent(event);
            }
        });
    }

    private void onGestureEvent(GestureEvent e) {
        boolean offerResult = mQueuedSingleTaps.offer(e);
        if (offerResult) {
            LogUtil.debug(TAG, "Successfully joined the queue.");
        } else {
            LogUtil.debug(TAG, "Failed to join queue.");
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (!arEngineAbilityCheck()) {
            finish();
            return;
        }
        JniInterface.onResume(nativeApplication, getApplicationContext(),this);
        mSurfaceView.onResume();

        // Listen to display changed events to detect 180 rotation, which does not cause config change or view resize.
        mDisplayRotationManager.registerDisplayListener();
    }

    private void setMessageWhenError(Exception catchException) {
        if (catchException instanceof ARUnavailableServiceNotInstalledException) {
            startActivity(new Intent(this,ConnectAppMarketActivity.class));
        } else if (catchException instanceof ARUnavailableServiceApkTooOldException) {
            message = "Please update HuaweiARService.apk";
        } else if (catchException instanceof ARUnavailableClientSdkTooOldException) {
            message = "Please update this app";
        } else if (catchException instanceof ARUnSupportedConfigurationException) {
            message = "The configuration is not supported by the device!";
        } else {
            message = "exception throw";
        }
    }

    /**
     * Check whether HUAWEI AR Engine server (com.huawei.arengine.service) is installed on
     * the current device. If not, redirect the user to HUAWEI AppGallery for installation.
     *
     * @return whether HUAWEI AR Engine server is installed
     */
    private boolean arEngineAbilityCheck() {
        boolean isInstallArEngineApk = JniInterface.isArEngineApkInstalled(nativeApplication, getApplicationContext());
        if (!isInstallArEngineApk && isRemindInstall) {
            Toast.makeText(this, "Please agree to install.", Toast.LENGTH_LONG).show();
            finish();
        }
        Log.d(TAG, "Is Install AR Engine Apk: " + isInstallArEngineApk);
        if (!isInstallArEngineApk) {
            startActivity(new Intent(this, ConnectAppMarketActivity.class));
            isRemindInstall = true;
        }
        return JniInterface.isArEngineApkInstalled(nativeApplication, getApplicationContext());
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] results) {
        if (!PermissionManager.hasPermission(this)) {
            Toast.makeText(this, "This application needs camera permission.", Toast.LENGTH_LONG).show();
            finish();
        }
    }

    private void stopArSession() {
        LogUtil.info(TAG, "stopArSession start.");
        Toast.makeText(this, message, Toast.LENGTH_LONG).show();
        if (mArSession != null) {
            mArSession.stop();
            mArSession = null;
        }
        LogUtil.info(TAG, "stopArSession end.");
    }

    @Override
    protected void onPause() {
        LogUtil.info(TAG, "onPause start.");
        super.onPause();
        if (mArSession != null) {
            mDisplayRotationManager.unregisterDisplayListener();
            mSurfaceView.onPause();
            mArSession.pause();
        }
        LogUtil.info(TAG, "onPause end.");
    }

    @Override
    protected void onDestroy() {
        LogUtil.info(TAG, "onDestroy start.");
        if (mArSession != null) {
            mArSession.stop();
            mArSession = null;
        }
        super.onDestroy();
        LogUtil.info(TAG, "onDestroy end.");
    }

    @Override
    public void onWindowFocusChanged(boolean isHasFocus) {
        LogUtil.debug(TAG, "onWindowFocusChanged");
        super.onWindowFocusChanged(isHasFocus);
        if (isHasFocus) {
            getWindow().getDecorView()
                    .setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_STABLE | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                            | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                            | View.SYSTEM_UI_FLAG_FULLSCREEN | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
        }
    }
}