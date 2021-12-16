//
// Created by Hayasi-Yumito on 2021/12/14.
//

#ifndef LARKXR_AR_DEMO_H
#define LARKXR_AR_DEMO_H


#include <application.h>
#include <rect_texture.h>

#include "huawei_arengine_interface.h"
#include "rendering/world_background_renderer.h"
#include "rendering/world_object_renderer.h"
#include "rendering/world_plane_renderer.h"
#include "rendering/world_point_cloud_renderer.h"
#include "rendering/world_render_manager.h"

class Ar_Demo : public Application {

public:
    //Jvm & Activity
    jobject mActivity;
    JavaVM* mJvm;
    JNIEnv*	Env;
    Ar_Demo(JavaVM* _vm, jobject act,JNIEnv* _Env);
    ~Ar_Demo();
    std::shared_ptr<RectTexture> rect_render_ = nullptr;
    int nativeTextrureFromMedia;
    //Native window surface
    ANativeWindow* mNativeWindow{};

    void DrawRect();
    //Set Native window surface
    void setSurface(JNIEnv* jni, jobject surface);

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


    // hw decoder callback textrue.
    virtual void OnMediaReady() override;
    virtual void OnMediaReady(int nativeTexture) override;
    virtual void OnMediaReady(int nativeTextureLeft, int nativeTextureRight) override;
    virtual void RequestTrackingInfo() override;
    virtual void OnTrackingFrame(const larkxrTrackingFrame& trackingFrame) override;

    void Init();
    void InitXr();

    bool initEGLSurface();

    void OnResume(void* env, void* context, void* activity);
    void OnDisplayGeometryChanged(int displayRotation, int width, int height);
    void OnDrawFrame();

    jboolean IsRunning();

    void OnPause();

    bool IsArEngineApkInstalled(JNIEnv *pEnv, jobject pJobject);

    void OnSurfaceCreated();

private:
    HwArSession *mArSession = nullptr;
    HwArFrame *mArFrame = nullptr;

    int mDisplayRotation = 0;
    int mWidth = 1;
    int mHeight = 1;

    std::vector<gWorldAr::ColoredAnchor> mColoredAnchors = {};
    gWorldAr::WorldRenderManager mWorldRenderManager = gWorldAr::WorldRenderManager();
};


#endif //LARKXR_AR_DEMO_H
