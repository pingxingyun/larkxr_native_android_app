//
// Created by fcx@pingixngyun.com on 2019/11/25.
//

#ifndef CLOUDLARKVRDEMO_SCENE_CLOUD_H
#define CLOUDLARKVRDEMO_SCENE_CLOUD_H

#include <lark_xr/xr_tracking_frame.h>
#include <ui/controller.h>
#include "ovr_scene.h"
//#include "object/cloudlarkvr_render.h"
#include "ui/loading/loading.h"
#include "rect_texture.h"
#include "ui/menu_view.h"

class OvrSceneCloud: public OvrScene, public MenuView::Callback {
public:
    OvrSceneCloud();
    ~OvrSceneCloud() override;

    // scene.
    bool InitGL(OvrFrameBuffer *frame_buffer, int num_buffers) override;
//    bool InitJava(ovrJava * java, bool useMultiView) override;
    bool ShutdownGL() override;

    void OnConnect();
    void OnMediaReady();
    void OnMediaReady(int nativeTexture);
    void OnMediaReady(int nativeTextrueLeft, int nativeTextureRight);
    void OnClose();

    bool Update(ovrMobile * ovr) override;

    bool UpdateAsync(ovrMobile *ovr);

    bool HandleInput(ovrMobile * ovr) override;
    // handle input on render thread.
    void HandleInput();
    bool Render(ovrMobile * ovr) override;

    bool Render(ovrMobile * ovr, const larkxrTrackingFrame& trackingFrame);
    bool Render(ovrMobile * ovr, const larkxrTrackingFrame& trackingFrame, const lark::XRVideoFrame& videoFrame);

    inline larkxrTrackingDevicePairFrame device_pair_frame() { return device_pair_frame_; }

    virtual void OnMenuViewSelect(bool submit) override;
private:
    void ShowMenu();
    void HideMenu();
    void OnCloseApp();

    larkxrDevicePair GetDevicePair(ovrMobile *ovr, double preditTime);

    bool back_button_down_last_frame_[Input::RayCast_Count]{};
    bool trigger_button_down_last_frame_[Input::RayCast_Count]{};
    bool enter_button_down_last_frame_[Input::RayCast_Count]{};

    std::shared_ptr<lark::SkyBox> sky_box_;
    std::shared_ptr<Loading> loading_;
    std::shared_ptr<lark::Controller> controller_left_;
    std::shared_ptr<lark::Controller> controller_right_;
    std::shared_ptr<lark::Object> fake_hmd_;
    std::shared_ptr<MenuView> menu_view_;

    std::shared_ptr<RectTexture> rect_texture_{};
    larkxrTrackingDevicePairFrame device_pair_frame_{};

    uint64_t tracking_frame_index_ = 0;

};


#endif //CLOUDLARKVRDEMO_SCENE_CLOUD_H
