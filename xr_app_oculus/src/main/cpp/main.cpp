#include <cstdlib>
#include <unistd.h>
#include <pthread.h>
#include <sys/prctl.h>					// for prctl( PR_SET_NAME )
#include <android/window.h>				// for AWINDOW_FLAG_KEEP_SCREEN_ON
#include <android/native_window_jni.h>	// for native window JNI
#include <android_native_app_glue.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <android/log.h>
#include <build_config.h>

#include "VrApi.h"
#include "VrApi_Helpers.h"
#include "VrApi_SystemUtils.h"
#include "VrApi_Input.h"

#include "log.h"
#include "ovr_application.h"
#include "env_context.h"

#include "assimp/cimport.h"
#include "assimp/Logger.hpp"
#include "assimp/DefaultLogger.hpp"

/**
 * Process the next main command.
 */
static void app_handle_cmd( struct android_app * app, int32_t cmd )
{
    auto * application = (OvrApplication *)app->userData;
    if (application) {
        switch ( cmd )
        {
            // There is no APP_CMD_CREATE. The ANativeActivity creates the
            // application thread from onCreate(). The application thread
            // then calls android_main().
            case APP_CMD_START:
                LOGD("APP_CMD_START");
                break;
            case APP_CMD_RESUME:
                LOGD("APP_CMD_RESUME");
                application->OnResume();
                break;
            case APP_CMD_PAUSE:
                LOGD("APP_CMD_PAUSE");
                application->OnPause();
                break;
            case APP_CMD_STOP:
                LOGD("APP_CMD_STOP");
                break;
            case APP_CMD_DESTROY:
                LOGD("APP_CMD_DESTROY");
                application->OnDestory();
                break;
            case APP_CMD_INIT_WINDOW:
                LOGD("APP_CMD_INIT_WINDOW");
                application->OnInitWindow(app->window);
                break;
            case APP_CMD_TERM_WINDOW:
                LOGD("APP_CMD_TERM_WINDOW");
                break;
            default:break;
        }
    }
//    application->handleAndroidCmd(app, cmd);
}

void android_main( struct android_app * app ) {
    ANativeActivity_setWindowFlags(app->activity, AWINDOW_FLAG_KEEP_SCREEN_ON, 0);

    // init app
    OvrApplication application;

    if (!application.InitVR(app)) {
        LOGE("init vr failed");
        exit(0);
    }

    std::string appid;
    {
        EnvWrapper env_wraper = Context::instance()->GetEnv();
        JNIEnv* env = env_wraper.get();

        jobject native_activity = app->activity->clazz;
        jclass clazz = env->GetObjectClass(native_activity);
        jmethodID giid = env->GetMethodID(clazz, "getIntent", "()Landroid/content/Intent;");
        jobject intent = env->CallObjectMethod(native_activity, giid);

        jclass icl = env->GetObjectClass(intent);
        jmethodID gseid = env->GetMethodID(icl, "getStringExtra", "(Ljava/lang/String;)Ljava/lang/String;");

        auto j_appid = (jstring)env->CallObjectMethod(intent, gseid, env->NewStringUTF("appid"));

        if (j_appid != nullptr) {
            const char *c_appid = env->GetStringUTFChars(j_appid, 0);
            appid = c_appid;
            env->ReleaseStringUTFChars(j_appid, c_appid);
        }

        env->DeleteLocalRef(j_appid);
        env->DeleteLocalRef(clazz);
        env->DeleteLocalRef(icl);
        env->DeleteLocalRef(intent);
    }

    LOGV("enter appli param %s", appid.c_str());

    LOGD("init vr success");

    if (!appid.empty()) {
        application.Set2DUIEnterAppliMode(appid);
    }

    if (!application.InitGL()) {
        LOGE("init gl failed");
        application.ShutdownGL();
        application.ShutdownVR();
        exit(0);
    };

    LOGD("init gl success");
    application.InitJava();

    app->userData = &application;
    app->onAppCmd = app_handle_cmd;

#if 0
    FILE* file = fopen("/storage/emulated/0/Android/data/com.pxy.cloudlarkxroculus/files/test.txt", "a");
    if (file != nullptr) {
        LOGV("*******************************write file;");
//        fwrite(buffer, sizeof(char), length, file);
        fputs("HELLO WORLD!\n", file);
        fflush(file);
        fclose(file);
    } else {
        LOGV("*******************************fopen failed;");
    }
#endif

    while ( app->destroyRequested == 0 )
    {
        // Read all pending events.
        for (;;)
        {
            int events;
            struct android_poll_source * source;
            const int timeoutMilliseconds = (application.ovr() == nullptr && app->destroyRequested == 0) ? -1 : 0;
            if (ALooper_pollAll( timeoutMilliseconds, nullptr, &events, (void **)&source ) < 0)
            {
                break;
            }

            // Process this event.
            if (source != nullptr)
            {
                source->process(app, source);
            }

            application.HandleVrModeChange();
        }

        application.OnUpdate();

    }
    application.ShutdownGL();
    application.ShutdownVR();
}


extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_cloudlarkxroculus_MainActivity_nativeInit(JNIEnv *env, jobject thiz) {
    BuildConfig::init(env, "com/pxy/cloudlarkxroculus/BuildConfig");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_cloudlarkxroculus_MainActivity_nativeNetworkAvaliable(JNIEnv *env, jobject thiz) {
    Application* application = Application::instance();
    if (application != nullptr) {
        application->OnNetworkAvailable();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_cloudlarkxroculus_MainActivity_nativeNetworkLost(JNIEnv *env, jobject thiz) {
    Application* application = Application::instance();
    if (application != nullptr) {
        application->OnNetworkLost();
    }
}