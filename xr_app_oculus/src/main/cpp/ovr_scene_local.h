//
// Created by fcx@pingxingyun.com on 2019/10/25.
//

#ifndef MY_APPLICATION_SCENE_LOCAL_H
#define MY_APPLICATION_SCENE_LOCAL_H

#include <ui/component/color_box.h>
#include <ui/component/border.h>
#include <ui/component/slider.h>
#include <ui/home/home.h>
#include <ui/controller.h>
#include <ui/loading/loading.h>
#include "ui/navigation.h"
#include "rect_texture.h"
#include "ovr_scene.h"
#include "ui/menu_view.h"

class OvrSceneLocal:public OvrScene {
public:
    OvrSceneLocal();
    ~OvrSceneLocal() override;

    // scene.
    bool InitGL(OvrFrameBuffer *frame_buffer, int num_buffers) override;
//    bool InitJava(ovrJava * java, bool useMultiView) override;
    bool ShutdownGL() override;

    bool Update(ovrMobile * ovr) override;

    void HomePage();
    void LoadingPage();
protected:
    bool HandleInput(ovrMobile * ovr) override;
    bool Render(ovrMobile * ovr) override;

private:
    void OnCloseApp();

    bool back_button_down_last_frame_[Input::RayCast_Count]{};
    bool trigger_button_down_last_frame_[Input::RayCast_Count]{};
    bool enter_button_down_last_frame_[Input::RayCast_Count]{};

    std::shared_ptr<lark::SkyBox> sky_box_;
    std::shared_ptr<lark::Controller> controller_left_;
    std::shared_ptr<lark::Controller> controller_right_;

    std::shared_ptr<Navigation> navigation_;
};


#endif //MY_APPLICATION_SCENE_LOCAL_H
