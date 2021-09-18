//
// Created by Admin on 2021/7/1.
//

#ifndef NOLOLARKXR_LARKXR_CLIENT_H
#define NOLOLARKXR_LARKXR_CLIENT_H
#include <lark_xr/xr_client.h>
#include <nibiru/NibiruVRApi.h>
#include "lark_xr/app_list_task.h"
#include "log.h"
#include "render/texture_rect.h"
#include "lark_xr/xr_latency_collector.h"
#include "scene_local.h"
#include "application.h"
#include "rect_texture.h"
#include "INxrSensorClient/INxrSensorClient.h"

#define USE_RENDER_QUEUE = 1;

using namespace lark;

class LarkxrClient:public lark::AppListTask::AppListTaskListener, public Application {
public:
    LarkxrClient();
    ~LarkxrClient();

    virtual bool InitVR(android_app* app) override { return false; };
    virtual bool InitVR() override { return false; };
    virtual void InitJava() override {};
    virtual void ShutdownVR() override {};

    virtual void HandleVrModeChange() override {};
    virtual bool OnUpdate() override { return false; };

    void InitJava(JavaVM* vm);
    virtual bool InitGL() override;
    void OnCreate();
    void OnPause();
    void OnResume();
    virtual void ShutdownGL() override;
    void OnDestory();

    void Connect();
    void Close();
    void EnterAppli(const std::string& appliId) override;
    virtual void CloseAppli() override;

    void Update(const glm::quat& rotation, const glm::vec3& position);
    void Draw(const nvr::nvr_Eye& eye);
    void Finish();

    // xr client callback
    virtual void OnSDKAuthorizationFailed(int code, const std::string& msg) override;
    virtual void OnConnected() override ;
    virtual void OnClose(int code) override;
    virtual void OnStreamingDisconnect() override ;
    virtual void OnInfo(int infoCode, const std::string& msg) override;
    virtual void OnError(int errCode, const std::string& msg) override ;
    // soft decoder
    virtual void OnMediaReady() override  {};
    // hw decoder callback textrue.
    virtual void OnMediaReady(int nativeTextrure) override;
    //双眼纹理
    virtual void OnMediaReady(int nativeTextrureLeft, int nativeTextureRight) override;

    virtual void OnTrackingFrame(std::unique_ptr<lark::XRTrackingFrame>&& frame) override {};
    // hw decoder callback without textrue.
    virtual void OnTrackingFrame(const larkxrTrackingFrame& trackingFrame) override;
    virtual void RequestTrackingInfo() override ;
    virtual void OnClientId(const std::string& clientId) override ;
    virtual void OnHapticsFeedback(bool isLeft, uint64_t startTime, float amplitude, float duration, float frequency) override;
    virtual void OnSyncPlayerSpace(larkxrPlaySpace* playSpace) override {};

    // applist callback
    virtual void OnAppListInfo(const std::vector<lark::AppliInfo> & appliInfo) override;
    virtual void OnAppListPageInfo(const lark::AppliPageInfo& appliPageInfo) override;
    virtual void OnFailed(const std::string& msg) override;
    virtual void OnRunMode(lark::GetVrClientRunMode::ClientRunMode runMode) override;

    void setAssets(JNIEnv *env, jobject &assets);

    void setNibiruVRApi(nvr::NibiruVRApi* vrApi);
    //
    inline void set_nxr_sensor_client(android::INxrSensorClient *nxr_sensor_client) { nxr_sensor_client_ = nxr_sensor_client; }

    int testTexutreId = 0;
    larkxrTrackingFrame cloud_tracking_ = {};

    bool media_ready(){ return xr_client_ &&xr_client_->media_ready(); }
    inline bool has_new_frame() { return has_new_frame_; }
private:
    void UpdateHmd(const glm::quat& rotation, const glm::vec3& position);
    void HandlInput();

    nvr::NibiruVRApi * nibiruVrApi = NULL;
    TextureRect textureRect;
    // appli info task
    std::shared_ptr<lark::AppListTask> app_list_task_;

    bool has_new_frame_ = false;
    int textureId = 0;
    int nativeTextrureLeft = 0;
    int nativeTextrureRight = 0;
    bool isStartedApp = false;
    bool inited = false;
    bool connected = false;

    std::shared_ptr<SceneLocal> scene_local_ = nullptr;
    std::shared_ptr<RectTexture> rect_render_ = nullptr;

    bool back_button_down_last_frame_[Input::RayCast_Count]{};
    bool trigger_button_down_last_frame_[Input::RayCast_Count]{};
    bool enter_button_down_last_frame_[Input::RayCast_Count]{};

    larkxrDevicePair device_pair_ = {};
    android::INxrSensorClient *nxr_sensor_client_ = nullptr;
};


#endif //NOLOLARKXR_LARKXR_CLIENT_H
