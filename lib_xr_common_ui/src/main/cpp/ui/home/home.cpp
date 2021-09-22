//
// Created by fcx@pingixngyun.com on 2019/11/15.
//

#include <ui/navigation.h>
#include <log.h>
#include <build_config.h>
//#include "cloudlarkvr/system.h"
#include "utils.h"
#include "ui/localization.h"
#include "home.h"
#include "lark_xr/xr_client.h"
#define LOG_TAG "ui_home"

using namespace glm;
using namespace std;

namespace {
    // blue.
    const char * PAGE_DOT           = "textures/ui/circle.png";
    // white.
    const char * PAGE_DOT_ACTIVIVE  = "textures/ui/circle_active.png";
}

std::string Home::s_client_id_ = "";

Home* Home::s_instance_ = nullptr;

Home *Home::instance() {
    return s_instance_;
}

void Home::SetClientId(const std::string & clientId) {
    s_client_id_ = clientId;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
Home::Home(Navigation *appNavigation):
    fps_str_(),
    View(appNavigation),
    pre_check_app_item_change_(),
    run_mode_(VrRunMode_Self),
    client_id_str_(""),
    app_list_task_(this, false),
    logo_loader_(lark::CompanyImageLoader::CompanyImageType_Header, this)
{
    // save global instance
    s_instance_ = this;
    Home::Init();
}

Home::~Home() {
    s_instance_ = nullptr;
}

void Home::Init() {
    run_mode_ = VrRunMode_Self;
    // disable backbtn.
    if (back_btn_)
        back_btn_->set_active(false);
    // Init logo
    logo_ = std::make_shared<Image>();
    logo_->Move(-2.37F, 1.325F, 0);
    AddChild(logo_);

#ifdef USE_EXT_LOGO
    mLogo2 = std::make_shared<Image>();
    mLogo2->SetPath("textures/ui/logo_china_unicom.png", true);
    mLogo2->Move(-1.7F, 1.26, 0);
    mLogo2->scale(0.28);
    AddChild(mLogo2);
#endif

    // Init title
    title_ = std::make_shared<Text>(localization::Loader::getResource().ui_home_title);
    title_->set_color(vec4(0.843F, 0.882F, 1.0F, 0.5F));
    title_->Move( -title_->width() / 2.0F + 0.15F, 1.325F, 0);
    AddChild(title_);
    // Init setupserver button
    {
        glm::vec2 p{};
        setup_server_button_ = std::make_shared<TextButton>(localization::Loader::getResource().ui_home_setup_serveraddr);
        p.x = 1.9F - setup_server_button_->GetSize().x / 2.0F;
        p.y = 1.325F - 0.3F;
        setup_server_button_->Move(p.x, p.y, 0);
        setup_server_button_->set_color(vec4(0.843F, 0.882F, 1.0F, 0.5F));
        setup_server_button_->ClearStatus();
        // add to aabb
        setup_server_button_->SetAABBPositon(glm::vec2(p.x, p.y - setup_server_button_->GetSize().y / 2.0F));
        PushAABB(setup_server_button_.get());
        AddChild(setup_server_button_);
    }
    // Init setup button
    {
        glm::vec2 p{};
        setup_button_ = std::make_shared<SetupButton>();
        p.x = 1.9F;
        p.y = 1.325F;
        setup_button_->Move(p.x, p.y, 0);
        // add to aabb
        setup_button_->SetAABBPositon(glm::vec2(p.x, p.y - 0.1));
        PushAABB(setup_button_.get());
        AddChild(setup_button_);
    }
    // page down button
    {
        glm::vec2 p{};
        page_down_button_ = std::make_shared<PageButton>(false);
        page_down_button_->set_active(false);
        p.x = 2.825F;
        p.y = -1.475F;
        page_down_button_->Move(p.x, p.y, 0);
        page_down_button_->SetAABBPositon(glm::vec2(p.x - page_down_button_->GetSize().x / 2.0F, p.y));
        PushAABB(page_down_button_.get());
        AddChild(page_down_button_);
    }
    // page up button
    {
        glm::vec2 p{};
        page_up_button_ = std::make_shared<PageButton>(true);
        page_up_button_->set_active(false);
        p.x = -2.75F;
        p.y = -1.475F;
        page_up_button_->Move(p.x, p.y, 0);
        page_up_button_->SetAABBPositon(glm::vec2(p.x, p.y));
        PushAABB(page_up_button_.get());
        AddChild(page_up_button_);
    }
    // Init advance setup button. no use
    {
        glm::vec2 p{};
        advance_setup_button_ = std::make_shared<TextButton>(localization::Loader::getResource().ui_home_advance_setting);
        advance_setup_button_->set_color(vec4(0.843F, 0.882F, 1.0F, 0.5F));
        p.x = 1.9F - advance_setup_button_->GetSize().x / 2.0F;
        p.y = -1.85F;
        advance_setup_button_->Move(p.x, p.y, 0);
        advance_setup_button_->ClearStatus();
        advance_setup_button_->SetAABBPositon(glm::vec2(p.x, p.y - advance_setup_button_->GetSize().y / 2.0F));
//        PushAABB(advance_setup_button_.get());
//        AddChild(advance_setup_button_);
    }
    // quit 3d ui button
    {
        glm::vec2 p{};
        quit_3d_ui_button_ = std::make_shared<TextButton>(localization::Loader::getResource().ui_home_quick_3d_ui);
        quit_3d_ui_button_->set_color(vec4(0.843F, 0.882F, 1.0F, 0.5F));
        p.x = 1.9F - quit_3d_ui_button_->GetSize().x / 2.0F;
        p.y = -1.6F;
        quit_3d_ui_button_->Move(p.x, p.y, 0);
        // add to aabb
        quit_3d_ui_button_->SetAABBPositon(glm::vec2(p.x, p.y));
        quit_3d_ui_button_->ClearStatus();
        PushAABB(quit_3d_ui_button_.get());
        AddChild(quit_3d_ui_button_);
        quit_3d_ui_button_->set_active(false);
    }
    larkxrSystemInfo systemInfo = lark::XRClient::system_info();
    // device name
    {
//        systemInfo.deviceName
        std::string deviceName = systemInfo.deviceName;
        device_name_ = std::make_shared<Text>(localization::Loader::getResource().ui_home_device_name + L": " +  utils::StringToWstring(deviceName));
        device_name_->Move(-2.37F, -1.6F, 0);
        device_name_->SetFontSize(22);
        AddChild(device_name_);
    }
    // mac 地址
    {
//        systemInfo.macAddress
        std::string macAddress = systemInfo.macAddress;
        mac_address_ = std::make_shared<Text>(localization::Loader::getResource().ui_home_mac_address + L": " + utils::StringToWstring(macAddress));
        mac_address_->Move(-2.37F, -1.75F, 0);
        mac_address_->SetFontSize(22);
        AddChild(mac_address_);
    }
    // client id
    {
        client_id_ = std::make_shared<Text>(localization::Loader::getResource().ui_home_client_id);
        client_id_->Move(-2.37F, -1.9F, 0);
        client_id_->SetFontSize(22);
        AddChild(client_id_);
    }
    // app version
    {
//        systemInfo.appVersion
        std::string appVersion = BuildConfig::version_name();
        app_version_ = std::make_shared<Text>(localization::Loader::getResource().ui_home_app_version + L": V" + utils::StringToWstring(appVersion));
        app_version_->Move(-2.37F, -2.05F, 0);
        app_version_->SetFontSize(22);
        AddChild(app_version_);
    }
    // support surver version
    {
//        systemInfo.supportServerVersion
        char buffer[50];
        sprintf(buffer, "%d.%d.+", LARK_NATIVE_CLIENT_SDK_VERSION_MARJOR, LARK_NATIVE_CLIENT_SDK_VERSION_MINOR);
        support_server_version_ = std::make_shared<Text>(localization::Loader::getResource().ui_home_support_server_version + L": V" + utils::StringToWstring(buffer));
        support_server_version_->Move(-2.37F, -2.20F, 0);
        support_server_version_->SetFontSize(22);
        AddChild(support_server_version_);
    }
    // run moide
    {
        teacher_mode_ = std::make_shared<Text>(localization::Loader::getResource().ui_home_run_mode_teacher);
        teacher_mode_->Move(-teacher_mode_->width() / 2.0F + 0.15F, 0.25, 0);
        teacher_mode_->set_active(false);
        AddChild(teacher_mode_);
    }
    // empty list
    {
        empty_list_ = std::make_shared<Text>(L"");
        empty_list_->SetFontSize(24);
        empty_list_->SetText(L"当前服务器上空空如也。");
        empty_list_->Move(-empty_list_->width() / 2.0F + 0.15F, -1.6F, 0);
        empty_list_->set_active(false);
        AddChild(empty_list_);
    }
    // Init items
    for (int i = 0; i < MAX_PAGE_ITEM_NUM; i ++) {
        int row  = i / ROW_ITEM_NUM;
        int line = i % ROW_ITEM_NUM;
        CoverItem * coverItem = new CoverItem(navigation_, L"");
        glm::vec2 p(-2.37F + (line * 1.3F), -0.155F - row * 1.175F);
        coverItem->Move(p.x, p.y, 0);
        // setup aabb position;
        coverItem->SetAABBPositon(p);
        // add to check list.
        PushAABB(coverItem);
        app_cover_items_[i].reset(coverItem);
        AddChild(app_cover_items_[i]);
    }
    // Init page dots
    for (auto &d : page_dot_) {
        auto * dot = new Image();
        dot->SetPath(PAGE_DOT, true);
        dot->Move(-0.5F, -1.65F, 0);
        dot->set_active(false);
        d.reset(dot);
        AddChild(d);
    }

    app_list_task_.SetPageSize(MAX_PAGE_ITEM_NUM);
    View::Init();
}

void Home::setFps(float &fps) {
    if (fps != fps_) {
        fps_ = fps;
        fps_str_ = L"FPS:" + to_wstring((int)fps_ + 1);
    }
}

void Home::Enter() {
    View::Enter();
    LOGV("Enter Homepage");
    // start fetch applist.
    app_list_task_.Start();
    // reload logo
    if (!load_success_) {
        logo_loader_.SendAsync();
    }
}

void Home::Leave() {
    View::Leave();
    LOGV("Leave Homepage");
    // stop fetch applist.
    app_list_task_.Stop();
}

void Home::HandleInput(lark::Ray * rays, int rayCount) {
    View::HandleInput(rays, rayCount);
    // handle inputs.
    if (setup_button_->picked() && Input::IsInputEnter()) {
        // go to setup page.
        navigation_->SetRouter(Navigation::SETUP);
    }
    //
    if (setup_server_button_->picked() && Input::IsInputEnter()) {
        navigation_->SetRouter(Navigation::SETUP_SERVERADDR);
    }
    if (page_up_button_->picked() && Input::IsInputEnter()) {
        ChangePage(false);
    }
    if (page_down_button_->picked() && Input::IsInputEnter()) {
        ChangePage(true);
    }
    if (quit_3d_ui_button_->active() && quit_3d_ui_button_->picked() && Input::IsInputEnter()) {
        Application::instance()->Quit3DUI();
    }
}

void Home::ChangePage(bool isDown) {
    // already ended or first.
    if ((isDown && !app_page_info_.hasNextPage) || (!isDown && !app_page_info_.hasPreviousPage))
        return;
    if (isDown) {
        app_list_task_.SetPage(app_page_info_.nextPage);
        UpdateAppList(1);
    } else {
        app_list_task_.SetPage(app_page_info_.prePage);
        UpdateAppList(-1);
    }
}

void Home::Update() {
    Object::Update();
    // check app item change every second;
    time_t now = time(nullptr);
    if (now - pre_check_app_item_change_ > 1) {
        // check client id change.
        UpdateClientId();
        // reload logo
        if (!load_success_) {
            logo_loader_.SendAsync();
        }
        pre_check_app_item_change_ = now;
    }
    {
//        Poco::ScopedLock<Poco::Mutex> lock(update_applist_mutex_);
        if (has_new_data_) {
            UpdateAppList(0);
        }
    }
    {
        std::lock_guard<std::mutex> lock(run_mode_change_mutex_);
        if (run_mode_change_) {
            UpdateRunMode();
            run_mode_change_ = false;
        }
    }
}

/**
 *
 * @param page 0 page not change. 1 page up -1 page down
 */
void Home::UpdateAppList(int page) {
    // teacher run mode.
    if (run_mode_ == VrRunMode_Teacher) {
        return;
    }
    // self run mode.

    total_page_num_ = app_page_info_.pages;
    current_page_ = app_page_info_.pageNum - 1 + page;
    // clear app num when update page.
    app_num_ = page == 0 ? app_page_info_.list.size() : 0;

    // update item
    for (int i = 0; i < MAX_PAGE_ITEM_NUM; i++) {
        auto coverItem = app_cover_items_[i].get();
        if (i >= app_num_) {
            coverItem->set_is_empty(true);
            continue;
        }
        lark::AppliInfo *item = &app_page_info_.list[i];
        if (coverItem != nullptr) {
            coverItem->set_app_id(item->appliId);
            coverItem->set_is_empty(false);
            coverItem->SetTitle(utils::StringToWstring(item->appliName));
//            coverItem->SetCoverUrl("textures/ui/cover_10.png", true);
            if (item->picUrl.empty() || item->picUrl == "") {
//                LOGV("update cover %s; use empty", item->picUrl.c_str());
                coverItem->SetCoverUrl("textures/ui/cover_11.jpg", true);
            } else {
//                LOGV("update cover %s", item->picUrl.c_str());
                coverItem->SetCoverUrl(item->picUrl, false);
            }
        }
    }
    // update page dot
    float offset = component::CAMERA_ORI_POSITION.x -  total_page_num_ * 0.125F / 2.0F + 0.2F;
    for (int i = 0; i < MAX_PAGE_NUM; i ++) {
        auto dot = page_dot_[i].get();
        if (i < total_page_num_) {
            // show dot
            dot->set_active(true);
            glm::mat4 transform(1.0f);
            transform = glm::translate(transform, glm::vec3(offset + i * 0.125F, -1.75F, 0));
            dot->set_transform(transform);
            if (i == current_page_)
                dot->SetPath(PAGE_DOT_ACTIVIVE, true);
            else
                dot->SetPath(PAGE_DOT, true);
        } else {
            dot->set_active(false);
        }
    }
    // update page
    page_down_button_->set_active(app_page_info_.hasNextPage);
    page_up_button_->set_active(app_page_info_.hasPreviousPage);
    // pullall data.
    has_new_data_ = false;
}

void Home::UpdateClientId() {
    if (client_id_str_ != s_client_id_) {
        // update client id.
        client_id_str_ = s_client_id_;
        client_id_->SetText(localization::Loader::getResource().ui_home_client_id + L": " + utils::StringToWstring(client_id_str_));
    }
}

void Home::UpdateRunMode() {
    teacher_mode_->set_active(!run_mode_ == VrRunMode_Self);
    for(auto &item: app_cover_items_) {
        item->set_active(run_mode_ == VrRunMode_Self);
    }
    page_down_button_->set_active(run_mode_ == VrRunMode_Self);
    page_up_button_->set_active(run_mode_ == VrRunMode_Self);
    for (auto &dot: page_dot_) {
        dot->set_active(run_mode_ == VrRunMode_Self);
    }
}

void Home::OnAppListInfo(const std::vector<lark::AppliInfo> &appliInfo) {
}

void Home::OnAppListPageInfo(const lark::AppliPageInfo& appliPageInfo) {
    // SetAppPageInfo(appliPageInfo);

    {
//        Poco::ScopedLock<Poco::Mutex> lock(update_applist_mutex_);
        has_new_data_ = true;
        app_page_info_ = appliPageInfo;
    }
    if (empty_list_) {
        empty_list_->set_active(appliPageInfo.list.size() == 0);
    }
    if (first_load_) {
        logo_loader_.SendAsync();
        first_load_ = false;
    }
}

void Home::OnFailed(const std::string &msg) {
    LOGV("============applist OnFailed %s", msg.c_str());
    Navigation::ShowToast(msg);
    {
//        Poco::ScopedLock<Poco::Mutex> lock(update_applist_mutex_);
        has_new_data_ = true;
        // clear list
        app_page_info_.list.clear();
    }
    if (empty_list_) {
        empty_list_->set_active(false);
    }
}

void Home::OnRunMode(lark::GetVrClientRunMode::ClientRunMode runMode) {
//    LOGV("============OnRunModeChange new %d old %d", runMode, run_mode_);
    std::lock_guard<std::mutex> lock(run_mode_change_mutex_);
    VrRunMode mode = runMode == lark::GetVrClientRunMode::CLIENT_RUNMODE_SELF ?
                     VrRunMode_Self : VrRunMode_Teacher;
    if (mode != run_mode_) {
        run_mode_ = mode;
        run_mode_change_ = true;
    }
}

void Home::OnImageLoadSuccess(lark::CompanyImageLoader::CompanyImageType type, const char* data, int size) {
    LOGV("load header logo success");
    logo_->LoadTexture(data, size);
    load_success_ = true;
}

void Home::OnImageLoadFailed(const std::string &err) {
    LOGV("load header logo failed %s", err.c_str());
    load_success_ = false;
}

void Home::ResetAppPageInfo() {
//    Poco::ScopedLock<Poco::Mutex> lock(update_applist_mutex_);
    has_new_data_ = true;
    // clear list
    app_page_info_ = {};

    app_list_task_.SetPage(1);
}

void Home::SetSupport2DUI() {
    if (quit_3d_ui_button_) {
        quit_3d_ui_button_->set_active(true);
    }
}
