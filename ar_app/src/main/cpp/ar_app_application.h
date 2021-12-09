//
// Created by Hayasi-Yumito on 2021/12/7.
//

#include <rect_texture.h>
#include <application.h>
#include <plane_renderer.h>

#ifndef LARKXR_AR_APP_APPLICATION_H
#define LARKXR_AR_APP_APPLICATION_H

#endif //LARKXR_AR_APP_APPLICATION_H

class ar_app_application : public Application {
public:
    ar_app_application(JavaVM *_vm, jobject act, JNIEnv *_Env,AAssetManager* asset_manager);
    AAssetManager* const asset_manager_;
    PlaneRenderer plane_renderer_;
    jobject mActivity;
    JavaVM* mJvm;
    JNIEnv*	Env;

    //Egl Environment
    EGLConfig mConfig;
    EGLContext mContext;
    EGLDisplay mDisplay;

    //Native window surface
    ANativeWindow* mNativeWindow;

    int nativeTextrureFromMedia{};
    int nativeTextrureFromMediaLeft{};
    int nativeTextrureFromMediaRight{};
    std::shared_ptr<RectTexture> rect_render_ = nullptr;

    void InitBackgroundGL();

    virtual bool InitVR(android_app *app) override;

    virtual bool InitVR() override { return false; };

    virtual void InitJava() override;

    virtual bool InitGL() override;

    virtual void ShutdownVR() override;

    virtual void ShutdownGL() override;

    //  callback from android native activity.
    virtual void HandleVrModeChange() override;

    virtual bool OnUpdate() override;

    // 进入应用
    virtual void EnterAppli(const std::string &appId) override;

    virtual void CloseAppli() override;

    ~ar_app_application() override;

    ANativeWindow *native_window_ = nullptr;

    // hw decoder callback textrue.
    virtual void OnMediaReady() override;
    virtual void OnMediaReady(int nativeTexture) override;
    virtual void OnMediaReady(int nativeTextureLeft, int nativeTextureRight) override;
    virtual void RequestTrackingInfo() override;
    virtual void OnTrackingFrame(const larkxrTrackingFrame& trackingFrame) override;
};