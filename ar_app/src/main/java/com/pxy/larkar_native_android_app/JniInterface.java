package com.pxy.larkar_native_android_app;

import android.content.Context;

public class JniInterface {
    public static native void creatNativeApplication(Context act);
    public static native void enterapp(String appid);
}
