#include <jni.h>
#include <cstring>
#include <env_context.h>
#include <build_config.h>
#include "World.cpp"
#include "nibiru/NibiruVRApi.h"
#include "larkxr_client.h"
#include "xvisio_vr_utils.h"
#include "INxrSensorClient/INxrSensorClient.h"
#include "INxrSensorClient/NxrMoudle.h"

#define USE_NXR_SENSOR_CLIENT 1
//#define USE_PREDIT_POSE 1

using namespace nvr;

//Nibiru VR Service, provide VR Apis
static NibiruVRApi sNVRApi;
// INxrSensorClient ? sNVRApi ? which ?
static android::INxrSensorClient *nxr_sensor_client = NULL;
static LarkxrClient larkxrClient;
static NxrMoudle moudle;

static bool enterButtonPressed = false;
static bool backButtonPressed = false;

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

//    int renderMode = sNVRApi.nvr_GetRenderMode();
//    NLOGD("render mode %d;", renderMode);
//
    // why ???
//    bool isVideoMode = sNVRApi.nvr_IsVideoMode();
//    NLOGD("is video mode %d", isVideoMode);

    // 更新操作
    Input::InputState * inputState = Input::GetInputState();
    // reset all state.
    inputState[Input::RayCast_Hmd] = {};
    if (enterButtonPressed) {
        inputState[Input::RayCast_Hmd].enterShortPressed = true;
        // reset state;
        enterButtonPressed = false;
    }
    if (backButtonPressed) {
        inputState[Input::RayCast_Hmd].backShortPressed = true;
        backButtonPressed = false;
        // close
        larkxrClient.Close();
    }

    glm::quat rotation = {};
    glm::vec3 positoin = {};

#ifdef USE_NXR_SENSOR_CLIENT
#ifdef USE_PREDIT_POSE
    if (!larkxrClient.media_ready()) {
#endif
    NxrSharedData tracking = {};
    nxr_sensor_client->getTrackingData(&tracking);
    rotation.x = tracking.rotation[0];
    rotation.y = tracking.rotation[1];
    rotation.z = -tracking.rotation[2];
    rotation.w = tracking.rotation[3];

    positoin.x = tracking.translation[0];
    positoin.y = tracking.translation[1];
    positoin.z = tracking.translation[2];

    glm::mat4x4 flipz = glm::scale(glm::mat4x4(1), glm::vec3(1,1,-1));
    glm::mat4 rotateMat = glm::mat4_cast(rotation);
    rotateMat = flipz * rotateMat * flipz;
    rotation = glm::quat_cast(rotateMat);

//        LOGI("nxr sensor %f %f %f %f; %f %f %f",
//             rotation.x, rotation.y, rotation.z, rotation.w,
//             positoin.x, positoin.y, positoin.z);
#ifdef USE_PREDIT_POSE
    } else {
        // test use predit pose
        mat4 preDictivePose = {};
        sNVRApi.nvr_getPredictiveHeadPose(&preDictivePose, 50);
        glm::mat4 poseMat = xvisio::toGlm(preDictivePose);
        rotation = glm::quat_cast(poseMat);
    }
#endif
#else
    glm::mat4x4 flipz = glm::scale(glm::mat4x4(1), glm::vec3(1,1,-1));
        // mat4 headpose2;
        // sNVRApi.nvr_GetHeadPose(&headpose2);
        glm::mat4 poseMat = xvisio::toGlm(headpose);

        // poseMat = flipz * poseMat * flipz;

        //poseMat = glm::inverse(poseMat);
        rotation = glm::quat_cast(poseMat);
        // xvisio::matrixToQuat(headpose, rotation);
        positoin = glm::vec3(position[0], position[1], position[2]);

        LOGI("rotation %f %f %f %f; %f %f %f", rotation.x, rotation.y, rotation.z, rotation.w, positoin.x, positoin.y, positoin.z);
#endif

    larkxrClient.Update(rotation, positoin);
    if (larkxrClient.media_ready()) {
        // 有新的帧再渲染
        if (!larkxrClient.has_new_frame()) {
            // NLOGD("no new frame");
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
        // sNVRApi.nvr_setFramePose(headpose.value_ptr());
    }

    //Finish this frame draw
    sNVRApi.nvr_FinishDraw();
//    LOGV("draw_frame end");
}

extern "C" {

JNIEXPORT void JNICALL Java_com_pxy_larkxr_1app_1xvisio_MyGLSurfaceView_initializeNative(
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

//    sNVRApi.nvr_SetEnableDirectRender(true);
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

    bool supportMutiview = sNVRApi.nvr_IsSupportMultiView();
    NLOGD("Support mutiview: %d", supportMutiview);

    supportMutiview = sNVRApi.nvr_IsSupportMultiView2();
    NLOGD("Support mutiview2: %d", supportMutiview);

    bool enableMutiview = sNVRApi.nvr_IsEnableMultiView();
    NLOGD("Enable Mutiview: %d", enableMutiview);

//    sNVRApi.nvr_SetRenderMode(RENDER_MODE_CONTINUOUS);

    int renderMode = sNVRApi.nvr_GetRenderMode();
    NLOGD("render mode %d;", renderMode);

    // Warning Game Mode latency 70
//    sNVRApi.nvr_setGameMode(true);
//    NLOGD("set game mode");

    // Test HIGH QUALITY
    sNVRApi.nvr_setDisplayQuality(NVR_DISPLAY_QUALITY_HIGH);

    sNVRApi.nvr_SetEnableFPS(true);

    // sNVRApi.nvr_StopVideoMode();
    bool isVideoMode = sNVRApi.nvr_IsVideoMode();
    NLOGD("is video mode %d", isVideoMode);

    NLOGD("init nvr service res: %d", res);

#ifdef USE_NXR_SENSOR_CLIENT
    NLOGD("start init nxr client");
    moudle.open("libnxrsensor_client.so");
    nxr_sensor_client = (android::INxrSensorClient *)moudle.getNxrClient();
    NLOGD("init nxr sensor client %p", nxr_sensor_client);
    bool sensor_res = nxr_sensor_client->init();
    NLOGD("int nxr sensor res %d", sensor_res);
    bool is_6dof = nxr_sensor_client->is6Dof();

    NxrDisplayCalibration displayCalibration {};
    nxr_sensor_client->readDisplayCalibration(&displayCalibration);

    NLOGD("nxr display calibration; translation left %f %f %f; right %f %f %f;",
          displayCalibration.translation[0][0], displayCalibration.translation[0][1], displayCalibration.translation[0][2],
          displayCalibration.translation[1][0], displayCalibration.translation[1][1], displayCalibration.translation[1][2]);

    glm::vec3 vec_eye = {
            displayCalibration.translation[1][0] - displayCalibration.translation[0][0],
            displayCalibration.translation[1][1] - displayCalibration.translation[0][1],
            displayCalibration.translation[1][2] - displayCalibration.translation[0][2]
    };

    float eye_distance = glm::distance(glm::vec3(displayCalibration.translation[0][0], displayCalibration.translation[0][1], displayCalibration.translation[0][2]),
                                       glm::vec3(displayCalibration.translation[1][0], displayCalibration.translation[1][1], displayCalibration.translation[1][2]));

    NLOGD("eye distance 1 %f; 2 %f", eye_distance);

    NLOGD("nxr display calibration; rotation left all %f %f %f; %f %f %f; %f %f %f",
          displayCalibration.rotation[0][0], displayCalibration.rotation[0][1], displayCalibration.rotation[0][2], displayCalibration.rotation[0][3],
          displayCalibration.rotation[0][4], displayCalibration.rotation[0][5], displayCalibration.rotation[0][6], displayCalibration.rotation[0][7],
          displayCalibration.rotation[0][8]);
    NLOGD("nxr sensor is 6dof %d", is_6dof);
#endif

    NLOGD("start init larkxr");
    larkxrClient.setAssets(env, assets);
    larkxrClient.setNibiruVRApi(&sNVRApi);
#ifdef USE_NXR_SENSOR_CLIENT
    larkxrClient.set_nxr_sensor_client(nxr_sensor_client);
#endif

    // 初始化 java 环境
    JavaVM *vm = NULL;
    env->GetJavaVM(&vm);
    larkxrClient.InitJava(vm);
    larkxrClient.OnCreate();

    NLOGD("init larkxr finish");
}

JNIEXPORT void JNICALL Java_com_pxy_larkxr_1app_1xvisio_MyGLSurfaceView_onSurfaceCreatedNative(
        JNIEnv *env, jobject obj) {

    NLOGD("on surface created");

    //Notify NVR Api surface created
    sNVRApi.nvr_OnSurfaceCreated();

    // 初始化 gl 环境
    // onResume 时重复调用
    larkxrClient.InitGL();
    sNVRApi.nvr_HideGaze();

    // sNVRApi.nvr_SetRenderMode(NVR_MODE_NVR);
    // sNVRApi.nvr_StopVideoMode();
    // sNVRApi.nvr_SetRenderMode(NVR_MODE_NVR);
}

JNIEXPORT void JNICALL Java_com_pxy_larkxr_1app_1xvisio_MyGLSurfaceView_onSurfaceDestroyed(
        JNIEnv *env, jobject obj) {

    NLOGD("on surface destroyed");

    larkxrClient.Close();
    larkxrClient.ShutdownGL();
}

JNIEXPORT void JNICALL Java_com_pxy_larkxr_1app_1xvisio_MyGLSurfaceView_onSurfaceChangedNative(
        JNIEnv *env, jobject obj, jint width, jint height) {

    NLOGD("on surface changed width %d height %d;", width, height);
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

JNIEXPORT void JNICALL Java_com_pxy_larkxr_1app_1xvisio_MyGLSurfaceView_onDrawFrameNative(
        JNIEnv *env, jobject obj) {
    onDrawFrame();
}

JNIEXPORT void JNICALL Java_com_pxy_larkxr_1app_1xvisio_MyGLSurfaceView_onDetachWindowNative(
        JNIEnv *env, jobject obj) {

    NLOGD("on detach");

    //Notify NVR Api window detached
    sNVRApi.nvr_DetachWindow();
    //Destor NVR Api
    sNVRApi.nvr_Destory();
}

JNIEXPORT void JNICALL Java_com_pxy_larkxr_1app_1xvisio_MyGLSurfaceView_onAttachWindowNative(
        JNIEnv *env, jobject obj) {
    NLOGD("on attach");
    //Notify NVR Api attach window
    sNVRApi.nvr_AttachWindow();

}

JNIEXPORT void JNICALL Java_com_pxy_larkxr_1app_1xvisio_MyGLSurfaceView_onPauseNative(
        JNIEnv *env, jobject obj) {
    NLOGD("on pause");
    //Notify NVR Api on pause
    sNVRApi.nvr_OnPause();
    larkxrClient.OnPause();
}

JNIEXPORT void JNICALL Java_com_pxy_larkxr_1app_1xvisio_MyGLSurfaceView_onResumeNative(
        JNIEnv *env, jobject obj) {

    NLOGD("on resume");
    //notify NVR Api resume
    sNVRApi.nvr_OnResume();
    larkxrClient.OnResume();
}

JNIEXPORT jint JNICALL Java_com_pxy_larkxr_1app_1xvisio_MyGLSurfaceView_onKeyDownNative(
        JNIEnv *env, jobject obj, jint keycode) {

    //called by onKeyDown, use nvr_GetInputKey to get input keycode
    int nvr_key = sNVRApi.nvr_GetInputKey(keycode);

    if (nvr_key == NVR_KEY_ENTER) {
        NLOGD("on nvr key down: NVR_KEY_ENTER");
        enterButtonPressed = true;
        return 1;
    } else if (nvr_key == NVR_KEY_BACK) {
        NLOGD("on nvr key down: BACK");
        backButtonPressed = true;
        return 1;
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
Java_com_pxy_larkxr_1app_1xvisio_MyGLSurfaceView_setTestTextureId(JNIEnv *env, jobject thiz,
                                                           jint textureid) {
    larkxrClient.testTexutreId = textureid;
}

// ui 初始化环境
extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_larkxr_1app_1xvisio_MainActivity_nativeInit(JNIEnv *env, jobject thiz, jobject am,
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

    BuildConfig::init(env, "com/pxy/larkxr_app_xvisio/BuildConfig");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_larkxr_1app_1xvisio_MyGLSurfaceView_nativeOnDestroy(JNIEnv *env, jobject thiz) {
    larkxrClient.OnDestory();

#ifdef USE_NXR_SENSOR_CLIENT
    if (nxr_sensor_client != nullptr) {
        nxr_sensor_client->destroy();
        nxr_sensor_client = nullptr;
    }
#endif
}