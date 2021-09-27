//
// Created by fcx on 2020/5/7.
//

#ifndef CLOUDLARKXR_WVR_SCENE_CLOUD_H
#define CLOUDLARKXR_WVR_SCENE_CLOUD_H


#include <skybox.h>
#include <lark_xr/types.h>
#include <lark_xr/xr_tracking_frame.h>
#include <ui/controller.h>
#include <ui/menu_view.h>
#include "wvr_scene.h"
#include "ui/loading/loading.h"
#include "rect_texture.h"

class WvrSceneCloud: public WvrScene, public MenuView::Callback {
public:
    using WvrScene::WvrScene;
    ~WvrSceneCloud();
    bool InitGL(void* left_eye_queue, void* right_eye_qeue,
                std::vector<WvrFrameBuffer*>* left_eye_fbo, std::vector<WvrFrameBuffer*>* right_eye_fbo) override;
    bool ShutdownGL() override;

    void OnConnect();
    void OnMediaReady();
    void OnMediaReady(int nativeTexture);
    void OnClose();

    void Update() override;
    void UpdateAsync(larkxrTrackingDevicePairFrame* devicePairFrame);
    bool Render() override;
    bool Render(const larkxrTrackingFrame& trackingFrame);
    bool Render(const larkxrTrackingFrame& trackingFrame, const lark::XRVideoFrame& videoFrame);
    bool HandleInput() override;

    virtual void OnMenuViewSelect(bool submit) override;
private:
    void ShowMenu();
    void HideMenu();
    void OnCloseApp();

    bool back_button_down_last_frame_[Input::RayCast_Count]{};
    bool trigger_button_down_last_frame_[Input::RayCast_Count]{};
    bool enter_button_down_last_frame_[Input::RayCast_Count]{};

    std::shared_ptr<lark::SkyBox> sky_box_;
    std::shared_ptr<Loading> loading_;
    std::shared_ptr<lark::Controller> controller_left_{};
    std::shared_ptr<lark::Controller> controller_right_{};
    std::shared_ptr<lark::Object> fake_hmd_;
    std::shared_ptr<MenuView> menu_view_;

    std::shared_ptr<RectTexture> rect_texture_{};
    larkxrDevicePair device_pair_{};

    uint64_t frame_index_ = 0;
};


#endif //CLOUDLARKXR_WVR_SCENE_CLOUD_H
