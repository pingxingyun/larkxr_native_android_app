#include <jni.h>
#include <cstring>
#include <env_context.h>
#include <build_config.h>
#include "World.cpp"
#include "nibiru/NibiruVRApi.h"
#include "larkxr_client.h"
#include "gsxr/gsxr_dev.h"


using namespace nvr;

//Nibiru VR Service, provide VR Apis
static NibiruVRApi sNVRApi;
static LarkxrClient larkxrClient;
static GsxrDev gsxrDev;

void onDraw(nvr_Eye *eye) {

    //Each eye render
    for (int i = 0; i < EYE_NUM; i++) {
        nvr_Eye useEye = eye[i];
//        NLOGD("eye %d project %d %d %d %d;", useEye.viewPort[0], useEye.viewPort[1], useEye.viewPort[2], useEye.viewPort[3]);
        //Prepare draw eye on one eye
        int result = sNVRApi.nvr_PrepareDrawEye(useEye.pos);

        if (result >= 0) {
            larkxrClient.Draw(useEye);
            //Finish draw eye on one eye
            sNVRApi.nvr_FinishDrawEye(useEye.pos);
        }
    }
}

mat4 headpose;
nvr_Eye eye[EYE_NUM];
float position[3];
//memset(position, 0, sizeof(float) * 3);

void onDrawFrame() {

    //Call prepare draw to get head info and check if can render now
    int res = sNVRApi.nvr_PrepareDraw2(&headpose, position, eye);
    //Not ready for render
    if (res < 0) {
        NLOGD("nvr api is not ready");
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0, 0.5, 0.5, 1);
        return;
    }
    larkxrClient.Update();
    if (larkxrClient.media_ready()) {
        // 有新的帧再渲染
        if (!larkxrClient.has_new_frame()) {
            NLOGD("no new frame");
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glClearColor(0.0, 0.5, 0.5, 1);
            return;
        } else {
            onDraw(eye);
            larkxrClient.Finish();
        }
    } else {
        onDraw(eye);
        larkxrClient.Finish();
    }

//    glm::mat4 result = glm::translate(matrix44, position_larkxr);

    if (larkxrClient.media_ready()) {
        glm::quat rotation_larkxr = larkxrClient.cloud_tracking_.tracking.rotation;
        glm::vec3 position_larkxr = larkxrClient.cloud_tracking_.tracking.position;

        rotation_larkxr.x = rotation_larkxr.x;
        rotation_larkxr.y = rotation_larkxr.y;
        rotation_larkxr.z = rotation_larkxr.z;
        rotation_larkxr.w = -rotation_larkxr.w;

        glm::mat4 matrix44 = glm::mat4_cast(rotation_larkxr);
        sNVRApi.nvr_setFramePose(glm::value_ptr(matrix44));
//        sNVRApi.nvr_setFramePose(glm::value_ptr(larkxrClient.cloud_tracking_.tracking.rawPoseMatrix));
    } else {
        sNVRApi.nvr_setFramePose(headpose.value_ptr());
    }

    //Finish this frame draw
    sNVRApi.nvr_FinishDraw();
//    LOGV("draw_frame end");
}

extern "C" {

JNIEXPORT void JNICALL Java_com_pxy_larkxr_1app_1nolo_MyGLSurfaceView_initializeNative (
        JNIEnv *env, jobject obj, jobject surfaceview, jobject activity, jobject assets) {

    NLOGD("initialized native");

    //Init NVR Api in construct method of GLSurfaceView
    int res = sNVRApi.nvr_Initialize(env, surfaceview, activity);

    if (res == 0) {
        NLOGD("init nvr succ");
    } else {
        NLOGE("init nvr failed");
    }

    //Check the Nibiru OS support DTR
    bool support = sNVRApi.isDTRSupport();

    NLOGD("Support DTR: %d", support);

    int sdkVer[3];
    sNVRApi.nvr_getSDKVersion(sdkVer);
    NLOGD("SDK Version : V%d,ReleaseDate.%d, Revision.%d", sdkVer[0], sdkVer[1], sdkVer[2]);


    if (sNVRApi.nvr_IsSupportDirectRender()) {
        NLOGD("enable direct render");
        sNVRApi.nvr_SetEnableDirectRender(true);
    } else {
        NLOGD("no use direct render");
    }

    bool support6dof = sNVRApi.isSupport6Dof();
    NLOGD("Support 6DOF: %d", support6dof);
    //1=rotation.2=position
    sNVRApi.setTrackingMode(support6dof ? 2 : 1);

    NLOGD("init nvr service res: %d", res);
    gsxrDev.Init();
    larkxrClient.setGsxrDev(&gsxrDev);
    larkxrClient.setAssets(env, assets);
    larkxrClient.setNibiruVRApi(&sNVRApi);

    // 初始化 java 环境
    JavaVM *vm = NULL;
    env->GetJavaVM(&vm);
    larkxrClient.InitJava(vm);
    larkxrClient.OnCreate();
}

JNIEXPORT void JNICALL Java_com_pxy_larkxr_1app_1nolo_MyGLSurfaceView_onSurfaceCreatedNative(
        JNIEnv *env, jobject obj) {

    NLOGD("on surface created");

    //Notify NVR Api surface created
    sNVRApi.nvr_OnSurfaceCreated();

    // 初始化 gl 环境
    // onResume 时重复调用
    larkxrClient.InitGL();
    sNVRApi.nvr_HideGaze();
}

JNIEXPORT void JNICALL Java_com_pxy_larkxr_1app_1nolo_MyGLSurfaceView_onSurfaceDestroyed(
        JNIEnv *env, jobject obj) {

    NLOGD("on surface destroyed");

    larkxrClient.Close();
    larkxrClient.ShutdownGL();
}

JNIEXPORT void JNICALL Java_com_pxy_larkxr_1app_1nolo_MyGLSurfaceView_onSurfaceChangedNative(
        JNIEnv *env, jobject obj, jint width, jint height) {

    NLOGD("on surface changed");
    //Notify NVR Api surface changed
    sNVRApi.nvr_OnSurfaceChanged(width, height);

    float values[2];
    sNVRApi.nvr_GetCurrentNearFar(values);
    //0: near; 1: far
    float near = values[0];
    float far = values[1];

    NLOGI("current near: %f, far: %f", near, far);

    //Only set FAR to 100
    sNVRApi.nvr_UpdateNearFar(near, 100);

}

JNIEXPORT void JNICALL Java_com_pxy_larkxr_1app_1nolo_MyGLSurfaceView_onDrawFrameNative(
        JNIEnv *env, jobject obj) {
    onDrawFrame();
}

JNIEXPORT void JNICALL Java_com_pxy_larkxr_1app_1nolo_MyGLSurfaceView_onDetachWindowNative(
        JNIEnv *env, jobject obj) {

    NLOGD("on detach");

    //Notify NVR Api window detached
    sNVRApi.nvr_DetachWindow();
    //Destor NVR Api
    sNVRApi.nvr_Destory();
}

JNIEXPORT void JNICALL Java_com_pxy_larkxr_1app_1nolo_MyGLSurfaceView_onAttachWindowNative(
        JNIEnv *env, jobject obj) {
    NLOGD("on attach");
    //Notify NVR Api attach window
    sNVRApi.nvr_AttachWindow();

}

JNIEXPORT void JNICALL Java_com_pxy_larkxr_1app_1nolo_MyGLSurfaceView_onPauseNative(
        JNIEnv *env, jobject obj) {
    NLOGD("on pause");
    //Notify NVR Api on pause
    sNVRApi.nvr_OnPause();
    larkxrClient.OnPause();
}

JNIEXPORT void JNICALL Java_com_pxy_larkxr_1app_1nolo_MyGLSurfaceView_onResumeNative(
        JNIEnv *env, jobject obj) {

    NLOGD("on resume");
    //notify NVR Api resume
    sNVRApi.nvr_OnResume();
    larkxrClient.OnResume();
}

JNIEXPORT jint JNICALL Java_com_pxy_larkxr_1app_1nolo_MyGLSurfaceView_onKeyDownNative(
        JNIEnv *env, jobject obj, jint keycode) {

    //called by onKeyDown, use nvr_GetInputKey to get input keycode
    int nvr_key = sNVRApi.nvr_GetInputKey(keycode);

    if (nvr_key == NVR_KEY_ENTER) {
        return 1;
    } else if (nvr_key == NVR_KEY_BACK) {
        NLOGD("on nvr key down: BACK");
    } else if (nvr_key == NVR_KEY_UP) {
        NLOGD("on nvr key down: UP");
    } else if (nvr_key == NVR_KEY_DOWN) {
        NLOGD("on nvr key down: DOWN");
    } else if (nvr_key == NVR_KEY_LEFT) {
        NLOGD("on nvr key down: LEFT");
    } else if (nvr_key == NVR_KEY_RIGHT) {
        NLOGD("on nvr key down: RIGHT");
    } else if (nvr_key == NVR_KEY_NKEY) {
        NLOGD("on nvr key down: NKEY");
    } else if (nvr_key == NVR_KEY_NONE) {
        NLOGD("on nvr key down: NONE");
    }

    return 0;

}
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_larkxr_1app_1nolo_MyGLSurfaceView_setTestTextureId(JNIEnv *env, jobject thiz,
                                                          jint textureid) {
    larkxrClient.testTexutreId = textureid;
}

// ui 初始化环境
extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_larkxr_1app_1nolo_MainActivity_nativeInit(JNIEnv *env, jobject thiz, jobject am,
                                                 jstring internal_data_path_,
                                                 jstring external_data_path_) {
    const char *internalDataPath = env->GetStringUTFChars(internal_data_path_, 0);
    const char *externalDataPath = env->GetStringUTFChars(external_data_path_, 0);

    LOGI("MainActivity_init: call  Context::getInstance()->init");
    JavaVM* vm = nullptr;
    env->GetJavaVM(&vm);
    Context::Init(vm, env, am, thiz, internalDataPath, externalDataPath);
    LOGI("register WVR main when library loading");

    env->ReleaseStringUTFChars(internal_data_path_, internalDataPath);
    env->ReleaseStringUTFChars(external_data_path_, externalDataPath);

    BuildConfig::init(env, "com/pxy/larkxr_app_nolo/BuildConfig");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_larkxr_1app_1nolo_MyGLSurfaceView_nativeOnDestroy(JNIEnv *env, jobject thiz) {
    larkxrClient.OnDestory();
}