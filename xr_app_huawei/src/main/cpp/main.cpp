//
// Created by Hayasi-Yumito on 2021/11/23.
//

//#include "main.h"
#include <android/window.h>				// for AWINDOW_FLAG_KEEP_SCREEN_ON
#include <android/native_window_jni.h>	// for native window JNI
#include <android_native_app_glue.h>
#include <build_config.h>
#include <log.h>
#include "xr_demo.h"
#include <jni.h>

/*// JNI 入口函数必须定义
void android_main( struct android_app * app ) {

}*/
XrDemo* gApp = nullptr;
AppState g_AppState = ON_RESUME;
bool stateBegeinSession = false;

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_xr_1app_1huawei_MainActivity_nativeInit(JNIEnv * jni
        , jclass clazz
        , jint x, jint y
        , jint texid
        , jobject act
        ,jobject surface)
{
    LOGI("init vr nativeOnCreate");
    if (nullptr == gApp)
    {
        LOGI("init vr");
        JavaVM *_vm = nullptr;
        jint ret = jni->GetJavaVM(&_vm);

        LOGI("Create global ref");

        jobject g_act = jni->NewGlobalRef(act);

        gApp = new XrDemo(_vm, g_act);

        gApp->setSurface(jni, surface);
        gApp->initSkyboxMethod(jni);
        gApp->start();
    }else{
        LOGI("resume vr");
        g_AppState = ON_RESUME;
        gApp->setSurface(jni, surface);
        stateBegeinSession = true;
    }
}