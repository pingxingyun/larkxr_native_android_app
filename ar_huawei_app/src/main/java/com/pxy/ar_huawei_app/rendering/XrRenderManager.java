package com.pxy.ar_huawei_app.rendering;

import android.app.Activity;
import android.content.Context;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.util.Log;

import com.huawei.hiar.ARSession;
import com.pxy.ar_huawei_app.JniInterface;
import com.pxy.ar_huawei_app.util.DisplayRotationManager;
import com.pxy.ar_huawei_app.util.GestureEvent;
import com.pxy.ar_huawei_app.util.LogUtil;

import java.util.concurrent.ArrayBlockingQueue;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class XrRenderManager implements GLSurfaceView.Renderer{
    private static final String TAG = XrRenderManager.class.getSimpleName();

    private static final float GL_CLEAR_COLOR_RED = 0.1f;

    private static final float GL_CLEAR_COLOR_GREEN = 0.1f;

    private static final float GL_CLEAR_COLOR_BLUE = 0.1f;

    private static final float GL_CLEAR_COLOR_ALPHA = 1.0f;

    private DisplayRotationManager mDisplayRotationManager;
    private ArrayBlockingQueue<GestureEvent> mQueuedSingleTaps;
    private Activity mActivity;
    private Context mContext;
    private ARSession mSession;
    private long mNativeApplication;

    public XrRenderManager(Activity activity, Context context) {
        mActivity = activity;
        mContext = context;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        GLES20.glClearColor(GL_CLEAR_COLOR_RED, GL_CLEAR_COLOR_GREEN, GL_CLEAR_COLOR_BLUE, GL_CLEAR_COLOR_ALPHA);
        Log.e(TAG,"onGlSurfaceCreated");
        JniInterface.onGlSurfaceCreated(mNativeApplication);
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        mDisplayRotationManager.updateViewportRotation(width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        // Synchronized to avoid racing onDestroy.
        synchronized (this) {
            if (mNativeApplication == 0) {
                return;
            }
            if (mDisplayRotationManager.getDeviceRotation()) {
                mDisplayRotationManager.updateArSessionDisplayGeometry(mNativeApplication);
            }
            JniInterface.onGlSurfaceDrawFrame(mNativeApplication);
        }
    }

    public void setDisplayRotationManage(DisplayRotationManager displayRotationManager) {
        if (displayRotationManager == null) {
            LogUtil.error(TAG, "SetDisplayRotationManage error, displayRotationManage is null!");
            return;
        }
        mDisplayRotationManager = displayRotationManager;
    }

    public void setQueuedSingleTaps(ArrayBlockingQueue<GestureEvent> queuedSingleTaps) {
        if (queuedSingleTaps == null) {
            LogUtil.error(TAG, "setSession error, arSession is null!");
            return;
        }
        mQueuedSingleTaps = queuedSingleTaps;
    }

    public void setArSession(ARSession arSession) {
        if (arSession == null) {
            LogUtil.error(TAG, "setSession error, arSession is null!");
            return;
        }
        mSession = arSession;
    }

    public void setNativeApplication(long nativeApplication) {
        mNativeApplication = nativeApplication;
    }
}
