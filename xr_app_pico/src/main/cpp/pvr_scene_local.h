//
// Created by fcx on 2020/6/30.
//

#ifndef CLOUDLARKXR_PVR_SCENE_LOCAL_H
#define CLOUDLARKXR_PVR_SCENE_LOCAL_H

#include <ui/component/color_box.h>
#include <ui/component/border.h>
#include <ui/component/slider.h>
#include <ui/home/home.h>
#include <ui/controller.h>
#include <ui/loading/loading.h>
#include <test_obj.h>
#include "ui/navigation.h"
#include "skybox.h"
#include "pvr_scene.h"

class PvrSceneLocal: public PvrScene {
public:
    PvrSceneLocal();
    ~PvrSceneLocal();

    virtual void InitGl(int eyeBufferWidth, int eyeBufferHeight) override;
    virtual void UpdateHMDPose(glm::quat rotation, glm::vec3 position) override;
    virtual void Draw(int eye) override;

    void HomePage();
    void LoadingPage();
private:
    void HandleInput();
    void OnCloseApp();

    std::shared_ptr<lark::SkyBox> sky_box_ = nullptr;
    std::shared_ptr<lark::Controller> controller_left_ = nullptr;
    std::shared_ptr<lark::Controller> controller_right_ = nullptr;
    std::shared_ptr<Navigation> navigation_ = nullptr;
    std::shared_ptr<TestObj> test_obj_ = nullptr;
    std::shared_ptr<Image> test_image_ = nullptr;

    bool back_button_down_last_frame_[Input::RayCast_Count]{};
    bool trigger_button_down_last_frame_[Input::RayCast_Count]{};
    bool enter_button_down_last_frame_[Input::RayCast_Count]{};
};


#endif //CLOUDLARKXR_PVR_SCENE_LOCAL_H
