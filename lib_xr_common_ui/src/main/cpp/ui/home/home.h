//
// Created by fcx@pingixngyun.com on 2019/11/15.
//

#ifndef CLOUDLARK_OCULUS_DEMO_HOME_H
#define CLOUDLARK_OCULUS_DEMO_HOME_H

#include <object.h>
#include <ui/component/button.h>
#include "ui/view.h"
#include "cover_item.h"
#include "lark_xr/app_list_task.h"
#include "lark_xr/request/company_image_loader.h"

class Navigation;
class Home: public View, public lark::AppListTask::AppListTaskListener, public lark::CompanyImageLoader::CompanyImageLoaderCallback {
public:
    enum VrRunMode {
        VrRunMode_Self = 0,
        VrRunMode_Teacher = 1,
    };
private:
    static Home* s_instance_;
    static std::string s_client_id_;
public:
    static Home* instance();

    static const int PICKED_NONE = -1;
    static const int MAX_PAGE_ITEM_NUM = 8;
    static const int MAX_PAGE_NUM = 25;
    static const int ROW_ITEM_NUM = 4;
    static const int TOTAL_ITEM_DATA = 200;

    static void SetClientId(const std::string & clientId);

    explicit Home(Navigation * appNavigation);
    ~Home();

    void setFps(float & fps);

    // update evey frame called by render thread.
    virtual void Update() override;
    // switch to current page.
    virtual void Enter() override;
    // leave current page.
    virtual void Leave() override;
    virtual void HandleInput(lark::Ray * rays, int rayCount) override;

    // applist callback
    virtual void OnAppListInfo(const std::vector<lark::AppliInfo> & appliInfo) override;
    virtual void OnAppListPageInfo(const lark::AppliPageInfo& appliPageInfo) override;
    virtual void OnFailed(const std::string& msg) override;
    virtual void OnRunMode(lark::GetVrClientRunMode::ClientRunMode runMode) override;

    // load lopo
    virtual void OnImageLoadSuccess(lark::CompanyImageLoader::CompanyImageType type, const char* data, int size) override;
    virtual void OnImageLoadFailed(const std::string& err) override;

    void ResetAppPageInfo();

    void SetSupport2DUI();
protected:
    virtual void Init() override;
private:
    void ChangePage(bool isDown);
    // 更新列表
    void UpdateAppList(int page);
    // 更新客户端id
    void UpdateClientId();
    void UpdateRunMode();

    std::shared_ptr<Text> title_;
    std::shared_ptr<CoverItem> app_cover_items_[MAX_PAGE_ITEM_NUM] = {};
    std::shared_ptr<Image> page_dot_[MAX_PAGE_NUM] = {};

    uint64_t total_page_num_ = 0;
    uint64_t app_num_ = 0;
    uint64_t current_page_ = 0;
    int current_picked_ = PICKED_NONE;
    float fps_ = 0;

    std::wstring fps_str_;

    std::shared_ptr<Text> device_name_;
    std::shared_ptr<Text> mac_address_;
    std::shared_ptr<Text> client_id_;
    std::shared_ptr<Text> app_version_;
    std::shared_ptr<Text> support_server_version_;

    std::shared_ptr<Image> logo_;
    std::shared_ptr<TextButton> setup_server_button_;
    std::shared_ptr<TextButton> advance_setup_button_;
    std::shared_ptr<SetupButton> setup_button_;
    std::shared_ptr<TextButton> quit_3d_ui_button_;

    std::shared_ptr<PageButton> page_down_button_;
    std::shared_ptr<PageButton> page_up_button_;

    std::shared_ptr<Text> teacher_mode_;

    std::shared_ptr<Text> empty_list_;

    time_t pre_check_app_item_change_;

    std::string client_id_str_ = "";

    VrRunMode run_mode_ = VrRunMode_Self;
    bool run_mode_change_ = false;
    std::mutex run_mode_change_mutex_ = {};

    // appli info task
    lark::AppListTask app_list_task_;
    lark::CompanyImageLoader logo_loader_;
    bool first_load_ = true;
    bool load_success_ = false;

//    Poco::Mutex update_applist_mutex_ = {};
    bool has_new_data_ = false;
    lark::AppliPageInfo app_page_info_ = {};

    bool support_2d_ui_ = false;
};


#endif //CLOUDLARK_OCULUS_DEMO_HOME_H
