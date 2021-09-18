//
// Created by fcx@pingxingyung.com on 2021/4/30.
// Copyright (c) 2021 www.pingxingyun.com All rights reserved.
//

#include <log.h>
#include "menu_view.h"

MenuView::MenuView(Callback* callback): View(nullptr), callback_(callback) {
    MenuView::Init();
}

MenuView::~MenuView() {
    // clear aabb.
    ClearAABB();
}

void MenuView::Init() {
    {
        bg_ = std::make_shared<ColorBox>();
        bg_->set_size(glm::vec2(1.5, 1.5));
        bg_->set_color(0x000530A2);
        bg_->set_position(0, 0, -0.01);
        AddChild(bg_);
    }

    {
        text_ = std::make_shared<Text>(L"确定退出当前云端应用？");
        text_->set_color(0xd7e1FF, 0xFF);
        text_->SetFontSize(25);
        text_->set_position(0.1, 1.2, 0);
        AddChild(text_);
    }

    {
        glm::vec3 p(0.7,0.5,0);
        btn_submit_ = std::make_shared<TextButton>(L"退出");
        btn_submit_->Move(p);
        btn_submit_->SetFontSize(24);
        btn_submit_->SetAABBPositon(glm::vec2(p.x, p.y));
        PushAABB(btn_submit_.get());
        AddChild(btn_submit_);
    }

    {
        glm::vec3 p(1.1,0.5,0);
        btn_cancle_ = std::make_shared<TextButton>(L"继续");
        btn_cancle_->Move(p);
        btn_cancle_->SetFontSize(24);
        btn_cancle_->SetAABBPositon(glm::vec2(p.x, p.y));
        PushAABB(btn_cancle_.get());
        AddChild(btn_cancle_);
    }

    // advance btn
//    {
//        glm::vec3 p(0, 0, 0);
//        advance_btn_ = std::make_shared<TextButton>(L"高级设置");
//        advance_btn_->Move(p);
//        advance_btn_->SetAABBPositon(glm::vec2(p.x, p.y));
//        PushAABB(advance_btn_.get());
//        AddChild(advance_btn_);
//    }

    View::back_btn_->set_active(false);
    View::Init();

//    plane_ = {
//            glm::vec3(0, 0, 1), // normal
//            glm::vec3(-0.75, -0.75, -1.5 + 0.2), // dot
//    };
}

void MenuView::Enter() {
    View::Enter();
}

void MenuView::Leave() {
    View::Leave();
}

void MenuView::Update() {
    Object::Update();

    if (btn_submit_->picked() && Input::IsInputEnter()) {
        LOGV("Menu view on submit");
        if (callback_ != nullptr) {
            callback_->OnMenuViewSelect(true);
        }
    }

    if (btn_cancle_->picked() && Input::IsInputEnter()) {
        LOGV("Menu view on cancle");
        if (callback_ != nullptr) {
            callback_->OnMenuViewSelect(false);
        }
    }
}