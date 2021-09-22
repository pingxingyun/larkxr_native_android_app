//
// Created by fcx@pingixngyun.com on 2019/11/15.
//

#ifndef CLOUDLARK_OCULUS_DEMO_NAVIGATION_H
#define CLOUDLARK_OCULUS_DEMO_NAVIGATION_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "object.h"
#include "ui/component/text.h"
#include "ui/home/home.h"
#include "ui/loading/loading.h"
#include "ui/setup/setup.h"
#include "ui/setup_server/setup_server_addr.h"
#include "view.h"

class Navigation: public lark::Object {
public:
    // static routers.
    enum ROUTERS {
        HOME = 0,
        SETUP,
        LOADING,
        SETUP_SERVERADDR,
        SETUP_ADVANCE,
    };
    static void ShowToast(const std::string & msg);
    static void ClearToast();

    Navigation();
    ~Navigation();
    void SetRouter(ROUTERS router);
    ROUTERS current() { return  current_; };

    virtual void Update() override;
    void HandelInput(lark::Ray * rays, int rayCount);
    void SetLoadingTips(const std::wstring& tipstr);

    void SetSupport2DUI();
private:
    // static faileds
    static std::string s_toast_str_;
    static bool        s_show_toast_;
    static uint64_t    s_last_toast_timestamp_;
    //
    View * GetViewByRouter(ROUTERS router);
    View * GetCurrentView();
    void CheckInfo();

    std::shared_ptr<Text> toast_;
    uint64_t last_show_toast_ = 0;

    ROUTERS current_;

    std::shared_ptr<Home>             home_page_;
    std::shared_ptr<Setup>            setup_;
    std::shared_ptr<Loading>          loading_;
    std::shared_ptr<SetupServerAddr>  setup_server_addr_;
};


#endif //CLOUDLARK_OCULUS_DEMO_NAVIGATION_H
