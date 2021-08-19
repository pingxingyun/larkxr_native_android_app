package com.pxy.larkxr_app_nolo;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.SurfaceHolder;

import com.nibiru.lib.vr.NibiruVR;
import com.pxy.larkxr_app_nolo.util.Utils;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MyGLSurfaceView extends GLSurfaceView {

	boolean surfaceHasDestroyed = false;
	public MyGLSurfaceView(Context context, Activity activity) {
		
		super(context);
		Log.v("ndk", "========================MyGLSurfaceView()");
		setEGLContextClientVersion(2);
		setEGLConfigChooser(8, 8, 8, 8, 16, 0);

		NibiruVR.destroyNibiruVRService();
		NibiruVR.clearBeforeInit();

		initializeNative(this, activity, context.getAssets());
		
		setRenderer(new Renderer() {
			
			@Override
			public void onSurfaceCreated(GL10 gl, EGLConfig config) {
				Log.v("ndk", "======================== onSurfaceCreated");
				setTestTextureId(Utils.initTexture(MyGLSurfaceView.this, 0));
				onSurfaceCreatedNative();
				GLES30.glClearColor(0.5f, 0.5f, 0.5f, 1);
			}
			
			@Override
			public void onSurfaceChanged(GL10 gl, int width, int height) {
				Log.v("ndk", "======================== on surface changed: " + width + " : " + height);
				if (surfaceHasDestroyed) {
					onSurfaceCreatedNative();
					surfaceHasDestroyed = false;
				}
				onSurfaceChangedNative(width, height);
			}
			
			@Override
			public void onDrawFrame(GL10 gl) {
				GLES30.glClear(GLES30.GL_COLOR_BUFFER_BIT | GLES30.GL_DEPTH_BUFFER_BIT);
				onDrawFrameNative();
			}
		});
	}
	

	@Override
	protected void onAttachedToWindow() {
		super.onAttachedToWindow();
		
		onAttachWindowNative();
		Log.v("ndk", "========================onAttachedToWindow");
	}

	@Override
	protected void onDetachedFromWindow() {
		super.onDetachedFromWindow();
		onDetachWindowNative();
		Log.v("ndk", "========================onDetachedFromWindow");
	}
	
	
	@Override
	public void onPause() {
		onPauseNative();
		super.onPause();
		Log.v("ndk", "========================onPause");
	}


	@Override
	public void onResume() {
		super.onResume();
		onResumeNative();
		Log.v("ndk", "========================onResume");
	}

	public void onDestroy() {
		Log.v("nak", "=========================onDestroy");
		nativeOnDestroy();
	}

	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		super.surfaceCreated(holder);
		Log.v("ndk", "========================surfaceCreated");
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		queueEvent(new Runnable() {

			@Override
			public void run() {
				Log.v("ndk", "GL Runnable : surfaceDestroyed");
				surfaceHasDestroyed = true;
				onSurfaceDestroyed();
			}
		});
		super.surfaceDestroyed(holder);
		Log.v("ndk", "========================surfaceDestroyed");
	}

	native void initializeNative(GLSurfaceView view, Activity activity, AssetManager assets);
	
	native void onSurfaceCreatedNative();
	
	native void onSurfaceChangedNative(int width, int height);
	
	native void onDrawFrameNative();
	
	native void onAttachWindowNative();
	
	native void onDetachWindowNative();
	
	native void onPauseNative();
	
	native void onResumeNative();

	native int onKeyDownNative(int keyCode);

	native void onSurfaceDestroyed();

	native void setTestTextureId(int textureid);

	native void nativeOnDestroy();
}
