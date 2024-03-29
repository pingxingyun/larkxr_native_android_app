//
// Created by fcx@pingixngyun.com on 2019/12/4.
//

#ifndef CLOUDLARKVRDEMO_SETUP_SERVER_ADDR_H
#define CLOUDLARKVRDEMO_SETUP_SERVER_ADDR_H

#include "ui/view.h"
#include "ui/component/keyboard.h"
#include "lark_xr/request/region_rtt_test.h"
#include "lark_xr/request/region_list.h"

class SetupServerAddrListener;
class SetupServerAddr: public View,
        public Keyboard::Callback,
        public lark::GetRegionListListener,
        public lark::RegionRttTestListener {
public:
    enum Mode {
        Mode_Audo   = 0,
        Mode_Manual = 1,
    };

    enum InputMode {
        InputMode_Ip   = 0,
        InputMode_Port = 1,
        InputMode_None = 1000,
    };

    struct RegionTestResult {
        uint64_t rtt;
        bool selected;
        lark::RegionInfo info;
    };

    class InputButton: public Button {
    public:
        explicit InputButton(const std::wstring &title);

        void setSelected(bool selected);
        void clearState();
        // component base.
        virtual glm::vec2 GetSize() override;
        // aabb
        virtual void HandleInput(glm::vec2 * point, int pointCount) override;
    protected:
        void Init();
    private:
//        bool mSelected;
        std::shared_ptr<ColorBox> line_;
    };

    SetupServerAddr(Navigation *navigation);
    ~SetupServerAddr();

    // switch to current page.
    virtual void Enter() override;
    // leave current page.
    virtual void Leave() override;
    // keyboard input
    virtual void onInput(const Keyboard::Key &key) override;
    // handle input
    virtual void HandleInput(lark::Ray * rays, int rayCount) override;
    // update every frame
    virtual void Update() override;

    // GetRegion callback
    virtual void OnSuccess(bool detectRttFlag, const std::vector<lark::RegionInfo>& appliPageInfo) override;
    virtual void OnFailed(const std::string& msg) override;

    // region_rtt_test callback
    virtual void OnRegionRttTestResult(uint64_t rtt, const std::string& regionId) override;
    virtual void OnRegionRttTestError(const std::string& err, const std::string& regionId) override;
    virtual void OnRegionRttTestClose(const std::string& err, const std::string& regionId) override;

    inline RegionTestResult selected_region_result() { return selected_result_; }

    virtual void set_listener(SetupServerAddrListener* callback) { callback_ = callback; };
protected:
    virtual void Init() override;

private:
//    void getMode();
//    void setMode();
    void getServerAddr();
    void setServerAddr();
//    void autoMode();
    void onInputModeChange(InputMode mode);
    //
    void GetAppList();

    Mode currnet_mode_ = Mode_Manual;
    InputMode input_mode_ = InputMode_None;

    std::shared_ptr<Text> title_;
    std::shared_ptr<Text> label_server_ip_;
    std::shared_ptr<Text> label_server_port_;
    std::shared_ptr<Text> label_mode_;

    std::shared_ptr<InputButton> server_ip_button_;
    std::shared_ptr<InputButton> server_port_button_;
    std::shared_ptr<Text> mode_text_;

    std::shared_ptr<TextButton> submit_button_;
//    std::shared_ptr<TextButton> auto_mode_button_;

    std::shared_ptr<Keyboard> keyboard_;

    std::shared_ptr<Text> status_messsage_;

    std::function<void()> get_applist_adapter_;
    bool is_detecting_ = false;

    std::shared_ptr<lark::RegionRttTest> region_rtt_test_ = {};
    std::shared_ptr<lark::GetRegionList> get_region_list_ = {};
    std::vector<RegionTestResult> results_ = {};
    RegionTestResult selected_result_ = { 0, false };

    std::vector<std::shared_ptr<TextButton>> region_list_ = {};
    std::shared_ptr<Text> region_title_ = {};

    int current_test_ = 0;
    std::mutex region_mutex_ = {};
    bool need_update_region_ = false;

    SetupServerAddrListener* callback_ = nullptr;
};

class SetupServerAddrListener {
public:
    virtual void OnUpdateRegion(const SetupServerAddr::RegionTestResult& result) = 0;
};


#endif //CLOUDLARKVRDEMO_SETUP_SERVER_ADDR_H
