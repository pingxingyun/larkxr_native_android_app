#include "egl_environment.h"
#include "logger.h"
#define LOG_TAG "EGL_ENV"

bool initEGLEnvironment(EGLDisplay& display, EGLConfig& config, EGLContext& context)
{
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY)
    {
        LOGE("Get EGL14 display failed.");
        return false;
    }

    if (!eglInitialize(display, nullptr, nullptr))
    {
        display = EGL_NO_DISPLAY;
        LOGE("Initialize EGL14 failed.");
        return false;
    }

    //EGL Display version info print
    const char *vendor = eglQueryString(display, EGL_VENDOR);
    const char *clientAPIs = eglQueryString(display, EGL_CLIENT_APIS);
    const char *eglversion = eglQueryString(display, EGL_VERSION);
    const char *eglExtension = eglQueryString(display, EGL_EXTENSIONS);

    //log those paras
    LOGI("IN: EGL_VENDOR: %s", vendor ? vendor : "Unkown");
    LOGI("IN: EGL_CLIENT_APIS: %s", clientAPIs ? clientAPIs : "Unkown");
    LOGI("IN: EGL_VERSION: %s", eglversion ? eglversion : "Unkown");
    LOGI("IN: EGL_EXTENSIONS: %s", eglExtension ? eglExtension : "Unkown");


    EGLConfig env_mConfig = nullptr;
    EGLContext env_mContext = EGL_NO_CONTEXT;

    //egl config chose colour config
    bool isConfigfound = false;
    const EGLint colourConfigAttrs[] =
            {
                    EGL_RED_SIZE, 8,
                    EGL_GREEN_SIZE, 8,
                    EGL_BLUE_SIZE, 8,
                    EGL_DEPTH_SIZE, 24,
                    EGL_SAMPLES, 0,
                    EGL_NONE, EGL_NONE,
                    EGL_NONE
            };

    EGLConfig configs[1200];
    EGLint configedNum = 0;

    if (EGL_FALSE == eglGetConfigs(display, configs, 1200, &configedNum))
        LOGE("IN: %s, eglGetConfigs return EGL_FALSE", __FUNCTION__);


    LOGI("IN: %s, eglGetConfigs return %i items.", __FUNCTION__, configedNum);

    int eglVer;
    for (eglVer = GLES_VERSION; eglVer >= 2; eglVer--)
    {
        if (isConfigfound)
            break;

        for (int i = 0; i < configedNum; i++)
        {
            if (isConfigfound)
                break;

            EGLint attr = 0;
            eglGetConfigAttrib(display, configs[i], EGL_RENDERABLE_TYPE, &attr);

            //wrong type, egnore
            if ((eglVer == 3) && (attr & EGL_OPENGL_ES3_BIT_KHR) != EGL_OPENGL_ES3_BIT_KHR)
                continue;
            if ((eglVer == 2) && (attr & EGL_OPENGL_ES2_BIT) != EGL_OPENGL_ES2_BIT)
                continue;

            eglGetConfigAttrib(display, configs[i], EGL_SURFACE_TYPE, &attr);
            //EGL_PBUFFER_BIT portable check
            if ((attr & EGL_WINDOW_BIT) && (attr & EGL_PBUFFER_BIT))
            {
                LOGI("IN: %s, Find out one colour config.", __FUNCTION__);
            }
            else
            {
                continue;
            }
            //check for colourConfigAttrs
            for (int j = 0; EGL_NONE != colourConfigAttrs[j]; j += 2)
            {
                EGLint tmpAttr = 0;
                eglGetConfigAttrib(display, configs[i], colourConfigAttrs[j], &tmpAttr);
                LOGI("IN: %s, eglGetConfigAttrib(colourConfigAttrs[j]) return: %i.", __FUNCTION__, tmpAttr);
                if (colourConfigAttrs[j + 1] != tmpAttr)
                    break;
                //when there is the last value finished check, it should be the target one config item
                if (colourConfigAttrs[j + 2] == EGL_NONE)
                {
                    //find target configs[i] fro colour config
                    env_mConfig = configs[i];
                    isConfigfound = true;
                    break;
                }
            }
        }
    }

    //check ret for config
    LOGI("IN: %s, colour config find return %s.", __FUNCTION__, isConfigfound ? "true" : "false");
    if (env_mConfig == nullptr)
    {
        LOGE("IN: %s, Not find acceptable EGL color config!", __FUNCTION__);
        return false;
    }

    //create context
    for (eglVer = GLES_VERSION; eglVer >= 2; eglVer--)
    {
        LOGI("try eglVer %d", eglVer);
        EGLint contextAttrs[] = {
                EGL_CONTEXT_CLIENT_VERSION,
                eglVer,
                EGL_CONTEXT_PRIORITY_LEVEL_IMG,
                static_cast<EGLint>(EGL_CONTEXT_PRIORITY_MEDIUM_IMG),EGL_NONE
                };

        contextAttrs[2] = EGL_NONE;
        contextAttrs[3] = EGL_NONE;

        env_mContext = eglCreateContext(display, env_mConfig, EGL_NO_CONTEXT, contextAttrs);
        if (env_mContext == EGL_NO_CONTEXT)
            continue;


        break;
    }

    //recheck context
    if (EGL_NO_CONTEXT == env_mContext)
    {
        LOGE("IN: %s, Create Context fail, eglerr: %s", __FUNCTION__, getEglErrorChars());
        return false;
    }

    config = env_mConfig;
    context = env_mContext;
    return true;
}