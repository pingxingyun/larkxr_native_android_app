//
// Created by fcx@pingxingyung.com on 2021/4/30.
// Copyright (c) 2021 www.pingxingyun.com All rights reserved.
//

#ifndef CLOUDLARKXR_MENU_VIEW_H
#define CLOUDLARKXR_MENU_VIEW_H

#include "view.h"
#include "navigation.h"

class MenuView: public View {
public:
    class Callback {
    public:
        virtual void OnMenuViewSelect(bool submit) = 0;
    };

    MenuView(Callback* callback);
    ~MenuView();
    // update evey frame called by render thread.
    virtual void Update() override;

protected:
    virtual void Init() override;
    // switch to current page.
    virtual void Enter() override;
    // leave current page.
    virtual void Leave() override;
private:
    std::shared_ptr<Text> text_;
    std::shared_ptr<TextButton> advance_btn_;
    std::shared_ptr<TextButton> btn_cancle_;
    std::shared_ptr<TextButton> btn_submit_;
    std::shared_ptr<ColorBox> bg_;
    Callback* callback_;
};


#endif //CLOUDLARKXR_MENU_VIEW_H
