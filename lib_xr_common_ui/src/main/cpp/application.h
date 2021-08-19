//
// Created by fcx on 2020/5/6.
//

#ifndef CLOUDLARKXR_APPLICATION_H
#define CLOUDLARKXR_APPLICATION_H

#include <android_native_app_glue.h>
#include "lark_xr/xr_client.h"

// 使用内部 ID 初始化
// 注意内部 ID 对外保密。
#define LARK_SDK_ID "2ad9f9a6aa454a11df274e900613510b"
#define LARK_SDK_SECRET "52b07a8437d39b6cda59cdf4f994f4aa59ce1aba90d9cf247b09b3568c75d4d7"

#ifdef LARK_SDK_SECRET
#include "lark_xr/pxy_inner_utils.h"
#endif

//
// application interface.
//
class Application: public lark::XRClientObserver {
public:
    // warning should init in child class
    static Application* instance();
    virtual ~Application();

    //
    virtual bool InitVR(android_app* app) = 0;
    virtual bool InitVR() = 0;
    virtual bool InitGL() = 0;
    virtual void InitJava() = 0;
    virtual void ShutdownVR() = 0;
    virtual void ShutdownGL() = 0;

    //  callback from android native activity.
    virtual void HandleVrModeChange() = 0;
    virtual bool OnUpdate() = 0;
    // 进入应用
    virtual void EnterAppli(const std::string& appId) = 0;
    virtual void CloseAppli() = 0;
    // xr client callback
    virtual void OnSDKAuthorizationFailed(int code, const std::string& msg) override {};
    virtual void OnConnected() override {};
    virtual void OnClose(int code) override {};
    virtual void OnStreamingDisconnect() override {};
    virtual void OnInfo(int infoCode, const std::string& msg) override;
    virtual void OnError(int errCode, const std::string& msg) override {};
    // soft decoder
    virtual void OnMediaReady() override  {};
    // hw decoder callback textrue.
    virtual void OnMediaReady(int nativeTextrure) override {};
    virtual void OnMediaReady(int nativeTextrureLeft, int nativeTextureRight) override {};
    virtual void OnTrackingFrame(std::unique_ptr<lark::XRTrackingFrame>&& frame) override {};
    // hw decoder callback without textrue.
    virtual void OnTrackingFrame(const larkxrTrackingFrame& trackingFrame) override {};
    virtual void RequestTrackingInfo() override {};
    virtual void OnHapticsFeedback(bool isLeft, uint64_t startTime, float amplitude, float duration, float frequency) override {};
    // request player space
    virtual void  OnSyncPlayerSpace(larkxrPlaySpace* playSpace) override {};

    virtual void OnClientId(const std::string& clientId) override;
    // setup server addr
    virtual void SetServerAddr(const std::string& ip, uint16_t port);
    // handle network change
    virtual void OnNetworkAvailable() {};
    virtual void OnNetworkLost() {};
protected:
    static void RegiseredInstance(Application* instance);
    static void UnRegiseredInstance();

    Application();
    virtual void HandleInput() {};
    // 初始化客户端接入凭证
    // android Context 创建成功之后调用
    // 从文件中读取 appkey 和 secret
    // 优先从 sd 卡读取
    // 【外部路径】or【内部路径】/data/data/APPID/larkxr/certificate_appkey.txt
    // 【外部路径】or【内部路径】/data/data/APPID/larkxr/certificate_appsecret.txt
    void InitCertificate();

    // xrclient
    std::shared_ptr<lark::XRClient> xr_client_ = nullptr;
    //
    bool connected_ = false;

private:
    // static instance
    // WARNING should init in child class
    static Application* s_instance_;
};


#endif //CLOUDLARKXR_APPLICATION_H
