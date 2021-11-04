package com.pxy.cloudlarkxrhtc;

import com.htc.vr.sdk.VRActivity;
import com.pxy.cloudlarkxrhtc.Activity.BaseApplication;
import com.pxy.cloudlarkxrkit.CrashHandler;
import com.pxy.cloudlarkxrkit.XrSystem;

import android.content.res.AssetManager;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import androidx.annotation.NonNull;

import java.io.File;

public class MainActivity extends VRActivity {
    private static String TAG = "HTC_MainActivity";
    static {
        System.loadLibrary("lark_xr_htc");
    }
    //
    private XrSystem xrSystem = null;
    public MainActivity() {
        super.setUsingRenderBaseActivity(true);
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        String s1 = getFilesDir().getAbsolutePath();

        String s2 = "";
        File externalFile = getExternalFilesDir("/");
        if (externalFile != null) {
            s2 = externalFile.getAbsolutePath();
        }

        init(getResources().getAssets(), s1, s2);
        super.onCreate(savedInstanceState);

        CrashHandler.getInstance().init(this);
        // shoud init vr system before vr started.
        xrSystem = new XrSystem();
        xrSystem.init(this);

        Log.d(TAG, "onCreate");
        nativeCreated();

        BaseApplication.getInstance().setmHandler(handler);
    }

    Handler handler=new Handler(){
        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);
            Log.d(TAG, "msg:"+msg.what);
            if (msg.what==4){
                finish();
            }
        }
    };
    @Override
    protected void onResume() {
        super.onResume();
        Log.d(TAG, "onResume");
        nativeResume();
        xrSystem.onResume();
    }
    @Override
    protected void onPause() {
        super.onPause();
        Log.d(TAG, "onPause");
        nativePause();
        xrSystem.onPause();
    }
    @Override
    protected void onDestroy() {
        super.onDestroy();
        Log.d(TAG, "onDestroy");
        nativeDestory();
        xrSystem.onDestroy();
    }


    // Pass this acitivty instance to native
    public native void init(AssetManager am, String internalDataPath, String externalDataPath);
    private native void nativeCreated();
    private native void nativeResume();
    private native void nativePause();
    private native void nativeDestory();
}
