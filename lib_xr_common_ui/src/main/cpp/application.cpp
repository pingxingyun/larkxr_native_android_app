//
// Created by fcx on 2020/5/6.
//

#include <ui/home/home.h>
#include <ui/navigation.h>
#include "application.h"
#include "env_context.h"
#include "log.h"
#include "utils.h"

void Application::RegiseredInstance(Application *instance) {
    s_instance_ = instance;
}

void Application::UnRegiseredInstance() {
    s_instance_ = nullptr;
}

Application* Application::instance() {
    return s_instance_;
}

Application* Application::s_instance_ = nullptr;

Application::Application() = default;

Application::~Application() = default;

void Application::EnterAppli(const std::string &appId) {
    LOGE("on enter appli","enter");
    if (xr_client_) {
        xr_client_->EnterAppli(appId);
    }
    LOGE("on enter appli","enterover");
}

void Application::CloseAppli() {
}

void Application::SetServerAddr(const std::string &ip, uint16_t port) {
    if (xr_client_) {
        xr_client_->OnPause();
        xr_client_->SetServerAddr(ip, port);
        xr_client_->OnResume();
    }
}

void Application::OnClientId(const std::string& clientId) {
    Home::SetClientId(clientId);
}

void Application::OnInfo(int infoCode, const std::string &msg) {
    Navigation::ShowToast(msg);
}

void Application::InitCertificate() {
    if (!Context::instance()) {
        LOGW("Init certificate failed context not created.");
        return;
    }
    std::string internalPath = Context::instance()->internal_data_path();
    std::string externalPath = Context::instance()->external_data_path();

    std::string path = !externalPath.empty() ? externalPath : internalPath;

    if (path.empty()) {
        LOGW("init certificate failed. data path empty.");
        return;
    }
    std::string appKeyPath = path + "/larkxr/certificate_appkey.txt";
    std::string appSecretPath = path + "/larkxr/certificate_appsecret.txt";

    char appkey[1024] = {};
    int appKeyLen = utils::ReadFile(appKeyPath.c_str(), appkey, 1024);

    if (appKeyLen <= 0) {
        LOGW("read appkey certificate failed. %s code %d", appKeyPath.c_str(), appKeyLen);
        return;
    }

    char appSecret[1024] = {};
    int appSecretLen = utils::ReadFile(appSecretPath.c_str(), appSecret, 1024);

    LOGV("appkey %s appSecret %s", appkey, appSecret);
    lark::XRClient::SetCertificate(std::string(appkey, appKeyLen), std::string(appSecret, appSecretLen));
}
