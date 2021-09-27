//
// Created by fcx on 2020/6/30.
//

#ifndef CLOUDLARKXR_PVR_APPLICATION_H
#define CLOUDLARKXR_PVR_APPLICATION_H

#include <glm/detail/type_quat.hpp>
#include "glm/glm.hpp"
#include <application.h>
#include "pvr_scene_local.h"
#include "pvr_scene_cloud.h"
#include "pvr_utils.h"

class PvrApplication: public Application {
public:
    // init application.
    static void InitInstance();
    static void ReleaseInstance();

    virtual bool InitVR(android_app* app) override { return false; };
    virtual bool InitVR() override { return false; };
    virtual bool InitGL() override { return false; };
    virtual bool InitGL(int eyeWidth, int eyeHeight);
    virtual void InitJava() override {};
    void InitJava(JavaVM* vm, jobject pvrSdk, jobject mainactivity_object_, int type);
    virtual void ShutdownVR() override {};
    virtual void ShutdownGL() override;
    // 对应 pico sdk 中的 deInitGL
    void deInitGL();

    //  callback from android native activity.
    virtual void HandleVrModeChange() override {};
    virtual bool OnUpdate() override { return false; };
    // 进入应用
    virtual void EnterAppli(const std::string& appId) override;
    virtual void CloseAppli() override;

    void FrameBegin(const pvr::PvrPose& pose);
    void UpdateControler(const pvr::PvrControllerState& controllerState);
    void Draw(int eye);
    void FrameEnd();
    bool GetRenderPose(pvr::PvrPose& pose);

    void OnResume();
    void OnPause();
    void OnDestory();

    inline pvr::PvrControllerState GetControllerState(pvr::PvrControllerType type) { return controller_[type]; }
    inline pvr::PvrPose hmd_pose() { return hmd_pose_; }
    // xr client callback
    virtual void OnConnected() override;
    virtual void OnClose(int code) override;
    virtual void OnError(int errCode, const std::string& msg) override;
    virtual void OnHapticsFeedback(bool isLeft, uint64_t startTime, float amplitude, float duration, float frequency) override;
    // hw decoder callback textrue.
    virtual void OnMediaReady(int nativeTextrure) override;
    virtual void OnMediaReady(int nativeTextureLeft, int nativeTextureRight) override;
    virtual void OnMediaReady() override;
    virtual void RequestTrackingInfo() override;
    virtual void OnSyncPlayerSpace(larkxrPlaySpace* playSpace) override;
    virtual void Quit3DUI() override;
private:
    static PvrApplication* s_pvr_instance_;

    PvrApplication();
    ~PvrApplication();

    void ResetTracking(pvr::PvrTrackingOrigin origin);

    std::shared_ptr<PvrSceneLocal> scene_local_ = {nullptr};
    std::shared_ptr<PvrSceneCloud> scene_cloud_ = {nullptr};
    pvr::PvrPose hmd_pose_{};
    pvr::PvrControllerState controller_[2];
    larkxrTrackingFrame cloud_tracking_ = {};
    bool has_new_frame_ = false;

    jobject pvr_sdk_object_ = nullptr;
    jobject mainactivity_object_ = nullptr;
};


#endif //CLOUDLARKXR_PVR_APPLICATION_H
