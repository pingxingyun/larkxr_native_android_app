//
// Created by Hayasi-Yumito on 2021/12/14.
//

#include <logger.h>
#include <utils.h>
#include <ui/navigation.h>
#include <rect_texture.h>
#include <EGL/egl.h>
#include <android/native_window_jni.h>
#include "Ar_Demo.h"


Ar_Demo *gApp= nullptr;

// use for any deeper, failure-possible init of app, or cxr client.
void Ar_Demo::Init() {
}

void Ar_Demo::InitXr(){
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
/*    xr_client_->SetServerAddr("222.128.6.137", 8585);
    xr_client_->EnterAppli("846813152229195776");*/
    xr_client_->SetServerAddr("192.168.31.15", 8181);
    xr_client_->EnterAppli("756846918545440768");
    rect_render_ = std::make_shared<RectTexture>();
    //lark::XRConfig::use_multiview = false;
/*    lark::XRConfig::fps = 72;
    lark::XRConfig::render_width = 1920;
    lark::XRConfig::render_height = 1080;*/
}

Ar_Demo::Ar_Demo(JavaVM *_vm, jobject act, JNIEnv *_Env) :
        mNativeWindow(nullptr) {
    mActivity = act;
    mJvm = _vm;
    Env = _Env;
}

Ar_Demo::~Ar_Demo() = default;

void Ar_Demo::setSurface(JNIEnv *jni, jobject surface) {
    if (surface != nullptr) {
        mNativeWindow = ANativeWindow_fromSurface(jni, surface);
        //LOGI("setSurface %p", mNativeWindow);
    }
}

bool Ar_Demo::InitVR(android_app *app) {
    return false;
}

void Ar_Demo::InitJava() {

}

bool Ar_Demo::InitGL() {
    return false;
}

void Ar_Demo::ShutdownVR() {

}

void Ar_Demo::ShutdownGL() {

}

void Ar_Demo::HandleVrModeChange() {

}

bool Ar_Demo::OnUpdate() {
    return false;
}

void Ar_Demo::EnterAppli(const std::string &appId) {

}

void Ar_Demo::CloseAppli() {

}

void Ar_Demo::OnMediaReady() {
    Application::OnMediaReady();
    LOGE("OnMediaReady");
}

void Ar_Demo::OnMediaReady(int nativeTexture) {
    Application::OnMediaReady(nativeTexture);
    LOGE("OnMediaReady+Na");
    nativeTextrureFromMedia = nativeTexture;
    rect_render_->SetMutiviewModeTexture(nativeTexture);
}

void Ar_Demo::OnMediaReady(int nativeTextureLeft, int nativeTextureRight) {
    Application::OnMediaReady(nativeTextureLeft, nativeTextureRight);
    LOGE("OnMediaReady+LR");
}

void Ar_Demo::RequestTrackingInfo() {
    Application::RequestTrackingInfo();
    larkxrTrackingDevicePairFrame frame;
    frame.devicePair.hmdPose.isConnected = true;

    xr_client_->SendDevicePair(frame);
}

void Ar_Demo::OnTrackingFrame(const larkxrTrackingFrame &trackingFrame) {
    Application::OnTrackingFrame(trackingFrame);
}

jboolean Ar_Demo::IsRunning() {
    return 0;
}

void Ar_Demo::OnResume(void* env, void* context, void* activity) {
    LOGI("WorldArApplication::OnResume()");
    if (mArSession == nullptr) {
        LOGI("WorldArApplication::0()");
        CHECK(HwArSession_create(env, context, &mArSession) == HWAR_SUCCESS);
        CHECK(mArSession);
        LOGI("WorldArApplication::1()");

        HwArConfig *arConfig = nullptr;
        HwArConfig_create(mArSession, &arConfig);
        LOGI("WorldArApplication::2()");

        CHECK(HwArSession_configure(mArSession, arConfig) == HWAR_SUCCESS);
        LOGI("WorldArApplication::3()");

        HwArConfig_destroy(arConfig);
        HwArFrame_create(mArSession, &mArFrame);
        HwArSession_setDisplayGeometry(mArSession, mDisplayRotation, mWidth, mHeight);
    }

    const HwArStatus status = HwArSession_resume(mArSession);
    CHECK(status == HWAR_SUCCESS);
}

void Ar_Demo::OnDisplayGeometryChanged(int displayRotation, int width, int height) {
    LOGI("WorldArApplication::OnDisplayGeometryChanged(%d, %d)", width, height);
    glViewport(0, 0, width, height);
    mDisplayRotation = displayRotation;
    mWidth = width;
    mHeight = height;
    if (mArSession != nullptr) {
        HwArSession_setDisplayGeometry(mArSession, displayRotation, width, height);
    }
}

void Ar_Demo::OnDrawFrame() {
    LOGI("WorldArApplication::OnDrawFrame()");
    mWorldRenderManager.OnDrawFrame(mArSession, mArFrame, mColoredAnchors, rect_render_);
//    DrawRect();
}



void Ar_Demo::OnPause() {

}

bool Ar_Demo::IsArEngineApkInstalled(JNIEnv *pEnv, jobject pJobject) {
    return HwArEnginesApk_isAREngineApkReady(pEnv, pJobject);
}

void Ar_Demo::OnSurfaceCreated() {
    LOGI("WorldArApplication::OnSurfaceCreated()");
    mWorldRenderManager.Initialize();
    InitXr();
}

void Ar_Demo::DrawRect() {
    rect_render_->DrawMultiview(glm::mat4(), glm::mat4());
}



