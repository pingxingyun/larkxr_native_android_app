package com.pxy.cloudlarkxroculus;

import android.content.Context;
import android.net.ConnectivityManager;
import android.net.ConnectivityManager.NetworkCallback;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.NetworkRequest;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;

import androidx.annotation.NonNull;

import com.pxy.cloudlarkxrkit.CrashHandler;
import com.pxy.cloudlarkxrkit.XrSystem;

public class MainActivity extends android.app.NativeActivity {

    private static final String TAG = "OculusMainAcitvity";

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
    }

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
