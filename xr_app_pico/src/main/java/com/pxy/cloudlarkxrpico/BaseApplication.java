package com.pxy.cloudlarkxrpico;

import android.app.Application;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;

public class BaseApplication extends Application {
    //Application.class
    private Handler mHandler = new Handler(Looper.getMainLooper()) {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            mListener.heandleMessage(msg);
            System.out.println("mListener 地址值==== " + mListener);
        }
    };

    public Handler getmHandler() {
        return mHandler;
    }

    public void setmHandler(Handler mHandler) {
        this.mHandler = mHandler;
    }

    private static HandlerListener mListener;

    public static void setOnHandlerListener(HandlerListener listener) {
        mListener = listener;
    }
    public  static HandlerListener getListener(){
        return mListener;
    }

    public  interface HandlerListener {
        void heandleMessage(Message msg);
    }

    public Class getMainActivityClass() {
        return MainActivity.class;
    }

    public BaseApplication() {
    }

    private static BaseApplication instance = null;

    public static BaseApplication getInstance() {
        if (instance == null) {
            instance = new BaseApplication();
        }
        return instance;
    }
}
