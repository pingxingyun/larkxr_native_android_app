//
// Created by fcx@pingixngyun.com on 2019/11/19.
//

#include <log.h>
#include "lark_xr/xr_client.h"
#include "loading.h"
#define LOG_TAG "ui_loading"

Loading::Loading(Navigation *navigation):
    View(navigation),
    center_loader_(lark::CompanyImageLoader::CompanyImageType_Logo, this),
    footer_loader_(lark::CompanyImageLoader::CompanyImageType_Footer, this)
{
    Loading::Init();
}

Loading::~Loading() = default;

void Loading::Init() {
    // disable backbtn.
    if (back_btn_)
        back_btn_->set_active(false);
    if (bg_)
        bg_->set_active(false);

    icon_ = std::make_shared<Image>();
    icon_->set_position(glm::vec3(-icon_->GetSize().x / 2.0F, 0, 0));
    icon_->set_callback(this);
    AddChild(icon_);

    footer_ = std::make_shared<Image>();
    footer_->Move(glm::vec3(1.2, -1.2, 0));
    AddChild(footer_);

    text_ = std::make_shared<Text>(L"Loading...");
    text_->set_color(0xd7e1FF, 0xFF);
    text_->SetFontSize(30);
    text_->Move(-text_->width() / 2.0F, -0.3F, 0);
    AddChild(text_);

    larkxrSystemInfo systemInfo = lark::XRClient::system_info();
    std::wstring quit = L"";
/*    if (systemInfo.platFromType == larkxrPlatFromType::Larkxr_Platform_Oculus_Quest ||
        systemInfo.platFromType == larkxrPlatFromType::Larkxr_Platform_PICO_NEO_2) {
        quit = L"按住手柄扳机键并短按B或Y键可退出云端应用返回列表。";
    } else {
        quit = L"按住手柄扳机键并短按APP键可退出云端应用返回列表。";
    }*/
    quit = L"通过HOME键返回应用列表。";
    tips_ = std::make_shared<Text>(L"");
    tips_->set_color(0xd7e1FF, 0xFF);
    tips_->SetFontSize(24);
    // force update width.
    tips_->SetText(L"Tips: " + quit);
    tips_->Move(-tips_->width() / 2.0F, -0.8F, 0);
    AddChild(tips_);
    View::Init();
}


void Loading::OnImageInited(Image *img) {
    LOGV("On image loaded");
    img->set_position(glm::vec3(-img->GetSize().x / 2.0F, 0, 0));
}

void Loading::OnImageLoadSuccess(lark::CompanyImageLoader::CompanyImageType type, const char *data,
                                 int size) {
    if (type == lark::CompanyImageLoader::CompanyImageType_Logo) {
        LOGV("on load logo success");
        icon_->LoadTexture(data, size);
    } else if (type == lark::CompanyImageLoader::CompanyImageType_Footer) {
        LOGV("on load footer logo success");
        footer_->LoadTexture(data, size);
    }
    load_success_  = true;
}

void Loading::OnImageLoadFailed(const std::string &err) {
    LOGV("OnImageLoade faield %s", err.c_str());
    load_success_  = false;
}

void Loading::Enter() {
    View::Enter();
    if (first_load_ || !load_success_) {
        center_loader_.SendAsync();
        footer_loader_.SendAsync();
        first_load_ = false;
    }
}

void Loading::SetQuitTips(const std::wstring &tips) {
    if (tips_) {
        tips_->SetText(L"Tips: " + tips);
        tips_->Move(-tips_->width() / 2.0F, -0.8F, 0);
    }
}

