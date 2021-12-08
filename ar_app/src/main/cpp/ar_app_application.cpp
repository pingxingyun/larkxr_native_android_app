//
// Created by Hayasi-Yumito on 2021/12/7.
//

#include <logger.h>
#include <utils.h>
#include <ui/navigation.h>
#include <lark_xr/xr_config.h>
#include <EGL/egl.h>
#include "ar_app_application.h"

ar_app_application *arapp = nullptr;
jstring serverip;

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_larkar_1native_1android_1app_JniInterface_creatNativeApplication(JNIEnv *env,
                                                                              jclass clazz,
                                                                              jobject act) {
// TODO: implement creatNativeApplication()
    LOGI("init vr");
    JavaVM *_vm = nullptr;
//jint ret = jni->GetJavaVM(&_vm);
    env->GetJavaVM(&_vm);
//LOGI("Create global ref");
    jobject g_act = env->NewGlobalRef(act);
    LOGI("init application");
    arapp = new ar_app_application(_vm, g_act, env);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_larkar_1native_1android_1app_JniInterface_enterapp(JNIEnv *env, jclass clazz,
                                                                jstring appid) {
    // TODO: implement enterapp()
    if (arapp != nullptr) {
        const char *charappid = env->GetStringUTFChars(appid, 0);
        arapp->EnterAppli(charappid);

        arapp->Set2DUIEnterAppliMode(charappid);
        env->ReleaseStringUTFChars(appid, charappid);
    }
}

ar_app_application::ar_app_application(JavaVM *_vm, jobject act, JNIEnv *_jEnv) {
    LOGI("ar_app_application");
    mActivity = act;
    mJvm = _vm;
    Env = _jEnv;
    LOGI("init xr_client_");
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
    xr_client_->SetServerAddr("222.128.6.137", 8585);
    xr_client_->EnterAppli("756846918545440768");
    rect_render_ = std::make_shared<RectTexture>();
    lark::XRConfig::use_multiview = false;
    lark::XRConfig::fps = 72;
    lark::XRConfig::render_width = 1920;
    lark::XRConfig::render_height = 1080;
}

ar_app_application::~ar_app_application() = default;

bool ar_app_application::InitVR(android_app *app) {
    return false;
}

void ar_app_application::InitJava() {

}

bool ar_app_application::InitGL() {
    return false;
}

void ar_app_application::ShutdownVR() {

}

void ar_app_application::ShutdownGL() {

}

void ar_app_application::HandleVrModeChange() {

}

bool ar_app_application::OnUpdate() {
    return false;
}

void ar_app_application::EnterAppli(const std::string &appId) {

}

void ar_app_application::CloseAppli() {

}

void ar_app_application::OnMediaReady(int nativeTexture) {
    Application::OnMediaReady(nativeTexture);
    LOGE("OnMediaReady+Na");
    nativeTextrureFromMedia = nativeTexture;
    rect_render_->SetMutiviewModeTexture(nativeTexture);
}

void ar_app_application::OnMediaReady(int nativeTextureLeft, int nativeTextureRight) {
    Application::OnMediaReady(nativeTextureLeft, nativeTextureRight);
    LOGE("OnMediaReady+L+R");
    nativeTextrureFromMediaLeft = nativeTextureLeft;
    nativeTextrureFromMediaRight = nativeTextureRight;
    rect_render_->SetStereoTexture(nativeTextureLeft, nativeTextureRight);
}

void ar_app_application::OnMediaReady() {
    Application::OnMediaReady();
}

void ar_app_application::RequestTrackingInfo() {
    Application::RequestTrackingInfo();
    larkxrTrackingDevicePairFrame frame;
    frame.devicePair.hmdPose.isConnected = true;
}

void ar_app_application::OnTrackingFrame(const larkxrTrackingFrame &trackingFrame) {
    Application::OnTrackingFrame(trackingFrame);
}

std::string jstring2str(JNIEnv *env, jstring jstr) {
    char *rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("GB2312");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte *ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0) {
        rtn = (char *) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    std::string stemp(rtn);
    free(rtn);
    return stemp;
}


