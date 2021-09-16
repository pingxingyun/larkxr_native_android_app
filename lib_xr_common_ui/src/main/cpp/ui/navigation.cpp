//
// Created by fcx@pingixngyun.com on 2019/11/15.
//

#include <utils.h>
#include "lark_xr/xr_client.h"
#include "navigation.h"
#include "log.h"

#define LOG_TAG "Navigation"

// statics
void Navigation::ShowToast(const std::string &msg) {
    if (msg.size() > 100) {
        s_toast_str_ = msg.substr(0, 100);
    } else {
        s_toast_str_ = msg;
    }
    s_show_toast_ = true;
    s_last_toast_timestamp_ = utils::GetTimestampUs();
}
void Navigation::ClearToast() {
    s_toast_str_ = "";
    s_show_toast_ = false;
    s_last_toast_timestamp_ = 0;
}
std::string Navigation::s_toast_str_;
bool Navigation::s_show_toast_ = false;
uint64_t Navigation::s_last_toast_timestamp_ = 0;
//
Navigation::Navigation() :
      home_page_(new Home(this)),
      setup_(new Setup(this)),
      loading_(new Loading(this)),
      setup_server_addr_(new SetupServerAddr(this)),
      toast_(new Text(L""))
{
    bool ui_3d_mode = Application::instance()->ui_mode() == Application::ApplicationUIMode_Opengles_3D;
    current_ = ui_3d_mode ? ROUTERS::HOME : ROUTERS::LOADING;

    // add chiledren.
    home_page_->set_active(ui_3d_mode);
    home_page_->Move(View::VIEW_POSITION_X, component::CAMERA_ORI_POSITION.y, View::VIEW_POSITION_Z);
    AddChild(home_page_);

    setup_->set_active(false);
    setup_->Move(View::VIEW_POSITION_X, component::CAMERA_ORI_POSITION.y, View::VIEW_POSITION_Z);
    AddChild(setup_);

    loading_->set_active(!ui_3d_mode);
    loading_->Move(View::VIEW_POSITION_X, component::CAMERA_ORI_POSITION.y, View::VIEW_POSITION_Z);
    AddChild(loading_);

    setup_server_addr_->set_active(false);
    setup_server_addr_->Move(View::VIEW_POSITION_X, component::CAMERA_ORI_POSITION.y, View::VIEW_POSITION_Z);
    AddChild(setup_server_addr_);

    toast_->Move(View::VIEW_POSITION_X - 2.0F, component::CAMERA_ORI_POSITION.y  - 1.9F, View::VIEW_POSITION_Z + 0.5F);
    toast_->set_active(false);
    AddChild(toast_);

    if (lark::XRClient::GetServerHost().empty()) {
        LOGV("server ip is empty.");
        SetRouter(Navigation::ROUTERS::SETUP_SERVERADDR);
        ShowToast("请设置服务器地址");
    } else {
        if (ui_3d_mode) {
            home_page_->Enter();
        } else {
            loading_->Enter();
        }
    }
}

Navigation::~Navigation() = default;


void Navigation::SetRouter(Navigation::ROUTERS router) {
    // TODO add navigation base.
    // switch to new page.
    if (current_ != router) {
        View * currentView = GetCurrentView();
        View * next = GetViewByRouter(router);
        if (currentView == nullptr || next == nullptr)
            return;

        currentView->Leave();
        currentView->set_active(false);

        next->Enter();
        next->set_active(true);

        current_ = router;
        LOGV("set app navigation router: %d", router);
    }
}

View *Navigation::GetViewByRouter(Navigation::ROUTERS router) {
    switch (router) {
        case HOME:
            return home_page_.get();
        case LOADING:
            return loading_.get();
        case SETUP:
            return setup_.get();
        case SETUP_SERVERADDR:
            return setup_server_addr_.get();
        case SETUP_ADVANCE:
            break;
    }
    return nullptr;
}

View *Navigation::GetCurrentView() {
    return GetViewByRouter(current_);
}

void Navigation::CheckInfo() {
    if (!s_show_toast_)
        return;
//    LOGV("show tost1: %d" , toast_->isActive());
    if (!toast_->active()) {
//        LOGV("show tost1: %s" , s_toast_str_.c_str());
        // init toast.
        toast_->SetText(utils::StringToWstring(s_toast_str_), true);
        toast_->set_active(true);
        last_show_toast_ = s_last_toast_timestamp_;
    } else {
        if (last_show_toast_ != s_last_toast_timestamp_) { // update
            toast_->SetText(utils::StringToWstring(s_toast_str_), true);
            last_show_toast_ = s_last_toast_timestamp_;
        }
    }

    uint64_t now = utils::GetTimestampUs();
    // show toast for 5 seconds
    if (now - s_last_toast_timestamp_ > 1000 * 1000 * 5) {
        s_show_toast_ = false;
        s_last_toast_timestamp_ = 0;
        s_toast_str_ = "";
        toast_->SetText(L"");
        toast_->set_active(false);
    }
}

void Navigation::Update() {
    CheckInfo();
    Object::Update();
}

void Navigation::HandelInput(lark::Ray *ray, int rayCount) {
    GetCurrentView()->HandleInput(ray, rayCount);
}

void Navigation::SetLoadingTips(const std::wstring &tips) {
    if (loading_) {
        loading_->SetQuitTips(tips);
    }
}
