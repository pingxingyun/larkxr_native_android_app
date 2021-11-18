//
// Created by fcx on 2020/7/2.
//

#ifndef CLOUDLARKXR_PVR_SCENE_CLOUD_H
#define CLOUDLARKXR_PVR_SCENE_CLOUD_H

#include <input.h>
#include <ui/controller.h>
#include <ui/loading/loading.h>
#include <rect_texture.h>
#include "skybox.h"
#include "pvr_scene.h"
#include "ui/menu_view.h"
#include "pvr_utils.h"

class PvrSceneCloud: public PvrScene, public MenuView::Callback {
public:
    PvrSceneCloud();
    ~PvrSceneCloud();

    void OnConnect();
    void OnMediaReady(int nativeTexture);
    void OnMediaReady(int nativeTextrueLeft, int nativeTextureRight);
    void OnMediaReady();
    void OnClose();

    virtual void InitGl(int eyeBufferWidth, int eyeBufferHeight) override;
    virtual void UpdateHMDPose(glm::quat rotation, glm::vec3 position) override;
    virtual void Draw(int eye) override;
    virtual void SetVideoFrame(const lark::XRVideoFrame& videoFrame);

    virtual void OnMenuViewSelect(bool submit);

    inline bool IsShowMenu() { return menu_view_->active(); }
private:
    void HandleInput();
    void ShowMenu();
    void HideMenu();
    void OnCloseApp();

    std::shared_ptr<lark::SkyBox> sky_box_ = nullptr;
    std::shared_ptr<Loading> loading_ = nullptr;
    std::shared_ptr<lark::Controller> controller_left_ = nullptr;
    std::shared_ptr<lark::Controller> controller_right_ = nullptr;

    bool back_button_down_last_frame_[Input::RayCast_Count]{};
    bool trigger_button_down_last_frame_[Input::RayCast_Count]{};
    bool enter_button_down_last_frame_[Input::RayCast_Count]{};

    std::shared_ptr<RectTexture> rect_texture_{};

    std::shared_ptr<lark::Object> fake_hmd_;
    std::shared_ptr<MenuView> menu_view_;

    pvr::PvrPose hmd_pose_{};
};


#endif //CLOUDLARKXR_PVR_SCENE_CLOUD_H
