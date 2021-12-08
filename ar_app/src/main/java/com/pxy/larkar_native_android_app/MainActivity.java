package com.pxy.larkar_native_android_app;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.Manifest;
import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.content.pm.PackageManager;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CaptureRequest;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;
import android.view.Surface;
import android.widget.Toast;

import com.pxy.cloudlarkxrkit.XrSystem;
import com.pxy.larkcore.CloudlarkManager;
import com.pxy.larkcore.LarkCoreClient;
import com.pxy.larkcore.Util;
import com.pxy.larkcore.request.AppListItem;
import com.pxy.larkcore.request.Base;
import com.pxy.larkcore.request.EnterAppliInfo;
import com.pxy.larkcore.request.GetAppliList;
import com.pxy.larkcore.request.PageInfo;

import java.util.ArrayList;
import java.util.List;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MainActivity extends AppCompatActivity {
    private static String TAG="ARMainActivity";
    //相机相关
    private List<Surface> surfaceViews=new ArrayList<>();//展示的views
    private GLSurfaceView surfaceView;//用于展示的glview
    private CameraDevice mCameraDevice;//摄像机
    private CameraCaptureSession mCameraCaptureSession;//摄像机数据
    private Handler handler;
    private int cameraID=0;//前后摄像头ID
    private int mTextureId = -1;
    private SurfaceTexture mSurfaceTexture;
    private CameraDrawer mDrawer;
    //rtc相关
    private XrSystem xrSystem = null;
    private LarkCoreClient client;
    //private String mServerIp = "222.128.6.137:8585";
    private String mServerIp = "192.168.31.120:8585";
    private EnterAppliInfo.Config rtcParams;

    static {
        Log.i(TAG, "LoadLibrary");
        System.loadLibrary("ar_app");
        System.loadLibrary("lark_pxygl");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        surfaceView=new GLSurfaceView(this);
        setContentView(surfaceView);

        FVBI();
        initview();
        initRtc();
    }

    private void FVBI() {
        //surfaceView = findViewById(R.id.suf);
    }

    private void initview() {
        initGL();
    }

    private void initGL() {
        //检查设备是否支持OpenGL ES 2.0
        final ActivityManager activityManager = (ActivityManager) getSystemService(ACTIVITY_SERVICE);
        final ConfigurationInfo configurationInfo = activityManager.getDeviceConfigurationInfo();
        final boolean supportES2 = configurationInfo.reqGlEsVersion >= 0x00020000;

        //配置OpenGL ES，主要是版本设置和设置Renderer，Renderer用于执行OpenGL的绘制
        if (supportES2) {
            surfaceView.setEGLContextClientVersion(2);
            surfaceView.setRenderer(renderer);
        } else {
            Toast.makeText(getApplicationContext(),"不支持OpenGL ES 2.0版本",Toast.LENGTH_SHORT).show();
            return;
        }
    }

    private GLSurfaceView.Renderer renderer=new GLSurfaceView.Renderer() {
        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            mTextureId = OpenGLUtils.getExternalOESTextureID();
            mSurfaceTexture = new SurfaceTexture(mTextureId);
            mDrawer = new CameraDrawer();
            mSurfaceTexture.setDefaultBufferSize(surfaceView.getWidth(), surfaceView.getHeight());

            initCamera();
        }

        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {
            Log.e(TAG, "onSurfaceChanged. thread: " + Thread.currentThread().getName());
            Log.d(TAG, "onSurfaceChanged. width: " + width + ", height: " + height);
            GLES20.glViewport(0, 0, width, height);
        }

        @Override
        public void onDrawFrame(GL10 gl) {
            GLES20.glClearColor(0, 0, 0, 0);
            GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT | GLES20.GL_DEPTH_BUFFER_BIT);
            mSurfaceTexture.updateTexImage();
            mDrawer.draw(mTextureId, cameraID != 0);
        }
    };

    /**
     * 摄像头创建监听
     */
    private CameraDevice.StateCallback stateCallback = new CameraDevice.StateCallback() {
        @Override
        public void onOpened(CameraDevice camera) {//打开摄像头
            mCameraDevice = camera;
            //开启预览
            takePreview();


        }

        @Override
        public void onDisconnected(CameraDevice camera) {//关闭摄像头
            if (null != mCameraDevice) {
                mCameraDevice.close();
                MainActivity.this.mCameraDevice = null;
            }
        }

        @Override
        public void onError(CameraDevice camera, int error) {//发生错误
            Toast.makeText(MainActivity.this, "摄像头开启失败", Toast.LENGTH_SHORT).show();
        }
    };

    private void initCamera() {
        HandlerThread handlerThread = new HandlerThread("Camera2");
        handlerThread.start();
        handler = new Handler(handlerThread.getLooper());
        String cameraid="";
        switch (cameraID){
            case 0:{
                cameraid= CameraCharacteristics.LENS_FACING_FRONT+"";
            }break;
            case 1:{
                cameraid= CameraCharacteristics.LENS_FACING_BACK+"";
            }break;
            default:{
                cameraid= CameraCharacteristics.LENS_FACING_FRONT+"";
            };
        }
        //cameraID= CameraCharacteristics.LENS_FACING_BACK+"";
        /*
        通过context.getSystemService(Context.CAMERA_SERVICE) 获取CameraManager.
                调用CameraManager .open()方法在回调中得到CameraDevice.
                通过CameraDevice.createCaptureSession() 在回调中获取CameraCaptureSession.
                构建CaptureRequest, 有三种模式可选 预览/拍照/录像.
                通过 CameraCaptureSession发送CaptureRequest, capture表示只发一次请求, setRepeatingRequest表示不断发送请求.
                拍照数据可以在ImageReader.OnImageAvailableListener回调中获取, CaptureCallback中则可获取拍照实际的参数和Camera当前状态.
        */
        CameraManager cameraManager = (CameraManager) this.getSystemService(Context.CAMERA_SERVICE);
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED) {
            // TODO: Consider calling
            //    ActivityCompat#requestPermissions
            // here to request the missing permissions, and then overriding
            //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
            //                                          int[] grantResults)
            // to handle the case where the user grants the permission. See the documentation
            // for ActivityCompat#requestPermissions for more details.
            ActivityCompat.requestPermissions(MainActivity.this, new String[]{Manifest.permission.CAMERA}, 1);
            return;
        }
        try {
            surfaceViews.add(new Surface(mSurfaceTexture));
            cameraManager.openCamera(cameraid,stateCallback,handler);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }

    /**
     * 开始预览
     */
    private void takePreview() {
        try {
            // 创建预览需要的CaptureRequest.Builder
            final CaptureRequest.Builder previewRequestBuilder = mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
            // 将SurfaceView的surface作为CaptureRequest.Builder的目标
            previewRequestBuilder.addTarget(surfaceViews.get(0));
            // 创建CameraCaptureSession，该对象负责管理处理预览请求和拍照请求
            mCameraDevice.createCaptureSession(surfaceViews, new CameraCaptureSession.StateCallback() // ③
            {
                @Override
                public void onConfigured(CameraCaptureSession cameraCaptureSession) {
                    if (null == mCameraDevice) return;
                    // 当摄像头已经准备好时，开始显示预览
                    mCameraCaptureSession = cameraCaptureSession;
                    try {
                        // 自动对焦
                        previewRequestBuilder.set(CaptureRequest.CONTROL_AF_MODE, CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE);
                        // 打开闪光灯
                        previewRequestBuilder.set(CaptureRequest.CONTROL_AE_MODE, CaptureRequest.CONTROL_AE_MODE_ON_AUTO_FLASH);
                        // 显示预览
                        CaptureRequest previewRequest = previewRequestBuilder.build();
                        mCameraCaptureSession.setRepeatingRequest(previewRequest, null, handler);
                    } catch (CameraAccessException e) {
                        e.printStackTrace();
                    }
                }

                @Override
                public void onConfigureFailed(CameraCaptureSession cameraCaptureSession) {
                    Toast.makeText(MainActivity.this, "配置失败", Toast.LENGTH_SHORT).show();
                }
            }, handler);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }
/*------------rtc-------------*/
    private void initRtc(){
        CloudlarkManager.init(this, CloudlarkManager.APP_TYPE_AR);

        Base.setServerAddr(false, mServerIp);

        xrSystem = new XrSystem();
        xrSystem.init(this, Util.getLocalMacAddress(this));

        JniInterface.creatNativeApplication(MainActivity.this);

        GetAppliList getAppliList=new GetAppliList(new GetAppliList.Callback() {
            @Override
            public void onSuccess(List<AppListItem> list) {
                AppListItem item=list.get(0);
                EnterAppliInfo enterAppliInfo=new EnterAppliInfo(new EnterAppliInfoCallback());
                enterAppliInfo.enterApp(item);
            }

            @Override
            public void onPageInfoChange(PageInfo pageInfo) {

            }

            @Override
            public void onFail(String s) {

            }
        });
        getAppliList.getAppliList();
    }

    LarkCoreClient.LarkCoreClientEvents event=new LarkCoreClient.LarkCoreClientEvents() {
        @Override
        public void onChooseAppServerIpFinished(String s) {
            Log.e("LarkCoreClientEventsIP",s);
            JniInterface.enterapp(rtcParams.appilId);
        }

        @Override
        public void onSyncTask() {

        }

        @Override
        public void onConnected() {

        }

        @Override
        public void onLoginSuccess(int i) {

        }

        @Override
        public void onAllowStreaming() {

        }

        @Override
        public void onAllowVrStreaming(int i) {

        }

        @Override
        public void onWebRtcOfferAnswer(String s, String s1) {

        }

        @Override
        public void onWebRtcIcecandidate(String s, int i, String s1) {

        }

        @Override
        public void onAppClose() {

        }

        @Override
        public void onClose() {

        }

        @Override
        public void onError(String s) {

        }
    };

    class EnterAppliInfoCallback implements EnterAppliInfo.Callback {
        @Override
        public void onSuccess(EnterAppliInfo.Config rtcParams) {
            Log.e("rtcParams",rtcParams.toString());
                /*mRtcClient = new RtcClient(rtcParams,renderer,
                        event,MainActivity.this);*/
            MainActivity.this.rtcParams=rtcParams;
            client=new LarkCoreClient(event);
                // 开始连接
            client.connect(rtcParams);
        }

        @Override
        public void onFail(String err) {

        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        if (requestCode == 1) {
            if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                initCamera();
            } else {
                // Permission Denied
                Toast.makeText(MainActivity.this, "未获取相机权限", Toast.LENGTH_SHORT).show();
            }
            return;
        }
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
    }
}

