package com.pxy.larkar_native_android_app;

import android.content.Context;
import android.content.res.AssetManager;

public class JniInterface {
    static AssetManager assetManager;
    public static native void creatNativeApplication(Context act,AssetManager assetManager);
    public static native void enterapp(String appid);
}
