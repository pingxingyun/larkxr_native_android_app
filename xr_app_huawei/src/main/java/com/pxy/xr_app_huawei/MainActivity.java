package com.pxy.xr_app_huawei;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.content.pm.ActivityInfo;
import android.net.ConnectivityManager;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.NetworkRequest;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.Window;
import android.view.WindowManager;

import com.pxy.cloudlarkxrkit.CrashHandler;
import com.pxy.cloudlarkxrkit.XrSystem;
import com.pxy.larkcore.Util;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = "HuaweiMainAcitvity";

    static {
        System.loadLibrary("lark_xr_huawei");
        System.loadLibrary("lark_pxygl");
    }
    private XrSystem xrSystem = null;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //nativeInit();
        CrashHandler.getInstance().init(this);

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);// 设置全屏
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);

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
        xrSystem.init(this, Util.getLocalMacAddress(this));
    }

    private ConnectivityManager.NetworkCallback mNetworkCallback = new ConnectivityManager.NetworkCallback() {
        @Override
        public void onAvailable(@NonNull Network network) {
            super.onAvailable(network);
            Log.d(TAG, "on net work avaliable");
            //nativeNetworkAvaliable();
        }

        @Override
        public void onLost(@NonNull Network network) {
            super.onLost(network);
            Log.d(TAG, "on net work lost");
            //nativeNetworkLost();
        }
    };

    //private native void nativeInit();
}