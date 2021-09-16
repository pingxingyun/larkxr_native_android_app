//
// Created by fcx@pingxingyun.com on 2019/10/23.
//

#ifndef MY_APPLICATION_APPLICATION_H
#define MY_APPLICATION_APPLICATION_H

#include <android_native_app_glue.h>
#include "ovr_egl.h"
#include "utils.h"
#include "ovr_scene_local.h"
#include "ovr_scene_cloud.h"
#include "lark_xr/xr_client.h"
#include "application.h"

class OvrApplication: public Application {
public:
    static const int CPU_LEVEL			= 2;
    static const int GPU_LEVEL			= 3;
    static const int NUM_MULTI_SAMPLES	= 4;

    OvrApplication();
    ~OvrApplication();

    // andoird lifecycle
    void OnResume();
    void OnPause();
    void OnInitWindow(ANativeWindow * window);
    void OnDestory();

    virtual bool InitVR(android_app* app) override;
    virtual bool InitVR() override { return false; };
    virtual void InitJava() override;
    virtual bool InitGL() override;
    virtual void ShutdownVR() override;
    virtual void ShutdownGL() override;

    //  callback from android native activity.
    virtual void HandleVrModeChange() override;
    virtual bool OnUpdate() override;
    // 进入应用
    virtual void EnterAppli(const std::string& appId) override;
    virtual void CloseAppli() override;

    inline ovrMobile* ovr() { return ovr_; }

    // xr client callback
    virtual void OnConnected() override;
    virtual void OnClose(int code) override;
    virtual void OnError(int errCode, const std::string& msg) override;
    virtual void OnHapticsFeedback(bool isLeft, uint64_t startTime, float amplitude, float duration, float frequency) override;
    // hw decoder callback textrue.
    virtual void OnMediaReady() override;
    virtual void OnMediaReady(int nativeTexture) override;
    virtual void OnMediaReady(int nativeTextureLeft, int nativeTextureRight) override;
    virtual void RequestTrackingInfo() override;
    virtual void OnSyncPlayerSpace(larkxrPlaySpace* playSpace) override;

    // handle network change
    virtual void OnNetworkAvailable() override;
    virtual void OnNetworkLost() override;
private:
    void CreateFrameBuffer(const ovrJava *java, const bool useMultiview);
    void DestoryFrameBuffer();
    void ClearFrameBuffer();

    ovrJava             java_{};
    std::shared_ptr<OvrEgl> ovr_egl_;

    ANativeWindow *		native_window_ = nullptr;
    bool                resumed_ = false;
    ovrMobile *			ovr_ = nullptr;

    int					cpu_level = 0;
    int					gpu_level_ = 0;
    int					main_thread_tid_ = 0;
    int					render_thread_tid_ = 0;

    bool				use_multiview_ = false;

    ovrTrackingSpace    tracking_space_;
    std::vector<ovrVector3f> guardian_points = {};
    // local ui
    std::shared_ptr<OvrSceneLocal> scene_local_ = {};

    // cloud render
    std::shared_ptr<OvrSceneCloud> scene_cloud_ = {};
    // frame buffer
    OvrFrameBuffer frame_buffer_[VRAPI_FRAME_LAYER_EYE_MAX];
    int			   num_buffers_;
};


#endif //MY_APPLICATION_APPLICATION_H
