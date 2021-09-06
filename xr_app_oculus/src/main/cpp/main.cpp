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
#include "logger.h"

#include "assimp/cimport.h"
#include "assimp/Logger.hpp"
#include "assimp/DefaultLogger.hpp"

std::string g_appid = "";
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

void android_main( struct android_app * app )
{
    // inint logger
    lark::Logger::InitLogger();
    ANativeActivity_setWindowFlags( app->activity, AWINDOW_FLAG_KEEP_SCREEN_ON, 0 );

    // init app
    OvrApplication application;


    if (!application.InitVR(app)) {
        LOGE("init vr failed");
        exit(0);
    }

    LOGD("init vr success");

    application.set_pre_appid(g_appid);

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

    LogE("ovr_native_application", "g_appid %s", g_appid.c_str());

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

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_cloudlarkxroculus_MainActivity_intoApp(JNIEnv *env, jobject thiz,jstring id) {
    // TODO: implement intoApp()
    LogE("ovr_native_application","-------------------------intoApp");
    Application* application = Application::instance();

    //ovr_application* application = (ovr_application*)ptr;
    if (application == nullptr) {
        return;
    }
    const char *charg_appid = env->GetStringUTFChars(id, 0);
    LogE("ovr_native_application","--------------------%s", charg_appid);
    g_appid = charg_appid;
    application->EnterAppli(charg_appid);
    env->ReleaseStringUTFChars(id, charg_appid);
}