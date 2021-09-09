//
// Created by fcx@pingxingyun.com on 2021/8/10.
//

#ifndef NOLOLARKXR_SCENE_LOCAL_H
#define NOLOLARKXR_SCENE_LOCAL_H


#include <ui/component/color_box.h>
#include <ui/component/border.h>
#include <ui/component/slider.h>
#include <ui/home/home.h>
#include <ui/controller.h>
#include <ui/loading/loading.h>
#include <test_obj.h>
#include "ui/navigation.h"
#include "skybox.h"
#include "scene_base.h"

class LarkxrClient;
class SceneLocal: public SceneBase {
public:
    SceneLocal();
    ~SceneLocal();

    void InitGl(LarkxrClient* client, int eyeBufferWidth, int eyeBufferHeight);
    virtual void UpdateHMDPose(glm::quat rotation, glm::vec3 position) override;
    void HandleInput(const larkxrDevicePair& devicePair);
    virtual void Draw(int eye) override;
    virtual void Draw(int eye, const glm::mat4& project, const glm::mat4& view) override;

    void HomePage();

private:
    void OnCloseApp();

    LarkxrClient* client_ = nullptr;

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


#endif //NOLOLARKXR_SCENE_LOCAL_H
