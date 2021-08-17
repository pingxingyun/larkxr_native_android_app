//
// Created by fcx@pingixngyun.com on 2019/12/4.
//

#include <log.h>
#include <env_context.h>
#include <utils.h>
#include <ui/localization.h>
#include <application.h>
#include <ui/home/home.h>
#include <ui/navigation.h>
#include "setup_server_addr.h"
#include "lark_xr/xr_client.h"
#include "lark_xr/request/get_appli_list.h"
#include "lark_xr/request/vrclient_heartbeat.h"
#include "lark_xr/xr_config.h"
#include "lark_xr/request/get_core_server_version_info.h"

namespace {
    // 3.0 change to 8181
    const uint16_t DEFAULT_PORT            = 8181;
    const uint64_t INPUT_LINE_COLOR        = 0xcfd9ff7F;
    const uint64_t INPUT_LINE_COLOR_ACTIVE = 0xcfd9ffFF;
    const float INPUT_BOX_WIDTH            = 1.0F;
    const float INPUT_BOX_HEIGHT           = 0.12F;
}

using namespace lark;

SetupServerAddr::SetupServerAddr(Navigation *navigation):
    View(navigation),
    get_applist_adapter_(std::bind(&SetupServerAddr::GetAppList, this))
{
    SetupServerAddr::Init();
}

SetupServerAddr::~SetupServerAddr() = default;

void SetupServerAddr::Init() {
    title_ = std::make_shared<Text>(localization::Loader().getResource().ui_home_setup_serveraddr);
    title_->Move(-title_->GetSize().x / 2.0F, 1.5F - title_->GetSize().y / 2.0F, 0);
    AddChild(title_);

    keyboard_ = std::make_shared<Keyboard>(this);
    keyboard_->Move(-1.8F, -1.0F, 0);
    keyboard_->SetAABBPositon(glm::vec2(-1.8F, -1.0F));
    PushAABB(keyboard_.get());
    AddChild(keyboard_);
    keyboard_->set_active(false);

    {
        label_server_ip_ = std::make_shared<Text>(localization::Loader().getResource().ui_setup_serveraddr_ip);
        label_server_ip_->Move(-1.5F, 1, 0);
        label_server_ip_->SetFontSize(26);
        AddChild(label_server_ip_);

        server_ip_button_ = std::make_shared<InputButton>(L"");
        server_ip_button_->Move(-0.6F, 1, 0);
        server_ip_button_->SetAABBPositon(glm::vec2(-0.6F, 1 - INPUT_BOX_HEIGHT * 0.5));
        PushAABB(server_ip_button_.get());
        AddChild(server_ip_button_);
    }

    {
        label_server_port_ = std::make_shared<Text>(localization::Loader().getResource().ui_setup_serveraddr_port);
        label_server_port_->Move(-1.5F, 0.8, 0);
        label_server_port_->SetFontSize(26);
        AddChild(label_server_port_);

        server_port_button_ = std::make_shared<InputButton>(L"");
        server_port_button_->Move(-0.6F, 0.8, 0);
        server_port_button_->SetAABBPositon(glm::vec2(-0.6F, 0.8 - INPUT_BOX_HEIGHT * 0.5));
        PushAABB(server_port_button_.get());
        AddChild(server_port_button_);
    }

    {
        label_mode_ = std::make_shared<Text>(localization::Loader().getResource().ui_setup_serveraddr_currentmode);
        label_mode_->Move(-1.5F, 0.6, 0);
        label_mode_->SetFontSize(26);
        AddChild(label_mode_);

        mode_text_ = std::make_shared<Text>(localization::Loader::getResource().ui_setup_serveraddr_hand_mode);
        mode_text_->Move(-0.6F, 0.6F, 0);
        mode_text_->SetFontSize(26);
        AddChild(mode_text_);
    }

    {
        submit_button_ = std::make_shared<TextButton>(localization::Loader().getResource().ui_setup_serveraddr_submit);
        submit_button_->SetFontSize(24);
        submit_button_->Move(-0.6F, 0.4, 0);
        submit_button_->SetAABBPositon(glm::vec2(-0.6F, 0.4 - submit_button_->GetSize().y / 2));
        submit_button_->SetAABBSize(submit_button_->GetSize());
        PushAABB(submit_button_.get());
        AddChild(submit_button_);
    }

    {
        status_messsage_ = std::make_shared<Text>(L"");
        status_messsage_->Move(-1.5F, 0, 0);
        status_messsage_->SetFontSize(26);
        AddChild(status_messsage_);
    }

    // sync mode frome java.
    getServerAddr();
    onInputModeChange(InputMode_None);
    View::Init();
}

void SetupServerAddr::HandleInput(Ray *rays, int rayCount) {
    View::HandleInput(rays, rayCount);
    bool isEnter = Input::IsInputEnter();
    // change input mode.
    if (server_ip_button_->picked() && isEnter) {
        onInputModeChange(InputMode_Ip);
    } else if (server_port_button_->picked() && isEnter) {
        onInputModeChange(InputMode_Port);
    }

    // 设置服务器地址
    if (submit_button_->picked() && isEnter) {
        setServerAddr();
    }
}

void SetupServerAddr::onInput(const Keyboard::Key &key) {
    LOGV("=========on input %s", key.keyValue.c_str());
    bool isInput = false;
    bool isEnter = false;
    if (input_mode_ == InputMode_Ip) {
        std::string ip = utils::WStringToString(server_ip_button_->GetText());
        if (key.keyType == Keyboard::KeyType_Input) {
            ip += key.keyValue;
        } else {
            // edl
            if (key.keyType == Keyboard::KeyTpye_Del) {
                if (!ip.empty())
                    ip.pop_back();
            } else if (key.keyType == Keyboard::KeyTpye_Enter) {
                isEnter = true;
            }
        }
        server_ip_button_->SetText(ip.empty() ? L"" : utils::StringToWstring(ip));
    } else if (input_mode_ == InputMode_Port) {
        std::string port = utils::WStringToString(server_port_button_->GetText());
        if (key.keyType == Keyboard::KeyType::KeyType_Input) {
            port += key.keyValue;
        } else {
            if (key.keyType == Keyboard::KeyTpye_Del) {
                if (!port.empty())
                    port.pop_back();
            } else if (key.keyType == Keyboard::KeyTpye_Enter) {
                isEnter = true;
            }
        }
        server_port_button_->SetText(port.empty() ? L"" : utils::StringToWstring(port));
    }

    if (isEnter) {
        keyboard_->set_active(false);
        // save to change.
        setServerAddr();
    } else {
        if (currnet_mode_ != Mode_Manual) {
            currnet_mode_ = Mode_Manual;
            // setmode to manual.
//            setMode();
            mode_text_->SetText(currnet_mode_ == Mode_Audo ?
                               localization::Loader::getResource().ui_setup_serveraddr_auto_mode
                               : localization::Loader::getResource().ui_setup_serveraddr_hand_mode);
        }
    }
}

void SetupServerAddr::Enter() {
    View::Enter();

//    getMode();
    getServerAddr();

    if (lark::XRClient::GetServerHost() == "") {
        status_messsage_->SetText(L"请设置服务器地址");
    }
    is_detecting_ = false;
}

void SetupServerAddr::Leave() {
    View::Leave();

    onInputModeChange(InputMode_None);

    status_messsage_->SetText(L"");
    is_detecting_ = false;
}

void SetupServerAddr::onInputModeChange(InputMode mode) {
    input_mode_ = mode;
    server_ip_button_->setSelected(input_mode_ == InputMode_Ip);
    server_port_button_->setSelected(input_mode_ == InputMode_Port);
    if (input_mode_ != InputMode_None) {
        keyboard_->set_active(true);
        keyboard_->SetTitle(input_mode_ == InputMode_Ip ? localization::Loader::getResource().ui_setup_serveraddr_input_ip
                                                       : localization::Loader::getResource().ui_setup_serveraddr_input_port);
    } else {
        keyboard_->set_active(false);
        keyboard_->SetTitle(localization::Loader::getResource().ui_keyboard_number);
    }
}

void SetupServerAddr::getServerAddr() {
    // TODO 服务器地址
    server_ip_button_->SetText(utils::StringToWstring(lark::XRClient::GetServerHost()));
    uint16_t port = lark::XRClient::GetServerPort();
    std::string s_port = port == 0 ? std::to_string(DEFAULT_PORT) : std::to_string(port);
    server_port_button_->SetText(utils::StringToWstring(s_port));
}

void SetupServerAddr::setServerAddr() {
    std::string ip = utils::WStringToString(server_ip_button_->GetText());
    uint16_t port = (uint16_t)std::stoi(utils::WStringToString(server_port_button_->GetText()));

    if (Home::instance()) {
        Home::instance()->ResetAppPageInfo();
    }

    Application* app =  Application::instance();
    if (app != nullptr) {
        app->SetServerAddr(ip, port);
    }
    if (!is_detecting_) {
        std::thread thread(get_applist_adapter_);
        thread.detach();
//        Poco::ThreadPool::defaultPool().start(get_applist_adapter_);
    } else {
        Navigation::ShowToast("正在检测中");
    }
}

void SetupServerAddr::GetAppList() {
    status_messsage_->SetText(L"检测服务器版本号", false);
    lark::GetCoreServerVersionInfo getCoreServerVersionInfo;
    getCoreServerVersionInfo.Send();
    if (getCoreServerVersionInfo.is_success()) {
        lark::CoreServerVersionInfo info = getCoreServerVersionInfo.core_server_version_info();
        LOGV("core server version info %d.%d.%d.%d; sdk version %d.%d.%d.%d",
             info.marjor, info.minor, info.revise, info.build,
             LARK_NATIVE_CLIENT_SDK_VERSION_MARJOR,
             LARK_NATIVE_CLIENT_SDK_VERSION_MINOR,
             LARK_NATIVE_CLIENT_SDK_VERSION_REVISE,
             LARK_NATIVE_CLIENT_SDK_VERSION_BUILD);
        if (info.marjor == LARK_NATIVE_CLIENT_SDK_VERSION_MARJOR && info.minor == LARK_NATIVE_CLIENT_SDK_VERSION_MINOR) {
            status_messsage_->SetText(L"检测服务器版本号成功", false);
        } else {
            char buffer[100];
            sprintf(buffer, "检测到与服务器版本号不匹配，服务器版本 [%d.%d] 客户端 SDK 版本 [%d.%d]",
                                              info.marjor, info.minor,
                                              LARK_NATIVE_CLIENT_SDK_VERSION_MARJOR,
                                              LARK_NATIVE_CLIENT_SDK_VERSION_MINOR);
            status_messsage_->SetText(utils::StringToWstring(buffer), false);
            // reset status
            is_detecting_ = false;
            return;
        }
    } else {
        std::string errMsg = "检测服务器版本号请求失败，与服务器版本可能不匹配, 请检查地址是否正确或联系服务器管理员获得帮助。Err:"
                + getCoreServerVersionInfo.message();
        status_messsage_->SetText(utils::StringToWstring(errMsg), false);
        return;
    }

    status_messsage_->SetText(L"发送请求获取应用列表检测服务器地址", false);

    lark::GetAppliList getAppliList;
    getAppliList.Send();
    if (getAppliList.is_success()) {
        status_messsage_->SetText(L"当前地址检测成功。点击返回按钮可返回首页。", false);
    } else {
        std::string errMsg = "应用列表请求失败，请检查地址是否正确或联系服务器管理员获得帮助。Err:" + getAppliList.message();
        status_messsage_->SetText(utils::StringToWstring(errMsg), false);
    }
    // reset status
    is_detecting_ = false;
}

//
// input buttons
//
SetupServerAddr::InputButton::InputButton(const std::wstring &title):Button(title) {
    Init();
}

void SetupServerAddr::InputButton::Init() {
    Base::set_size(glm::vec2(INPUT_BOX_WIDTH, INPUT_BOX_HEIGHT));

    line_ = std::make_shared<ColorBox>();
    line_->set_size(glm::vec2(Base::size_.x, 0.01F));
    line_->set_color(INPUT_LINE_COLOR);
    AddChild(line_);

    title_->SetFontSize(26);
    title_->set_position(0, 0.025, 0);
    AddChild(title_);

    SetAABBSize(Base::size_);
}

void SetupServerAddr::InputButton::clearState() {
    picked_ = false;
    setSelected(false);
}

glm::vec2 SetupServerAddr::InputButton::GetSize() {
    return Base::size_;
}

void SetupServerAddr::InputButton::HandleInput(glm::vec2 *point, int pointCount) {
    Button::HandleInput(point, pointCount);
}

void SetupServerAddr::InputButton::setSelected(bool selected) {
    line_->set_color(selected ? INPUT_LINE_COLOR_ACTIVE : INPUT_LINE_COLOR);
}