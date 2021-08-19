package com.pxy.larkxr_app_nolo;

import android.app.Activity;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Window;

import com.pxy.cloudlarkxrkit.XrSystem;

import java.io.File;

public class MainActivity extends Activity {

    private static final String TAG = "Nolo_MainActivity";

    static{
        System.loadLibrary("native-lib");
    }

    MyGLSurfaceView mView;
    private XrSystem        xrSystem = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.d(TAG, "onCreate: ");
        requestWindowFeature(Window.FEATURE_NO_TITLE);

        String s1 = getFilesDir().getAbsolutePath();

        String s2 = "";
        File externalFile = getExternalFilesDir("/");
        if (externalFile != null) {
            s2 = externalFile.getAbsolutePath();
        }

        nativeInit(getResources().getAssets(), s1, s2);

        xrSystem = new XrSystem();
        xrSystem.init(this);

        super.onCreate(savedInstanceState);
        mView = new MyGLSurfaceView(this, this);

        setContentView(mView);
    }

    @Override
    public void onPause() {
        Log.d(TAG, "onPause: ");
        if (mView != null) {
            mView.onPause();
        }
        super.onPause();
        xrSystem.onPause();
    }

    @Override
    public void onResume() {
        Log.d(TAG, "onResume: ");
        super.onResume();
        xrSystem.onResume();
        if (mView != null) {
            mView.onResume();
        }
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        // TODO Auto-generated method stub

        //Let NDK handle key event
        if (mView != null) {
            if (mView.onKeyDownNative(keyCode) == 1) {
                return true;
            }
        }

        return super.onKeyDown(keyCode, event);
    }

    @Override
    protected void onDestroy() {
        Log.d(TAG, "onDestroy: ");
        super.onDestroy();
        if (mView != null) {
            mView.onDestroy();
        }
        xrSystem.onDestroy();
    }

    // native application
    private native void nativeInit(AssetManager am, String internalDataPath, String externalDataPath);
}
