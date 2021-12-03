#include "xr_demo.h"
#include "matrix_functions.h"
#include "gl_function_ext.h"
#include "logger.h"
#include "egl_environment.h"
#include <time.h>
#include <string.h>
#include <log.h>
#include <string>
#include <ctime>
#include <thread>
#include <cmath>
#include <ui/navigation.h>
#include "utils.h"
#include "env_context.h"
//#include "windows.h"


clock_t start_t = clock();
clock_t end_t;
int intTest = 1;
using namespace std;
bool sessionRunning = false;
bool stateBegeinSession = false;
bool stateEndSession = false;


typedef struct {
    double w, x, y, z;
} Quaternion;

typedef struct {
    double roll, pitch, yaw;
} EulerAngles;


EulerAngles ToEulerAngles(Quaternion q) {
    EulerAngles angles;

    // roll (x-axis rotation)
    double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
    double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
    angles.roll = std::atan2(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    double sinp = 2 * (q.w * q.y - q.z * q.x);
    if (std::abs(sinp) >= 1)
        angles.pitch = std::copysign(M_PI / 2, sinp); // use 90 degrees if out of range
    else
        angles.pitch = std::asin(sinp);

    // yaw (z-axis rotation)
    double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
    double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
    angles.yaw = std::atan2(siny_cosp, cosy_cosp);

    angles.roll = angles.roll / M_PI * 180;
    angles.pitch = angles.pitch / M_PI * 180;
    angles.yaw = angles.yaw / M_PI * 180;
    return angles;
}


#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "XRDemo"

XrDemo *gApp = nullptr;

AppState g_AppState = ON_RESUME;

extern "C" {
void Java_com_pxy_xr_1app_1huawei_MainActivity_nativeInit(JNIEnv *jni, jclass clazz, jint x, jint y,
                                                          jint texid, jobject act,
                                                          jobject surface) {
    //LOGI("init vr nativeOnCreate");
    if (nullptr == gApp) {
        LOGI("init vr");
        JavaVM *_vm = nullptr;
        jint ret = jni->GetJavaVM(&_vm);
        //LOGI("Create global ref");

        jobject g_act = jni->NewGlobalRef(act);

        gApp = new XrDemo(_vm, g_act, jni);
        //gApp->InitGLGraphics();
        gApp->setSurface(jni, surface);
        gApp->initSkyboxMethod(jni);
        gApp->start();
    } else {
        //LOGI("resume vr");
        g_AppState = ON_RESUME;
        gApp->setSurface(jni, surface);
        stateBegeinSession = true;
    }
}

void Java_com_pxy_xr_1app_1huawei_MainActivity_uninit(JNIEnv *env, jclass clazz) {
    //LOGI("in uninit");
    if (gApp == nullptr) {
        //LOGI("gApp == nullptr");
        return;
    }
    g_AppState = ON_PAUSE;
    xrRequestExitSession(gApp->mSession);
    //LOGI("out uninit");
}

void Java_com_pxy_xr_1app_1huawei_MainActivity_nativeDestroy(JNIEnv *env, jclass clazz) {
    //LOGI("vr nativeOnDestroy");
    g_AppState = ON_DESTROY;
    if (nullptr != gApp) {
        gApp->stop();

        delete gApp;
        gApp = nullptr;
    }
    //LOGI("uninit vr nativeOnDestroy end");
}
}

XrDemo::XrDemo(JavaVM *_vm, jobject act, JNIEnv *_Env) :
        mNativeWindow(nullptr) {
    mActivity = act;
    mJvm = _vm;
    Env = _Env;
    mInstanceLossState = false;
    mInstanceLossStateTime = 0.0;
}

XrDemo::~XrDemo() {
    if (mNativeWindow != nullptr) {
        ANativeWindow_release(mNativeWindow);
        mNativeWindow = nullptr;
    }

}

void XrDemo::XrDemoBeginSession() {
    XrSessionBeginInfo sessionBeginInfo{XR_TYPE_SESSION_BEGIN_INFO};
    sessionBeginInfo.primaryViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO; //XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO; XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO
    XrResult retxrBeginSession = xrBeginSession(gApp->mSession, &sessionBeginInfo);
    //LOGI("retxrBeginSession :%d", retxrBeginSession);
    sessionRunning = true;
}


void XrDemo::InitGLGraphics() {

    //Step1 Setup Egl Environment
    //LOGI("Setup Egl Environment");
    //glm::mat4 Model = glm::mat4(1.0f);

    initEGLEnvironment(mDisplay, mConfig, mContext);
    initEGLSurface();

    initGLExtFunctionPointer();
    //Step2 App init
    //LOGI("App init sksk");

    xr_client_ = std::make_shared<lark::XRClient>();
    xr_client_->Init(mJvm);
    xr_client_->RegisterObserver(this);

    // 初始化客户端接入凭证
    LOGE("初始化客户端接入凭证");
    InitCertificate();
#ifdef LARK_SDK_SECRET
    // 初始化 cloudlark sdk
    std::string timestamp = utils::GetTimestampMillStr();
    std::string signature = utils::GetSignature(LARK_SDK_ID, LARK_SDK_SECRET, timestamp);
    if (!xr_client_->InitSdkAuthorization(LARK_SDK_ID, signature, timestamp)) {
        LOGV("init sdk auth faild %d %s", xr_client_->last_error_code(),
             xr_client_->last_error_message().c_str());
        Navigation::ShowToast(xr_client_->last_error_message());
    }
#else
    if (!xr_client_->InitSdkAuthorization(LARK_SDK_ID)) {
                        LOGV("init sdk auth faild %d %s", xr_client_->last_error_code(), xr_client_->last_error_message().c_str());
                        Navigation::ShowToast(xr_client_->last_error_message());
                    }
#endif

    xr_client_->SetServerAddr("192.168.31.15", 8181);
    xr_client_->EnterAppli("756846918545440768");
    rect_render_ = std::make_shared<RectTexture>();
    lark::XRConfig::use_multiview = false;
    lark::XRConfig::fps = 72;
    lark::XRConfig::render_width = 1920;
    lark::XRConfig::render_height = 1080;

    mSkyboxShader.build();
    mSkyboxShader.use();
    mSkyboxShader.setInt("Texture0", 0);

    mSkyboxModel.build();

    mSkyboxShader.buildCube();
    mSkyboxShader.useCube();
    mSkyboxShader.setIntCube("Texture0", 0);
    mSkyboxShader.setIntCubeName("u_cameraPos");

    mSkyboxModel.buildCube();
    mSkyboxTexture.build();
}

void XrDemo::DeInitGLGraphics() {
    //LOGI("onThreadExit");
    mSkyboxShader.unBuild();
    //LOGI("mSkyboxShader.unBuild()");

    mSkyboxModel.unBuild();
    //LOGI("mSkyboxModel.unBuild()");

    mSkyboxTexture.unBuild();
    //LOGI("mSkyboxTexture.unBuild()");

    mSkyboxShader.unBuildCube();
    //LOGI("mSkyboxShader.unBuildCube()");

    mSkyboxModel.unBuildCube();
    //LOGI("mSkyboxModel.unBuild()");

    mSkyboxTexture.unBuild();
    //LOGI("mSkyboxTexture.unBuild()");

    //DeinitEGLEnvironment(mDisplay, mConfig, mContext);

    //DeinitGLExtFunctionPointer();
}

void XrDemo::CreateInstance() {
    //LOGI("XrEnumerate");
    uint32_t apiLayerPropCount;
    int ret = xrEnumerateApiLayerProperties(0, &apiLayerPropCount, nullptr);
    //LOGI("EnumerateApiLayerProperties %d", ret);
    std::vector<XrApiLayerProperties> apiProperties;
    apiProperties.resize(apiLayerPropCount, {XR_TYPE_API_LAYER_PROPERTIES});
    ret = xrEnumerateApiLayerProperties(apiLayerPropCount, &apiLayerPropCount,
                                        apiProperties.data());
    //LOGI("EnumerateApiLayerProperties 2 %d", ret);
    //return;

    uint32_t insExtensionCount;
    ret = xrEnumerateInstanceExtensionProperties(nullptr, 0, &insExtensionCount, nullptr);
    //return;
    //LOGI("EnumerateInstanceExtensionProperties %d", ret);
    std::vector<XrExtensionProperties> extProperties;
    extProperties.resize(insExtensionCount, {XR_TYPE_EXTENSION_PROPERTIES});
    xrEnumerateInstanceExtensionProperties(nullptr, insExtensionCount, &insExtensionCount,
                                           extProperties.data());
    //LOGI("extProperties.size():%d", extProperties.size());
    for (int i = 0; i < extProperties.size(); i++) {
        //LOGI("ExtProps: %s %d", extProperties[i].extensionName, extProperties[i].extensionVersion);
    }

    //LOGI("XrCreateInstance");
    XrInstanceCreateInfoAndroidKHR createInfoAndroid{XR_TYPE_INSTANCE_CREATE_INFO_ANDROID_KHR};
    createInfoAndroid.applicationActivity = (void *) mActivity;
    createInfoAndroid.applicationVM = (void *) mJvm;
    std::vector<const char *> extensions;
    extensions.push_back(XR_KHR_ANDROID_CREATE_INSTANCE_EXTENSION_NAME);
    extensions.push_back(XR_KHR_OPENGL_ES_ENABLE_EXTENSION_NAME);

    XrInstanceCreateInfo instanceCreateInfo{XR_TYPE_INSTANCE_CREATE_INFO};
    instanceCreateInfo.next = (const void *) &createInfoAndroid;
    instanceCreateInfo.enabledExtensionCount = (uint32_t) extensions.size();
    instanceCreateInfo.enabledExtensionNames = extensions.data();
    strcpy(instanceCreateInfo.applicationInfo.applicationName, "HelloXR");
    instanceCreateInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;
    ret = xrCreateInstance(&instanceCreateInfo, &mInstance);
    //LOGI("xrCreateInstance %d", ret);
    //return;
    //Test instanceProperties
    //LOGI("before xrGetInstanceProperties");
    XrInstanceProperties instanceProperties{XR_TYPE_INSTANCE_PROPERTIES};
    xrGetInstanceProperties(mInstance, &instanceProperties);
    //LOGI("after demo xrGetInstanceProperties");
}

void XrDemo::CreateActions() {
    // add .
    //LOGI("In CreateActions");
    // Create an action set.
    {
        LOGW("Create an action set_1");
        XrActionSetCreateInfo actionSetInfo{XR_TYPE_ACTION_SET_CREATE_INFO};
        strcpy(actionSetInfo.actionSetName, "gameplay");
        strcpy(actionSetInfo.localizedActionSetName, "Gameplay");
        actionSetInfo.priority = 0;
        xrCreateActionSet(mInstance, &actionSetInfo, &m_actionSet);
    }

    xrStringToPath(mInstance, "/user/hand/left", &LRPath[0]);
    xrStringToPath(mInstance, "/user/hand/right", &LRPath[1]);
    xrStringToPath(mInstance, "/user/head", &headPath);

    // Create actions.
    {
        // Create an input action for grabbing objects with the left and right hands.
        LOGW("Create an action_1");
        XrActionCreateInfo actionInfo{XR_TYPE_ACTION_CREATE_INFO};

        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy(actionInfo.actionName, "home");
        strcpy(actionInfo.localizedActionName, "Home");
        actionInfo.countSubactionPaths = 2;
        actionInfo.subactionPaths = LRPath;
        xrCreateAction(m_actionSet, &actionInfo, &m_XrAction[0]);

        // Create an input action for squeeze objects with the left and right hands.

        actionInfo.actionType = XR_ACTION_TYPE_FLOAT_INPUT;
        strcpy(actionInfo.actionName, "trigger");
        strcpy(actionInfo.localizedActionName, "Trigger");
        actionInfo.countSubactionPaths = 2;
        actionInfo.subactionPaths = LRPath;
        xrCreateAction(m_actionSet, &actionInfo, &m_XrAction[1]);

        actionInfo.actionType = XR_ACTION_TYPE_VECTOR2F_INPUT;
        strcpy(actionInfo.actionName, "trackpad");
        strcpy(actionInfo.localizedActionName, "Trackpad");
        actionInfo.countSubactionPaths = 2;
        actionInfo.subactionPaths = LRPath;
        xrCreateAction(m_actionSet, &actionInfo, &m_XrAction[2]);

        actionInfo.actionType = XR_ACTION_TYPE_POSE_INPUT;
        strcpy(actionInfo.actionName, "pose");
        strcpy(actionInfo.localizedActionName, "Pose");
        actionInfo.countSubactionPaths = 2;
        actionInfo.subactionPaths = LRPath;
        xrCreateAction(m_actionSet, &actionInfo, &m_XrAction[3]);


        // Create an output action getting the left and right hand Vibrate.
        actionInfo.actionType = XR_ACTION_TYPE_VIBRATION_OUTPUT;
        strcpy(actionInfo.actionName, "vibrate");
        strcpy(actionInfo.localizedActionName, "Vibrate");
        actionInfo.countSubactionPaths = 2;
        actionInfo.subactionPaths = LRPath;
        xrCreateAction(m_actionSet, &actionInfo, &m_XrAction[4]);

        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy(actionInfo.actionName, "back");
        strcpy(actionInfo.localizedActionName, "Back");
        actionInfo.countSubactionPaths = 2;
        actionInfo.subactionPaths = LRPath;
        xrCreateAction(m_actionSet, &actionInfo, &m_XrAction[5]);

        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy(actionInfo.actionName, "volume_up");
        strcpy(actionInfo.localizedActionName, "Volume_up");
        actionInfo.countSubactionPaths = 2;
        actionInfo.subactionPaths = LRPath;
        xrCreateAction(m_actionSet, &actionInfo, &m_XrAction[6]);

        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy(actionInfo.actionName, "volume_down");
        strcpy(actionInfo.localizedActionName, "Volume_down");
        actionInfo.countSubactionPaths = 2;
        actionInfo.subactionPaths = LRPath;
        xrCreateAction(m_actionSet, &actionInfo, &m_XrAction[7]);

        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy(actionInfo.actionName, "trackpad_click");
        strcpy(actionInfo.localizedActionName, "Trackpad_click");
        actionInfo.countSubactionPaths = 2;
        actionInfo.subactionPaths = LRPath;
        xrCreateAction(m_actionSet, &actionInfo, &m_XrAction[8]);

        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy(actionInfo.actionName, "trigger_click");
        strcpy(actionInfo.localizedActionName, "Trigger_click");
        actionInfo.countSubactionPaths = 2;
        actionInfo.subactionPaths = LRPath;
        xrCreateAction(m_actionSet, &actionInfo, &m_XrAction[9]);

        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy(actionInfo.actionName, "trackpad_touch");
        strcpy(actionInfo.localizedActionName, "Trackpad_touch");
        actionInfo.countSubactionPaths = 2;
        actionInfo.subactionPaths = LRPath;
        xrCreateAction(m_actionSet, &actionInfo, &m_XrAction[10]);

        actionInfo.actionType = XR_ACTION_TYPE_FLOAT_INPUT;
        strcpy(actionInfo.actionName, "trackpad_touch_x");
        strcpy(actionInfo.localizedActionName, "Trackpad_touch_x");
        actionInfo.countSubactionPaths = 2;
        actionInfo.subactionPaths = LRPath;
        xrCreateAction(m_actionSet, &actionInfo, &m_XrAction[11]);

        actionInfo.actionType = XR_ACTION_TYPE_FLOAT_INPUT;
        strcpy(actionInfo.actionName, "trackpad_touch_y");
        strcpy(actionInfo.localizedActionName, "Trackpad_touch_y");
        actionInfo.countSubactionPaths = 2;
        actionInfo.subactionPaths = LRPath;
        xrCreateAction(m_actionSet, &actionInfo, &m_XrAction[12]);

    }

    {
        xrStringToPath(mInstance, "/user/hand/left/input/home/click", &HomePath[0]);
        xrStringToPath(mInstance, "/user/hand/right/input/home/click", &HomePath[1]);
        xrStringToPath(mInstance, "/user/hand/left/input/back/click", &BackPath[0]);
        xrStringToPath(mInstance, "/user/hand/right/input/back/click", &BackPath[1]);
        xrStringToPath(mInstance, "/user/hand/left/input/volume_up/click", &VolumeUpPath[0]);
        xrStringToPath(mInstance, "/user/hand/right/input/volume_up/click", &VolumeUpPath[1]);
        xrStringToPath(mInstance, "/user/hand/left/input/volume_down/click", &VolumeDownPath[0]);
        xrStringToPath(mInstance, "/user/hand/right/input/volume_down/click", &VolumeDownPath[1]);

        xrStringToPath(mInstance, "/user/hand/left/input/trigger/value", &TriggerValuePath[0]);
        xrStringToPath(mInstance, "/user/hand/right/input/trigger/value", &TriggerValuePath[1]);
        xrStringToPath(mInstance, "/user/hand/left/input/trigger/click", &TriggerClickPath[0]);
        xrStringToPath(mInstance, "/user/hand/right/input/trigger/click", &TriggerClickPath[1]);
        xrStringToPath(mInstance, "/user/hand/left/input/trackpad/value", &TrackpadValuePath[0]);
        xrStringToPath(mInstance, "/user/hand/right/input/trackpad/value", &TrackpadValuePath[1]);
        xrStringToPath(mInstance, "/user/hand/left/input/trackpad/x", &TrackpadXValuePath[0]);
        xrStringToPath(mInstance, "/user/hand/right/input/trackpad/x", &TrackpadXValuePath[1]);
        xrStringToPath(mInstance, "/user/hand/left/input/trackpad/y", &TrackpadYValuePath[0]);
        xrStringToPath(mInstance, "/user/hand/right/input/trackpad/y", &TrackpadYValuePath[1]);
        xrStringToPath(mInstance, "/user/hand/left/input/trackpad/click", &TrackpadClickPath[0]);
        xrStringToPath(mInstance, "/user/hand/right/input/trackpad/click", &TrackpadClickPath[1]);
        xrStringToPath(mInstance, "/user/hand/left/input/trackpad/touch", &TrackpadTouchPath[0]);
        xrStringToPath(mInstance, "/user/hand/right/input/trackpad/touch", &TrackpadTouchPath[1]);
        xrStringToPath(mInstance, "/user/hand/left/input/aim/pose", &PoseValuePath[0]);
        xrStringToPath(mInstance, "/user/hand/right/input/aim/pose", &PoseValuePath[1]);
        xrStringToPath(mInstance, "/user/hand/left/output/haptic", &HapticPath[0]);
        xrStringToPath(mInstance, "/user/hand/right/output/haptic", &HapticPath[1]);
    }
    // Suggest bindings for KHR Simple.
    {
        LOGW("Suggest bindings for KHR Simple");
        xrStringToPath(mInstance, "/interaction_profiles/huawei/controller",
                       &khrSimpleInteractionProfilePath);
        std::vector<XrActionSuggestedBinding> bindings;
        bindings.push_back({m_XrAction[0], HomePath[0]});
        bindings.push_back({m_XrAction[0], HomePath[1]});
        bindings.push_back({m_XrAction[1], TriggerValuePath[0]});
        bindings.push_back({m_XrAction[1], TriggerValuePath[1]});
        bindings.push_back({m_XrAction[2], TrackpadValuePath[0]});
        bindings.push_back({m_XrAction[2], TrackpadValuePath[1]});
        bindings.push_back({m_XrAction[3], PoseValuePath[0]});
        bindings.push_back({m_XrAction[3], PoseValuePath[1]});
        bindings.push_back({m_XrAction[4], HapticPath[0]});
        bindings.push_back({m_XrAction[4], HapticPath[1]});
        bindings.push_back({m_XrAction[5], BackPath[0]});
        bindings.push_back({m_XrAction[5], BackPath[1]});
        bindings.push_back({m_XrAction[6], VolumeUpPath[0]});
        bindings.push_back({m_XrAction[6], VolumeUpPath[1]});
        bindings.push_back({m_XrAction[7], VolumeDownPath[0]});
        bindings.push_back({m_XrAction[7], VolumeDownPath[1]});
        bindings.push_back({m_XrAction[8], TrackpadClickPath[0]});
        bindings.push_back({m_XrAction[8], TrackpadClickPath[1]});
        bindings.push_back({m_XrAction[9], TriggerClickPath[0]});
        bindings.push_back({m_XrAction[9], TriggerClickPath[1]});
        bindings.push_back({m_XrAction[10], TrackpadTouchPath[0]});
        bindings.push_back({m_XrAction[10], TrackpadTouchPath[1]});
        bindings.push_back({m_XrAction[11], TrackpadXValuePath[0]});
        bindings.push_back({m_XrAction[11], TrackpadXValuePath[1]});
        bindings.push_back({m_XrAction[12], TrackpadYValuePath[0]});
        bindings.push_back({m_XrAction[12], TrackpadYValuePath[1]});


        XrInteractionProfileSuggestedBinding suggestedBindings{
                XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING};  // XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING  XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED
        suggestedBindings.interactionProfile = khrSimpleInteractionProfilePath;
        suggestedBindings.suggestedBindings = bindings.data();
        suggestedBindings.countSuggestedBindings = (uint32_t) bindings.size();
        XrResult ret_xrSuggest = xrSuggestInteractionProfileBindings(mInstance, &suggestedBindings);
        //LOGI("ret_xrSuggest:%d", ret_xrSuggest);

    }

    //xrCreateActionSpace
    {
        //LOGW("xrCreateActionSpace");
        XrActionSpaceCreateInfo actionSpaceInfo{XR_TYPE_ACTION_SPACE_CREATE_INFO};
        //XrActionSpaceCreateInfo actionSpaceInfo{XR_TYPE_SPACE_LOCATION};
        actionSpaceInfo.action = m_XrAction[3];
        actionSpaceInfo.poseInActionSpace.orientation.x = 0.0;
        actionSpaceInfo.poseInActionSpace.orientation.y = 0.0;
        actionSpaceInfo.poseInActionSpace.orientation.z = 0.0;
        actionSpaceInfo.poseInActionSpace.orientation.w = 1.0;

        actionSpaceInfo.poseInActionSpace.position.x = 0.0;
        actionSpaceInfo.poseInActionSpace.position.y = 0.0;
        actionSpaceInfo.poseInActionSpace.position.z = 0.0;

        actionSpaceInfo.subactionPath = LRPath[0];

        XrResult ret_xrCreateActionSpace_0 = xrCreateActionSpace(mSession, &actionSpaceInfo,
                                                                 &LRSpace[0]);
        LOGI("ret_xrCreateActionSpace_0:%d", ret_xrCreateActionSpace_0);

        actionSpaceInfo.subactionPath = LRPath[1];
        XrResult ret_xrCreateActionSpace_1 = xrCreateActionSpace(mSession, &actionSpaceInfo,
                                                                 &LRSpace[1]);
        LOGI("ret_xrCreateActionSpace_1:%d", ret_xrCreateActionSpace_1);
    }



    //xrAttachSessionActionSets
    {
        XrSessionActionSetsAttachInfo attachInfo{XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO};
        attachInfo.countActionSets = 1;
        attachInfo.actionSets = &m_actionSet;
        XrResult ret_xrAttach = xrAttachSessionActionSets(mSession, &attachInfo);
        //LOGI("ret_xrAttach:%d", ret_xrAttach);
    }



    // xrGetCurrentInteractionProfile
    {
        XrPath interactionProfilepath;
        XrInteractionProfileState interactionProfile{XR_TYPE_INTERACTION_PROFILE_STATE};
        XrResult ret_xrGetCurrent = xrGetCurrentInteractionProfile(mSession, LRPath[0],
                                                                   &interactionProfile);
        //LOGI("ret_xrGetCurrent:%d", ret_xrGetCurrent);

        //xrPathToString
        interactionProfilepath = interactionProfile.interactionProfile;
        uint32_t pathCount;
        xrPathToString(mInstance, interactionProfilepath, 0, &pathCount, nullptr);
        std::string pathStr(pathCount, '\0');
        xrPathToString(mInstance, interactionProfilepath, pathCount, &pathCount, &pathStr.front());
        pathStr.resize(pathCount - 1);
        //LOGI("ret interactionProfilepath pathStr: %s", &pathStr.front());
    }


}

void XrDemo::InitializeSystem() {
    LOGI("XrGetSystemId");
    XrFormFactor formFactor{XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY};

    XrSystemGetInfo systemInfo{XR_TYPE_SYSTEM_GET_INFO};
    systemInfo.formFactor = formFactor;
    xrGetSystem(mInstance, &systemInfo, &mSystemId);
}

void XrDemo::InitializeSession() {
    LOGI("XrGetSystemProps");
    XrSystemProperties properties{XR_TYPE_SYSTEM_PROPERTIES};
    xrGetSystemProperties(mInstance, mSystemId, &properties);

    uint32_t viewConfigTypeCount;
    xrEnumerateViewConfigurations(mInstance, mSystemId, 0, &viewConfigTypeCount, nullptr);

    std::vector<XrViewConfigurationType> viewConfigTypes(viewConfigTypeCount);
    xrEnumerateViewConfigurations(mInstance, mSystemId, viewConfigTypeCount, &viewConfigTypeCount,
                                  viewConfigTypes.data());


    //LOGI("XrGetViewConfigProps");
    XrViewConfigurationType viewConfigType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO; //XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO; XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO
    XrViewConfigurationProperties viewConfigProperties{XR_TYPE_VIEW_CONFIGURATION_PROPERTIES};
    int ret = xrGetViewConfigurationProperties(mInstance, mSystemId, viewConfigType,
                                               &viewConfigProperties);
    //LOGI("xrGetViewConfigurationProperties ret:%d", ret);

    //LOGI("XrEnumViewConfigViews");
    uint32_t viewCount;
    std::vector<XrViewConfigurationView> viewConfigViews;
    ret = xrEnumerateViewConfigurationViews(mInstance, mSystemId, viewConfigType, 0, &viewCount,
                                            nullptr);

    viewConfigViews.resize(viewCount, {XR_TYPE_VIEW_CONFIGURATION_VIEW});
    ret = xrEnumerateViewConfigurationViews(mInstance, mSystemId, viewConfigType, viewCount,
                                            &viewCount, viewConfigViews.data());
    //LOGI("xrEnumerateViewConfigurationViews ret:%d", ret);
    //LOGI("viewCount:%d", viewCount);

    //LOGI("XrEnumBlendModes");
    uint32_t blendModeCount;
    xrEnumerateEnvironmentBlendModes(mInstance, mSystemId, viewConfigType, 0, &blendModeCount,
                                     nullptr);

    std::vector<XrEnvironmentBlendMode> blendModes(blendModeCount);
    xrEnumerateEnvironmentBlendModes(mInstance, mSystemId, viewConfigType, blendModeCount,
                                     &blendModeCount, blendModes.data());

    //LOGI("XrGetOpenGLESGraphicsRequirementsKHR");
    XrGraphicsRequirementsOpenGLESKHR graphicsRequirements{
            XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_ES_KHR};
    xrGetOpenGLESGraphicsRequirementsKHR(mInstance, mSystemId, &graphicsRequirements);

    InitGLGraphics();
    //LOGI("XrGraphicsBindingOpenGLESAndroidKHR");
    XrGraphicsBindingOpenGLESAndroidKHR graphicBindingsGLES{
            XR_TYPE_GRAPHICS_BINDING_OPENGL_ES_ANDROID_KHR};
    graphicBindingsGLES.next = nullptr;
    graphicBindingsGLES.display = mDisplay;
    graphicBindingsGLES.config = mConfig;
    graphicBindingsGLES.context = mContext;
    //LOGI("XrCreateSession");
    XrSessionCreateInfo sessionCreateInfo{XR_TYPE_SESSION_CREATE_INFO};
    sessionCreateInfo.next = (const void *) &graphicBindingsGLES;
    sessionCreateInfo.systemId = mSystemId;
    xrCreateSession(mInstance, &sessionCreateInfo, &mSession);

    //CreateActions
    CreateActions();

    //LOGI("XrEnumSwapchainFormats");
    uint32_t formatCount;
    xrEnumerateSwapchainFormats(mSession, 0, &formatCount, nullptr);

    std::vector<int64_t> swapchainFormats(formatCount);
    xrEnumerateSwapchainFormats(mSession, formatCount, &formatCount, swapchainFormats.data());


    //LOGI("XrCreateSwapchain xrEnumerateSwapchainImages");
    mSwapchains.resize(viewCount);
    mSwapchainsImageArray.resize(viewCount);
    for (int i = 0; i < viewCount; i++) {
        XrSwapchainCreateInfo swapchainCreateInfo{XR_TYPE_SWAPCHAIN_CREATE_INFO};
        swapchainCreateInfo.createFlags = 0;
        swapchainCreateInfo.usageFlags =
                XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCreateInfo.format = GL_RGBA8;
        swapchainCreateInfo.sampleCount = 1;
        swapchainCreateInfo.width = viewConfigViews[i].recommendedImageRectWidth;
        swapchainCreateInfo.height = viewConfigViews[i].recommendedImageRectHeight;
        swapchainCreateInfo.faceCount = 1;
        swapchainCreateInfo.arraySize = 1;
        swapchainCreateInfo.mipCount = 1;

        xrCreateSwapchain(mSession, &swapchainCreateInfo, &(mSwapchains[i]));

        uint32_t imageCount;
        xrEnumerateSwapchainImages(mSwapchains[i], 0, &imageCount, nullptr);
        std::vector<XrSwapchainImageOpenGLESKHR> swapchainImages(imageCount);
        for (XrSwapchainImageOpenGLESKHR &image : swapchainImages) {
            image.type = XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_ES_KHR;
        }
        XrSwapchainImageBaseHeader *swapchainImageBase;
        swapchainImageBase = (reinterpret_cast<XrSwapchainImageBaseHeader *>(swapchainImages.data()));
        xrEnumerateSwapchainImages(mSwapchains[i], imageCount, &imageCount, swapchainImageBase);
        mSwapchainsImageArray[i] = swapchainImages;
    }
    //LOGI("XrEnumRefAppSpace");
    uint32_t refSpaceCount;
    xrEnumerateReferenceSpaces(mSession, 0, &refSpaceCount, nullptr);

    std::vector<XrReferenceSpaceType> refSpaces(refSpaceCount);
    xrEnumerateReferenceSpaces(mSession, refSpaceCount, &refSpaceCount, refSpaces.data());


    XrExtent2Df bounds;
    xrGetReferenceSpaceBoundsRect(mSession, XR_REFERENCE_SPACE_TYPE_LOCAL, &bounds);

    //LOGI("XrCreateAppSpace");
    XrReferenceSpaceCreateInfo referenceSpaceCreateInfo{XR_TYPE_REFERENCE_SPACE_CREATE_INFO};
    referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
    referenceSpaceCreateInfo.poseInReferenceSpace = {{0.0f, 0.0f, 0.0f, 1.0f},
                                                     {0.0f, 0.0f, 0.0f}};
    xrCreateReferenceSpace(mSession, &referenceSpaceCreateInfo, &mAppSpace);
}


void XrDemo::ProcessEvents(bool *exitRenderLoop, bool *sessionRunning) {
    ////LOGI("In ProcessEvents");
    *exitRenderLoop = false;

    XrEventDataBuffer buffer{XR_TYPE_EVENT_DATA_BUFFER};
    XrEventDataBaseHeader *header = reinterpret_cast<XrEventDataBaseHeader *>(&buffer);

    if (mInstanceLossState == true && (GetSeconds() - mStart) >= mInstanceLossStateTime) {
        //LOGI("*exitRenderLoop = true");
        *exitRenderLoop = true;
    }

    // Process all pending messages.
    while (xrPollEvent(mInstance, &buffer) != XR_EVENT_UNAVAILABLE) {
        //LOGI("xrPollEvent(mInstance, &buffer) != XR_EVENT_UNAVAILABLE");
        switch (header->type) {
            case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING: {
                //LOGI("XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING exitRenderLoop = true");
                const auto stateInstanceEvent = *reinterpret_cast<const XrEventDataInstanceLossPending *>(header);
                if (stateInstanceEvent.lossTime > 0) {
                    mStart = GetSeconds();
                    //LOGI("mStart: %f", mStart);
                    mInstanceLossState = true;
                    mInstanceLossStateTime = (double) (stateInstanceEvent.lossTime / 1000000000);
                    //LOGI("mInstanceLossStateTime: %f", mInstanceLossStateTime);
                    return;
                }

                *exitRenderLoop = true;
                return;
            }
            case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
                //LOGI("XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED");
                XrSessionState sessionState{XR_SESSION_STATE_UNKNOWN};
                const auto stateEvent = *reinterpret_cast<const XrEventDataSessionStateChanged *>(header);
                sessionState = stateEvent.state;
                switch (sessionState) {
                    case XR_SESSION_STATE_READY: {
                        //LOGI("XR_SESSION_STATE_READY");
                        XrSessionBeginInfo sessionBeginInfo{XR_TYPE_SESSION_BEGIN_INFO};
                        sessionBeginInfo.primaryViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO; //XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO; XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO

                        XrResult retxrBeginSessSw = xrBeginSession(mSession, &sessionBeginInfo);
                        //LOGI("retxrBeginSessSw: %d", retxrBeginSessSw);
                        *sessionRunning = true;
                        break;
                    }
                    case XR_SESSION_STATE_STOPPING: {
                        //LOGI("XR_SESSION_STATE_STOPPING");
                        *sessionRunning = false;
                        XrResult retxrEndSession = xrEndSession(mSession);
                        //LOGI("retxrEndSession: %d", retxrEndSession);
                        stateEndSession = true;
                        break;
                    }
                    case XR_SESSION_STATE_EXITING: {
                        //LOGI("XR_SESSION_STATE_EXITING");
                        if (g_AppState == ON_DESTROY) {
                            //LOGI("destroy");
                            *exitRenderLoop = true;
                        } else { //destroy
                            //LOGI("g_AppState: %d", g_AppState);
                            *exitRenderLoop = false;
                        }
                        break;
                    }
                    case XR_SESSION_STATE_LOSS_PENDING: {
                        // Poll for a new systemId
                        //LOGI("XR_SESSION_STATE_LOSS_PENDING exitRenderLoop = true");
                        *exitRenderLoop = true;
                        break;
                    }
                    default: {
                        //LOGI("default");
                        break;
                    }

                }
                break;
            }
            case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING: {
                //LOGI("XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING");
                break;
            }
            case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED: {
                //LOGI("XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED");
                break;
            }
            default: {
                //LOGI("Ignoring event type %d", header->type);
                break;
            }
        }
    }
}


void XrDemo::PollActions() {
    {
        //const XrActiveActionSet activeActionSet{m_actionSet, LRSpace[1]};
        const XrActiveActionSet activeActionSet{m_actionSet, XR_NULL_PATH};

        XrActionsSyncInfo syncInfo{XR_TYPE_ACTIONS_SYNC_INFO};
        syncInfo.countActiveActionSets = 1;
        syncInfo.activeActionSets = &activeActionSet;
        xrSyncActions(mSession, &syncInfo);
    }

    for (int i = 0; i < 1; i++) {
        const XrPath subactionLRPath = LRPath[i];

        XrActionStateBoolean Home{XR_TYPE_ACTION_STATE_BOOLEAN};
        XrActionStateGetInfo getInfo_home{XR_TYPE_ACTION_STATE_GET_INFO};
        getInfo_home.action = m_XrAction[0];
        getInfo_home.subactionPath = subactionLRPath;
        xrGetActionStateBoolean(mSession, &getInfo_home, &Home);
        ////LOGI("ID: %d Trigger_Home Home.currentSate:%d", i, Home.currentState);

        XrActionStateFloat Trigger{XR_TYPE_ACTION_STATE_FLOAT};
        XrActionStateGetInfo getInfo_trigger{XR_TYPE_ACTION_STATE_GET_INFO};
        getInfo_trigger.action = m_XrAction[1];
        getInfo_trigger.subactionPath = subactionLRPath;
        xrGetActionStateFloat(mSession, &getInfo_trigger, &Trigger);
        ////LOGI("ID: %d Trigger.currentState:%f", i, Trigger.currentState);


        XrActionStateVector2f TrackpadValue{XR_TYPE_ACTION_STATE_VECTOR2F};
        XrActionStateGetInfo getInfo_v2{XR_TYPE_ACTION_STATE_GET_INFO};
        getInfo_v2.action = m_XrAction[2];
        getInfo_v2.subactionPath = subactionLRPath;
        xrGetActionStateVector2f(mSession, &getInfo_v2, &TrackpadValue);
        ////LOGI("ID: %d TrackpadValue.currentState:%f , %f", i, TrackpadValue.currentState.x,
        //     TrackpadValue.currentState.y);


        XrActionStatePose PoseState{XR_TYPE_ACTION_STATE_POSE};
        XrActionStateGetInfo getInfo_p{XR_TYPE_ACTION_STATE_GET_INFO};
        getInfo_p.action = m_XrAction[3];
        getInfo_p.subactionPath = subactionLRPath;
        xrGetActionStatePose(mSession, &getInfo_p, &PoseState);
        LOGI("PoseState.isActive:%d", PoseState.isActive);

        XrHapticVibration vibration{XR_TYPE_HAPTIC_VIBRATION};
        vibration.amplitude = 0.5;
        vibration.duration = XR_MIN_HAPTIC_DURATION;
        vibration.frequency = XR_FREQUENCY_UNSPECIFIED;
        XrHapticActionInfo hapticActionInfo{XR_TYPE_HAPTIC_ACTION_INFO};
        hapticActionInfo.action = m_XrAction[4];
        hapticActionInfo.subactionPath = subactionLRPath;
        xrApplyHapticFeedback(mSession, &hapticActionInfo, (XrHapticBaseHeader *) &vibration);

        XrActionStateBoolean Back{XR_TYPE_ACTION_STATE_BOOLEAN};
        XrActionStateGetInfo getInfo_back{XR_TYPE_ACTION_STATE_GET_INFO};
        getInfo_back.action = m_XrAction[5];
        getInfo_back.subactionPath = subactionLRPath;
        xrGetActionStateBoolean(mSession, &getInfo_back, &Back);
        ////LOGI("ID: %d Back.currentSate:%d", i, Back.currentState);


        XrActionStateBoolean Volume_up{XR_TYPE_ACTION_STATE_BOOLEAN};
        XrActionStateGetInfo getInfo_volume_up{XR_TYPE_ACTION_STATE_GET_INFO};
        getInfo_volume_up.action = m_XrAction[6];
        getInfo_volume_up.subactionPath = subactionLRPath;
        xrGetActionStateBoolean(mSession, &getInfo_volume_up, &Volume_up);
        ////LOGI("ID: %d Volume_up.currentSate:%d", i, Volume_up.currentState);

        XrActionStateBoolean Volume_down{XR_TYPE_ACTION_STATE_BOOLEAN};
        XrActionStateGetInfo getInfo_volume_down{XR_TYPE_ACTION_STATE_GET_INFO};
        getInfo_volume_down.action = m_XrAction[7];
        getInfo_volume_down.subactionPath = subactionLRPath;
        xrGetActionStateBoolean(mSession, &getInfo_volume_down, &Volume_down);
        ////LOGI("ID: %d Volume_down.currentSate:%d", i, Volume_down.currentState);

        XrActionStateBoolean Trackpad_click{XR_TYPE_ACTION_STATE_BOOLEAN};
        XrActionStateGetInfo getInfo_trackpad_click{XR_TYPE_ACTION_STATE_GET_INFO};
        getInfo_trackpad_click.action = m_XrAction[8];
        getInfo_trackpad_click.subactionPath = subactionLRPath;
        xrGetActionStateBoolean(mSession, &getInfo_trackpad_click, &Trackpad_click);
        LOGI("ID: %d Trackpad_click.currentSate:%d", i, Trackpad_click.currentState);


        XrActionStateBoolean Trigger_click{XR_TYPE_ACTION_STATE_BOOLEAN};
        XrActionStateGetInfo getInfo_trigger_click{XR_TYPE_ACTION_STATE_GET_INFO};
        getInfo_trigger_click.action = m_XrAction[9];
        getInfo_trigger_click.subactionPath = subactionLRPath;
        xrGetActionStateBoolean(mSession, &getInfo_trigger_click, &Trigger_click);
        LOGI("ID: %d Trigger_Home Trigger_click.currentSate:%d", i, Trigger_click.currentState);

        if (Trackpad_click.currentState || Trigger_click.currentState) {
            //markclick
            mTrackingStateS->devicePair.controllerState[0].inputState.triggerValue = 1;
        }

        XrActionStateBoolean Trackpad_touch{XR_TYPE_ACTION_STATE_BOOLEAN};
        XrActionStateGetInfo getInfo_trackpad_touch{XR_TYPE_ACTION_STATE_GET_INFO};
        getInfo_trackpad_touch.action = m_XrAction[10];
        getInfo_trackpad_touch.subactionPath = subactionLRPath;
        xrGetActionStateBoolean(mSession, &getInfo_trackpad_touch, &Trackpad_touch);
        ////LOGI("ID: %d Trackpad_touch.currentSate:%d", i, Trackpad_touch.currentState);

        XrActionStateFloat Trackpad_X_Value{XR_TYPE_ACTION_STATE_FLOAT};
        XrActionStateGetInfo getInfo_trackpad_X_Value{XR_TYPE_ACTION_STATE_GET_INFO};
        getInfo_trackpad_X_Value.action = m_XrAction[11];
        getInfo_trackpad_X_Value.subactionPath = subactionLRPath;
        xrGetActionStateFloat(mSession, &getInfo_trackpad_X_Value, &Trackpad_X_Value);
        ////LOGI("ID: %d Trackpad_X_Value.currentState:%f", i, Trackpad_X_Value.currentState);


        XrActionStateFloat Trackpad_Y_Value{XR_TYPE_ACTION_STATE_FLOAT};
        XrActionStateGetInfo getInfo_trackpad_Y_Value{XR_TYPE_ACTION_STATE_GET_INFO};
        getInfo_trackpad_Y_Value.action = m_XrAction[12];
        getInfo_trackpad_Y_Value.subactionPath = subactionLRPath;
        xrGetActionStateFloat(mSession, &getInfo_trackpad_Y_Value, &Trackpad_Y_Value);
        ////LOGI("ID: %d Trackpad_Y_Value.currentState:%f", i, Trackpad_Y_Value.currentState);

        //xrStopHapticFeedback
        XrHapticActionInfo hapticActionInfoStop{XR_TYPE_HAPTIC_ACTION_INFO};
        hapticActionInfoStop.action = m_XrAction[4];
        hapticActionInfoStop.subactionPath = subactionLRPath;
        xrStopHapticFeedback(mSession, &hapticActionInfoStop);

    }


    //xrEnumerateBoundSourcesForAction
    {
        XrBoundSourcesForActionEnumerateInfo getInfoEnum = {
                XR_TYPE_BOUND_SOURCES_FOR_ACTION_ENUMERATE_INFO};
        getInfoEnum.action = m_XrAction[7];
        uint32_t pathCount = 0;
        xrEnumerateBoundSourcesForAction(mSession, &getInfoEnum, 0, &pathCount, nullptr);

        std::vector<XrPath> pathsxx(pathCount);
        xrEnumerateBoundSourcesForAction(mSession, &getInfoEnum, uint32_t(pathsxx.size()),
                                         &pathCount, pathsxx.data());

        //xrPathToString
        XrPath pathtemp = pathsxx[0];
        uint32_t pathCountX;
        xrPathToString(mInstance, pathsxx[0], 0, &pathCountX, nullptr);
        std::string pathStrX(pathCountX, '\0');
        xrPathToString(mInstance, pathsxx[0], pathCountX, &pathCountX, &pathStrX.front());
        pathStrX.resize(pathCountX - 1);
        //LOGI(" pathStrX: %s", &pathStrX.front());

        uint32_t pathCountXX;
        xrPathToString(mInstance, pathsxx[1], 0, &pathCountXX, nullptr);
        std::string pathStrXX(pathCountXX, '\0');
        xrPathToString(mInstance, pathsxx[1], pathCountXX, &pathCountXX, &pathStrXX.front());
        pathStrXX.resize(pathCountXX - 1);
        //LOGI(" pathStrXX: %s", &pathStrXX.front());


        std::string sourceName0, sourceName1;
        constexpr XrInputSourceLocalizedNameFlags all =
                XR_INPUT_SOURCE_LOCALIZED_NAME_USER_PATH_BIT |
                XR_INPUT_SOURCE_LOCALIZED_NAME_INTERACTION_PROFILE_BIT |
                XR_INPUT_SOURCE_LOCALIZED_NAME_COMPONENT_BIT;

        XrInputSourceLocalizedNameGetInfo nameInfo = {XR_TYPE_INPUT_SOURCE_LOCALIZED_NAME_GET_INFO};

        nameInfo.sourcePath = pathsxx[0];
        nameInfo.whichComponents = all;
        uint32_t nameInfoSize0 = 0;

        xrGetInputSourceLocalizedName(mSession, &nameInfo, 0, &nameInfoSize0, nullptr);
        std::vector<char> grabSource0(nameInfoSize0);
        xrGetInputSourceLocalizedName(mSession, &nameInfo, uint32_t(grabSource0.size()),
                                      &nameInfoSize0, grabSource0.data());
        sourceName0 += std::string(grabSource0.begin(), grabSource0.end());
        //LOGI(" sourceName0:%s", sourceName0.c_str());

        nameInfo.sourcePath = pathsxx[1];
        uint32_t nameInfoSize1 = 0;

        xrGetInputSourceLocalizedName(mSession, &nameInfo, 0, &nameInfoSize1, nullptr);
        std::vector<char> grabSource1(nameInfoSize1);
        xrGetInputSourceLocalizedName(mSession, &nameInfo, uint32_t(grabSource1.size()),
                                      &nameInfoSize1, grabSource1.data());
        sourceName1 += std::string(grabSource1.begin(), grabSource1.end());
        //LOGI(" sourceName1:%s", sourceName1.c_str());
    }


}

bool XrDemo::Run() {
    CreateInstance();
    InitializeSystem();
    InitializeSession();

    LOGI("In ThreadLoop");

    while (true) {
        bool exitRenderLoop = false;
        ProcessEvents(&exitRenderLoop, &sessionRunning);
        if (stateBegeinSession == true && stateEndSession == true) {
            initEGLSurface();
            //LOGI("stateBegeinSession == true && stateEndSession == true");
            XrDemoBeginSession();
            stateBegeinSession = false;
            stateEndSession = false;
        }
        if (exitRenderLoop) {
            //LOGI("exitRenderLoop");
            break;
        }

        ////LOGI("sessionRunning : %d",sessionRunning);
        if (sessionRunning) {
            PollActions();
            RenderFrame();
            //mRender = 1;
        } else {
            // //LOGI("session not Running");
            // Throttle loop since xrWaitFrame won't be called.
            timespec total_time;
            timespec left_time;
            total_time.tv_sec = 0;
            total_time.tv_nsec = (long) (250000000);
            nanosleep(&total_time, &left_time);
        }
    }

    DeInitializeSession();
    DestroyActions();
    DestroyInstance();

    DeInitGLGraphics();
    return true;
}

void XrDemo::DeInitializeSession() {
    //LOGI("DeInitializeSession");
    xrDestroySession(mSession);
    xrDestroySpace(mAppSpace);
    //add
    xrDestroySpace(LRSpace[0]);
    xrDestroySpace(LRSpace[1]);
    std::vector<XrSwapchain> mSwapchains;
    for (auto swapchain : mSwapchains) {
        xrDestroySwapchain(swapchain);
    }
}

void XrDemo::DestroyInstance() {
    //LOGI("DestroyInstance()");
    xrDestroyInstance(mInstance);
}

void XrDemo::RenderFrame() {
    //1. WaitFrame
    XrFrameWaitInfo frameWaitInfo{XR_TYPE_FRAME_WAIT_INFO};
    XrFrameState frameState{XR_TYPE_FRAME_STATE};
    xrWaitFrame(mSession, &frameWaitInfo, &frameState);

    //2. BeginFrame
    XrFrameBeginInfo frameBeginInfo{XR_TYPE_FRAME_BEGIN_INFO};
    xrBeginFrame(mSession, &frameBeginInfo);

    //3. Prepare Layers
    std::vector<XrCompositionLayerBaseHeader *> layers;

    //3.1 Projection Layer
    XrCompositionLayerProjection layer{XR_TYPE_COMPOSITION_LAYER_PROJECTION};

    //3.2 >>>Render Layer>>>
    RenderLayer(frameState.predictedDisplayTime, layer);

    //3.3 Push into Layers
    layers.push_back(reinterpret_cast<XrCompositionLayerBaseHeader *>(&layer));

    //4. EndFrame
    XrFrameEndInfo frameEndInfo{XR_TYPE_FRAME_END_INFO};
    frameEndInfo.displayTime = frameState.predictedDisplayTime;
    frameEndInfo.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_ALPHA_BLEND;
    frameEndInfo.layerCount = (uint32_t) layers.size();
    frameEndInfo.layers = layers.data();
    int ret = xrEndFrame(mSession, &frameEndInfo);
    //LOGI("xrEndFrame ret:%d", ret);

}


void XrDemo::RenderLayer(XrTime predictedDisplayTime, XrCompositionLayerProjection &layer) {
    std::vector<XrView> mViews;
    mViews.resize(2, {XR_TYPE_VIEW});
    //1. Locate Views

    XrViewLocateInfo viewLocateInfo{XR_TYPE_VIEW_LOCATE_INFO};
    viewLocateInfo.viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO; //XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO; XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO
    viewLocateInfo.displayTime = predictedDisplayTime;
    viewLocateInfo.space = mAppSpace;   //mView相对于世界坐标系

    XrViewState viewState{XR_TYPE_VIEW_STATE};
    uint32_t viewCapacityInput = 2;
    uint32_t viewCountOutput;

    xrLocateViews(mSession, &viewLocateInfo, &viewState, viewCapacityInput, &viewCountOutput,
                  mViews.data());
    ////LOGI("after xrLocateViews viewCountOutput:%d", viewCountOutput);

    //add
    XrSpaceLocation spaceLocation_l{XR_TYPE_SPACE_LOCATION};
    xrLocateSpace(LRSpace[0], mAppSpace, predictedDisplayTime, &spaceLocation_l);
/*    LOGI("spaceLocation_l_position:%f,%f,%f", spaceLocation_l.pose.position.x,
         spaceLocation_l.pose.position.y, spaceLocation_l.pose.position.z);*/
    LOGI("spaceLocation_l_orientation:%f,%f,%f,%f", spaceLocation_l.pose.orientation.x,
         spaceLocation_l.pose.orientation.y, spaceLocation_l.pose.orientation.z,
         spaceLocation_l.pose.orientation.w);

/*    XrSpaceLocation spaceLocation_r{XR_TYPE_SPACE_LOCATION};
    xrLocateSpace(LRSpace[1], mAppSpace, predictedDisplayTime, &spaceLocation_r);
    LOGI("spaceLocation_r_position:%f,%f,%f", spaceLocation_r.pose.position.x,
         spaceLocation_r.pose.position.y, spaceLocation_r.pose.position.z);
    LOGI("spaceLocation_r_orientation:%f,%f,%f,%f", spaceLocation_r.pose.orientation.x,
         spaceLocation_r.pose.orientation.y, spaceLocation_r.pose.orientation.z,
         spaceLocation_r.pose.orientation.w);*/

    Quaternion qTemp;
    qTemp.x = spaceLocation_l.pose.orientation.x;
    qTemp.y = spaceLocation_l.pose.orientation.y;
    qTemp.z = spaceLocation_l.pose.orientation.z;
    qTemp.w = spaceLocation_l.pose.orientation.w;

    //markcontroller
    mTrackingStateS[0].devicePair.controllerState[0].pose.rotation.x = spaceLocation_l.pose.orientation.x;
    mTrackingStateS[0].devicePair.controllerState[0].pose.rotation.y = spaceLocation_l.pose.orientation.y;
    mTrackingStateS[0].devicePair.controllerState[0].pose.rotation.z = spaceLocation_l.pose.orientation.z;
    mTrackingStateS[0].devicePair.controllerState[0].pose.rotation.w = spaceLocation_l.pose.orientation.w;

    EulerAngles angles;
    angles = ToEulerAngles(qTemp);
    //参考OpenXR aim  grip 坐标系的规定 向右为+X  向上为+Y 向后为+Z
    //LOGI("angles roll:%f, pitch:%f, yaw:%f", angles.roll, angles.pitch, angles.yaw);

    //2. Prepare LayerViews

    std::vector<XrCompositionLayerProjectionView> projectionLayerViews;
    projectionLayerViews.resize(viewCountOutput);


    //3. Swapchain
    //LOGI("viewCountOutput:%d", viewCountOutput);
    for (int i = 0; i < viewCountOutput; i++)   //viewCountOutput ==> 1
    {
        //3.1 Acquire Image
        uint32_t swapchainImageIndex;
        XrSwapchainImageAcquireInfo acquireInfo{XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};
        xrAcquireSwapchainImage(mSwapchains[i], &acquireInfo, &swapchainImageIndex);

        //3.2 Wait Image
        XrSwapchainImageWaitInfo waitInfo{XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};
        waitInfo.timeout = XR_INFINITE_DURATION;
        xrWaitSwapchainImage(mSwapchains[i], &waitInfo);

        //3.3 Prepare LayerViews
        projectionLayerViews[i] = {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW};
        projectionLayerViews[i].pose = mViews[i].pose;
        //LOGI("i = %d : pose xyz:%f,%f,%f", i, mViews[i].pose.position.x, mViews[i].pose.position.y,
        //     mViews[i].pose.position.z);
        //LOGI("i = %d : pose orientation:%f,%f,%f,%f", i, mViews[i].pose.orientation.x,
        //     mViews[i].pose.orientation.y, mViews[i].pose.orientation.z,
        //     mViews[i].pose.orientation.w);
        projectionLayerViews[i].fov = mViews[i].fov;
        projectionLayerViews[i].subImage.swapchain = mSwapchains[i];
        projectionLayerViews[i].subImage.imageRect.offset = {0, 0};
        projectionLayerViews[i].subImage.imageRect.extent = {1080, 1080};
        projectionLayerViews[i].subImage.imageArrayIndex = 0;
        const XrSwapchainImageBaseHeader *const swapchainImage = (XrSwapchainImageBaseHeader *) (&(mSwapchainsImageArray[i][swapchainImageIndex]));
        //3.4 >>>Graphic Render>>>
        renderView(projectionLayerViews[i], swapchainImage, spaceLocation_l.pose,
                   i);    //1st Render

        //3.5 Release Image
        XrSwapchainImageReleaseInfo releaseInfo{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
        xrReleaseSwapchainImage(mSwapchains[i], &releaseInfo);
    }

    //4. Make Layer
    layer.next = nullptr;
    layer.layerFlags = 0;
    layer.space = mAppSpace;
    layer.viewCount = (uint32_t) projectionLayerViews.size();
    //LOGI("layer.viewCount:%d", layer.viewCount);
    layer.views = projectionLayerViews.data();
}


void XrDemo::renderView(const XrCompositionLayerProjectionView &layerView,
                        const XrSwapchainImageBaseHeader *swapchainImage, const XrPosef &ctlPose,
                        int i) {
    const uint32_t colorTexture = reinterpret_cast<const XrSwapchainImageOpenGLESKHR *>(swapchainImage)->image;

    XrQuaternionf xrquat;
    XrVector3f xrvec;
    xrquat.x = layerView.pose.orientation.x;
    xrquat.y = layerView.pose.orientation.y;
    xrquat.z = layerView.pose.orientation.z;
    xrquat.w = layerView.pose.orientation.w;

    xrvec.x = layerView.pose.position.x;
    xrvec.y = layerView.pose.position.y;
    xrvec.z = layerView.pose.position.z;

    XrQuaternionf xrquatCtl;
    xrquatCtl.x = ctlPose.orientation.x;
    xrquatCtl.y = ctlPose.orientation.y;
    xrquatCtl.z = ctlPose.orientation.z;
    xrquatCtl.w = ctlPose.orientation.w;

    XrMatrix4x4f eyeViewMatrix = XrMatrix4x4f_CreateFromQuaternion_4(&xrquat, &xrvec);
    XrMatrix4x4f ctlMatrix = XrMatrix4x4f_CreateFromQuaternion(&xrquatCtl);
    XrMatrix4x4f eyeViewOffsetMatrix = XrMatrix4x4f_CreateOffset();

    const float tanAngleLeft = tanf(layerView.fov.angleLeft);
    const float tanAngleRight = tanf(layerView.fov.angleRight);
    const float tanAngleUp = tanf(layerView.fov.angleUp);
    float const tanAngleDown = tanf(layerView.fov.angleDown);

    XrMatrix4x4f projectionMatrix = XrMatrix4x4f_CreateProjection(tanAngleLeft, tanAngleRight,
                                                                  tanAngleUp, tanAngleDown, 0.1f,
                                                                  200.0f);  //最远最近距离
    //XrMatrix4x4f projectionMatrix = XrMatrix4x4f_CreateProjection(tanAngleLeft, tanAngleRight, tanAngleUp, tanAngleDown, 0.0f, 0.0f);

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_CUBE_MAP, mSkyboxTexture.mTextureId);

    mSkyboxShader.use();
    mSkyboxShader.setMat4("projection", &projectionMatrix.m[0][0]);
    mSkyboxShader.setMat4("view", &eyeViewMatrix.m[0][0]);
    mSkyboxModel.draw();

    glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, mSkyboxTexture.mTextureIdCube);
    mSkyboxShader.useCube();
    mSkyboxShader.setMat4Cube("projection", &projectionMatrix.m[0][0]);
    mSkyboxShader.setMat4Cube("view", &eyeViewMatrix.m[0][0]);
    mSkyboxShader.setMat4Cube("viewOffset", &eyeViewOffsetMatrix.m[0][0]);
    mSkyboxShader.setMat4Cube("viewCtlMatrix", &ctlMatrix.m[0][0]);

    mSkyboxModel.drawCube();

    mSkyboxShader.unUse();
    mSkyboxShader.unUseCube();

    //mark
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    //rect_render_->DrawMultiview(glm::mat4(), glm::mat4());

    switch (i) {
        case 0: {
            rect_render_->DrawStereo(lark::Object::EYE_LEFT, glm::mat4(), glm::mat4());
            break;
        }
        case 1: {
            rect_render_->DrawStereo(lark::Object::EYE_RIGHT, glm::mat4(), glm::mat4());
            break;
        }
    }

    mTrackingStateS[i].devicePair.hmdPose.isConnected = true;

    mTrackingStateS[i].devicePair.hmdPose.position.x = layerView.pose.position.x;
    mTrackingStateS[i].devicePair.hmdPose.position.y = layerView.pose.position.y;
    mTrackingStateS[i].devicePair.hmdPose.position.z = layerView.pose.position.z;

    mTrackingStateS[i].devicePair.hmdPose.rotation.x = layerView.pose.orientation.x;
    mTrackingStateS[i].devicePair.hmdPose.rotation.y = layerView.pose.orientation.y;
    mTrackingStateS[i].devicePair.hmdPose.rotation.z = layerView.pose.orientation.z;
    mTrackingStateS[i].devicePair.hmdPose.rotation.w = layerView.pose.orientation.w;
}

void XrDemo::setSurface(JNIEnv *jni, jobject surface) {
    if (surface != nullptr) {
        mNativeWindow = ANativeWindow_fromSurface(jni, surface);
        //LOGI("setSurface %p", mNativeWindow);
    }
}

bool XrDemo::initEGLSurface() {
    const EGLint attribs[] = {EGL_NONE};
    EGLSurface windowSurface = eglCreateWindowSurface(mDisplay, mConfig, mNativeWindow, attribs);

    if (eglMakeCurrent(mDisplay, windowSurface, windowSurface, mContext) == EGL_FALSE) {
        //LOGI("eglMakeCurrent failed: %s", getEglErrorChars());
        return false;
    }
    return true;
}

void XrDemo::initSkyboxMethod(JNIEnv *jni) {
    mSkyboxTexture.initJavaMethod(jni, mJvm, mActivity);
}

void XrDemo::start() {
    //LOGI("Creating render thread");
    pthread_create(&_threadId, nullptr, threadStartCallback, this);
}

void XrDemo::stop() {
    pthread_join(_threadId, nullptr);
}


void *XrDemo::threadStartCallback(void *myself) {
    XrDemo *demo = (XrDemo *) myself;
    demo->Run();

    pthread_exit(nullptr);
    return nullptr;
}


void XrDemo::DestroyActions() {
    //LOGI("DestroyActions");
    for (int i = 0; i < 13; i++) {
        XrResult retxrDestroyAct = xrDestroyAction(m_XrAction[i]);
        // //LOGI("retxrDestroyAct:%d",retxrDestroyAct);
    }
    XrResult retxrDestroyActSet = xrDestroyActionSet(m_actionSet);
    ////LOGI("retxrDestroyActSet:%d",retxrDestroyActSet);
}

double XrDemo::GetSeconds() {
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    const int64_t result =
            (int64_t) tp.tv_sec * (int64_t) (1000 * 1000 * 1000) + int64_t(tp.tv_nsec);

    return result * 0.000000001;
}

double XrDemo::GetNanos() {
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    const int64_t result =
            (int64_t) tp.tv_sec * (int64_t) (1000 * 1000 * 1000) + int64_t(tp.tv_nsec);

    return result;
}

//-----------lark_xr------------//

void XrDemo::OnResume() {
    LOGV("onResume()");
    LOGV("    APP_CMD_RESUME");
    if (xr_client_) {
        xr_client_->OnResume();
    }
}

void XrDemo::OnPause() {
    LOGV("onPause()");
    LOGV("    APP_CMD_PAUSE");
    if (xr_client_) {
        xr_client_->OnPause();
    }
}

void XrDemo::OnInitWindow(ANativeWindow *window) {
    LOGV("surfaceCreated()");
    LOGV("    APP_CMD_INIT_WINDOW");
    native_window_ = window;
    if (xr_client_) {
        xr_client_->OnCreated();
    }
}

void XrDemo::OnDestory() {
    LOGV("onDestroy()");
    LOGV("    APP_CMD_DESTROY");
    native_window_ = nullptr;
    if (xr_client_) {
        xr_client_->OnDestory();
    }
}

bool XrDemo::InitVR(android_app *app) {
    LOGENTRY();
    mJvm = app->activity->vm;
    Env = app->activity->env;
    // TODO
    // WARING jin verions dif AttachCurrentThread param different.
    mJvm->AttachCurrentThread(&Env, nullptr);
    mActivity = app->activity->clazz;

    LOGE("初始化环境");
    LOGE("InitVR");
    // 初始化环境。
    Context::Init(app->activity);

    return true;
}

void XrDemo::InitJava() {

}

bool XrDemo::InitGL() {
    return false;
}

void XrDemo::ShutdownVR() {
    LOGENTRY();
    // 清理环境
    Context::Reset();

    if (mJvm) {
        mJvm->DetachCurrentThread();
    }
}

void XrDemo::ShutdownGL() {
    LOGENTRY();
    // release cloudlark
    if (xr_client_) {
        xr_client_->UnRegisterObserver();
        xr_client_->Release();
    }

    // reset all state.
    Input::ResetInput();
    Navigation::ClearToast();
}

void XrDemo::HandleVrModeChange() {

}

bool XrDemo::OnUpdate() {
    return false;
}

void XrDemo::EnterAppli(const string &appId) {
    LOGENTRY();
    if (xr_client_) {
        xr_client_->EnterAppli(appId);
        //xr_client_->EnterAppli(appId);
    }
}

void XrDemo::CloseAppli() {
    LOGENTRY();
    if (xr_client_) {
        xr_client_->Close();
    }
}

void XrDemo::OnMediaReady(int nativeTextrure) {
    LOGENTRY();
    LOGE("OnMediaReady+Na");
    nativeTextrureFromMedia = nativeTextrure;
    rect_render_->SetMutiviewModeTexture(nativeTextrure);
}

void XrDemo::OnMediaReady(int nativeTextureLeft, int nativeTextureRight) {
    LOGENTRY();
    LOGE("OnMediaReady+L+R");
    nativeTextrureFromMediaLeft = nativeTextureLeft;
    nativeTextrureFromMediaRight = nativeTextureRight;
    rect_render_->SetStereoTexture(nativeTextureLeft, nativeTextureRight);
}

void XrDemo::OnMediaReady() {
    LOGE("OnMediaReady");
}

void XrDemo::RequestTrackingInfo() {
    //LOGE("RequestTrackingInfo");
    larkxrTrackingDevicePairFrame frame;
    frame.devicePair.hmdPose.isConnected = true;
    frame.devicePair.hmdPose.position.x =
            (mTrackingStateS[0].devicePair.hmdPose.position.x +
             mTrackingStateS[1].devicePair.hmdPose.position.x) / 2;
    frame.devicePair.hmdPose.position.y =
            (mTrackingStateS[0].devicePair.hmdPose.position.y +
             mTrackingStateS[1].devicePair.hmdPose.position.y) / 2 + 1.5;
    frame.devicePair.hmdPose.position.z =
            (mTrackingStateS[0].devicePair.hmdPose.position.z +
             mTrackingStateS[1].devicePair.hmdPose.position.z) / 2;

    frame.devicePair.hmdPose.rotation.x =
            (mTrackingStateS[0].devicePair.hmdPose.rotation.x +
             mTrackingStateS[1].devicePair.hmdPose.rotation.x) / 2;
    frame.devicePair.hmdPose.rotation.y =
            (mTrackingStateS[0].devicePair.hmdPose.rotation.y +
             mTrackingStateS[1].devicePair.hmdPose.rotation.y) / 2;
    frame.devicePair.hmdPose.rotation.z =
            (mTrackingStateS[0].devicePair.hmdPose.rotation.z +
             mTrackingStateS[1].devicePair.hmdPose.rotation.z) / 2;
    frame.devicePair.hmdPose.rotation.w =
            (mTrackingStateS[0].devicePair.hmdPose.rotation.w +
             mTrackingStateS[1].devicePair.hmdPose.rotation.w) / 2;

    frame.devicePair.controllerState[0].pose.isConnected = true;
    frame.devicePair.controllerState[0].pose.is6Dof = false;
    frame.devicePair.controllerState[0].pose.isValidPose = true;
    frame.devicePair.controllerState[0].inputState.isConnected = true;
    frame.devicePair.controllerState[0].pose.status = 0;

    frame.devicePair.controllerState[1].pose.isConnected = true;
    frame.devicePair.controllerState[1].pose.is6Dof = false;
    frame.devicePair.controllerState[1].pose.isValidPose = true;
    frame.devicePair.controllerState[1].inputState.isConnected = true;
    frame.devicePair.controllerState[1].pose.status = 0;

    frame.devicePair.controllerState[0].pose.position.x = 0.2;
    frame.devicePair.controllerState[0].pose.position.y = 1;
    frame.devicePair.controllerState[0].pose.position.z = -0.4;

    frame.devicePair.controllerState[0].pose.rotation.x =
            mTrackingStateS[0].devicePair.controllerState[0].pose.rotation.x;
    frame.devicePair.controllerState[0].pose.rotation.y =
            mTrackingStateS[0].devicePair.controllerState[0].pose.rotation.y;
    frame.devicePair.controllerState[0].pose.rotation.z =
            mTrackingStateS[0].devicePair.controllerState[0].pose.rotation.z;
    frame.devicePair.controllerState[0].pose.rotation.w =
            mTrackingStateS[0].devicePair.controllerState[0].pose.rotation.w;
    LOGE("framdevicePair%f-%f-%f-%f",
         frame.devicePair.controllerState[0].pose.rotation.x,
         frame.devicePair.controllerState[0].pose.rotation.y,
         frame.devicePair.controllerState[0].pose.rotation.z,
         frame.devicePair.controllerState[0].pose.rotation.w);
/*    frame.devicePair.controllerState[0].pose.rotation.x =
            (mTrackingStateS[0].devicePair.controllerState[0].pose.rotation.x +
             mTrackingStateS[1].devicePair.controllerState[0].pose.rotation.x) / 2;
    frame.devicePair.controllerState[0].pose.rotation.y =
            (mTrackingStateS[0].devicePair.controllerState[0].pose.rotation.y +
             mTrackingStateS[1].devicePair.controllerState[0].pose.rotation.y) / 2;
         mTrackingStateS[1].devicePair.controllerState[0].pose.rotation.y);
    frame.devicePair.controllerState[0].pose.rotation.z =
            (mTrackingStateS[0].devicePair.controllerState[0].pose.rotation.z +
             mTrackingStateS[1].devicePair.controllerState[0].pose.rotation.z) / 2;
    frame.devicePair.controllerState[0].pose.rotation.w =
            (mTrackingStateS[0].devicePair.controllerState[0].pose.rotation.w +
             mTrackingStateS[1].devicePair.controllerState[0].pose.rotation.w) / 2;*/
    xr_client_->SendDevicePair(frame);
}

void XrDemo::OnTrackingFrame(const larkxrTrackingFrame &trackingFrame) {
    //LOGE("OnTrackingFrame");
    Application::OnTrackingFrame(trackingFrame);
}

