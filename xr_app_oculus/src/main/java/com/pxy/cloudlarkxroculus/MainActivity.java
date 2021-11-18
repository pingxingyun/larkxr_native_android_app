package com.pxy.cloudlarkxroculus;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.ConnectivityManager;
import android.net.ConnectivityManager.NetworkCallback;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.NetworkRequest;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import androidx.annotation.NonNull;

import com.pxy.cloudlarkxrkit.CrashHandler;
import com.pxy.cloudlarkxrkit.XrSystem;
import com.pxy.cloudlarkxroculus.Activity.BaseApplication;
import com.pxy.cloudlarkxroculus.Activity.ListActivity;

public class MainActivity extends android.app.NativeActivity {

    private static final String TAG = "OculusMainAcitvity";
    private static final String SETTING = "pxy_setting";
    private static final String SETTING_LIST_3D= "list3D";

    static {
        System.loadLibrary("lark_xr_oculus");
        System.loadLibrary("lark_pxygl");
    }
    //
    private XrSystem xrSystem = null;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        nativeInit();
        super.onCreate(savedInstanceState);
        Log.d(TAG, "java active onCreate");
        CrashHandler.getInstance().init(this);

        ConnectivityManager connectivityManager =
                (ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            connectivityManager.registerDefaultNetworkCallback(mNetworkCallback);
        } else {
            NetworkRequest request = new NetworkRequest.Builder()
                    .addCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET).build();
            connectivityManager.registerNetworkCallback(request, mNetworkCallback);
        }

        xrSystem = new XrSystem();
        xrSystem.init(this);

        BaseApplication.getInstance().setmHandler(handler);
    }

    Handler handler=new Handler(){
        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);
            Log.d(TAG, "msg:"+msg.what);
            if (msg.what==4){
                System.exit(0);
            }
        }
    };
    @Override
    protected void onResume() {
        super.onResume();
        Log.d(TAG, "java activity onResume");
        xrSystem.onResume();
    }
    @Override
    protected void onPause() {
        super.onPause();
        Log.d(TAG, "java activity onPause");
        xrSystem.onPause();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Log.d(TAG, "java activity onDestroy");
        xrSystem.onDestroy();
    }

    public void onError(int errCode, String msg) {
        // TODO back to 2d applist when error
        Log.e(TAG,errCode+"|"+msg);
        switchTo2DAppList();
    }

    public void switchTo2DAppList() {
        Log.d(TAG, "switchTo2DAppList");
        //startActivity(new Intent(MainActivity.this, ListActivity.class));

        SharedPreferences sp = MainActivity.this.getSharedPreferences(SETTING, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sp.edit();
        editor.putBoolean(SETTING_LIST_3D, false);
        editor.apply();
        finish();
    }

    private NetworkCallback mNetworkCallback = new NetworkCallback() {
        @Override
        public void onAvailable(@NonNull Network network) {
            super.onAvailable(network);
            Log.d(TAG, "on net work avaliable");
            nativeNetworkAvaliable();
        }

        @Override
        public void onLost(@NonNull Network network) {
            super.onLost(network);
            Log.d(TAG, "on net work lost");
            nativeNetworkLost();
        }
    };

    private native void nativeInit();
    private native void nativeNetworkAvaliable();
    private native void nativeNetworkLost();
}
