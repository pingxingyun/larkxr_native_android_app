//
// Created by Administrator on 2021/8/10.
//
#include <log.h>
#include <lark_xr/xr_client.h>
#include "scene_local.h"
#include "larkxr_client.h"
#define LOG_TAG "pvr_scene_local"

SceneLocal::SceneLocal():
        sky_box_(),
        controller_left_(),
        controller_right_(),
        navigation_(),
        test_obj_()
{

}

SceneLocal::~SceneLocal() = default;

void
SceneLocal::InitGl(LarkxrClient *client, int eyeBufferWidth, int eyeBufferHeight) {
    SceneBase::InitGl(eyeBufferWidth, eyeBufferHeight);

    client_ = client;

    LOGV("obj size %ld", objects_.size());

    LOGV("skybox %p", sky_box_.get());

    sky_box_ = std::make_shared<lark::SkyBox>("textures/skybox_8_2k.jpg");
    SceneBase::AddObject(sky_box_);

    LOGV("skybox %p", sky_box_.get());

    lark::ControllerConfig controllerConfig = {
            "model/nolo_controller_m1_left/nolo_controller_m1_left.obj",
            "model/nolo_controller_m1_right/nolo_controller_m1_right.obj",
            glm::vec3(-0.009, 0.006, -0.007),
            glm::vec3(0.009, 0.006, -0.007),
            1.0f,
            glm::half_pi<float>() / 3.0F,
            glm::vec3(1,0,0),
            glm::vec4(1, 1, 1, 1),
            {
                    lark::Model::MaterialTextureType_DIFFUSE,
                    "oculus_quest_controller.png"
            }
    };

    // controllerConfig.modelRotate = -glm::half_pi<float>() / 3.0F;
    // controllerConfig.modelRotateAxis = glm::vec3(1, 0, 0);

    // controllers
    controller_left_ = std::make_shared<lark::Controller>(true, controllerConfig);
    controller_left_->Move(-0.3, 0, -0.3);
//     add to pvr_xr_scene;
    SceneBase::AddObject(controller_left_);

    controller_right_ = std::make_shared<lark::Controller>(false, controllerConfig);
    controller_right_->Move(0.3, 0, -0.3);
    // add to pvr_xr_scene;
    SceneBase::AddObject(controller_right_);

    // navigation.
    navigation_ = std::make_shared<Navigation>();
    navigation_->Move(0, 1.5, 0);
    SceneBase::AddObject(navigation_);

//    test_obj_.reset();
//    LOGV("test obj count %ld test obj %p", test_obj_.use_count(), test_obj_.get());
//    test_obj_ = std::make_shared<TestObj>();
//    test_obj_->Move(-0.3, 0, -2.3);
//    AddObject(test_obj_);

//    test_image_ = std::make_shared<Image>();
//    test_image_->SetPath("textures/skybox_8_2k.jpg", true);
//    test_image_->Move(-0.3, 0, -1.5);
//    AddObject(test_image_);
//    LOGV("test obj count %ld test obj %p", test_obj_.use_count(), test_obj_.get());
    LOGV("init gl finished");

    LOGV("obj size %ld", objects_.size());
}

void SceneLocal::UpdateHMDPose(glm::quat rotation, glm::vec3 position) {
    SceneBase::UpdateHMDPose(rotation, position);
}

void SceneLocal::Draw(int eye) {
    SceneBase::Draw(eye);
    GLenum err = glGetError();
    if (err == GL_OUT_OF_MEMORY) {
        LOGE("glGetError(): GL_OUT_OF_MEMORY");
    } else if (err != GL_NO_ERROR) {
        LOGE("glGetError(): %d", err);
    } else {
//        LOGV("gl no error");
    }
}

void SceneLocal::HomePage() {
    LOGV("================HomePage");
    navigation_->SetRouter(Navigation::HOME);
}

void SceneLocal::HandleInput(const larkxrDevicePair& devicePair) {
    bool backButtonDownThisFrame[Input::RayCast_Count] = {false, false};
    bool triggerDownThisFrame[Input::RayCast_Count] = {false, false};
    bool enterButtonDownThisFrame[Input::RayCast_Count] = {false, false};

    // 0->left, 1->right, 2->hmd
    lark::Ray rays[Input::RayCast_Count] = {};
    Input::InputState * inputState = Input::GetInputState();
    for (int i = 0; i < Input::RayCast_Count; i++) {
        // skip hmd
        if (i == Input::RayCast_Hmd) {
            continue;
        }

        bool isLeft = i == 0;
        Input::RayCastType rayCastType = isLeft ? Input::RayCast_left : Input::RayCast_Right;
        Transform transform(devicePair.controllerState[i].pose.rotation,
                            devicePair.controllerState[i].pose.position);
        transform.Rotate(devicePair.controllerState[i].rotateDeg, devicePair.controllerState[i].rotateAxis);
        Ray *ray = nullptr;
        if (isLeft) {
            controller_left_->set_transform(transform);
            // controller_left_->set_active(devicePair.controllerState[i].inputState.isConnected);
            ray = &rays[0];
        } else {
            controller_right_->set_transform(transform);
            // controller_right_->set_active(devicePair.controllerState[i].inputState.isConnected);
            ray = &rays[1];
        }
        ray->ori = transform.GetPosition();
        ray->dir = transform.Forward();

        backButtonDownThisFrame[rayCastType] |=
                devicePair.controllerState[i].inputState.buttons & LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_B_Click);
        backButtonDownThisFrame[rayCastType] |=
                devicePair.controllerState[i].inputState.buttons & LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Y_Click);
        backButtonDownThisFrame[rayCastType] |=
                devicePair.controllerState[i].inputState.buttons & LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Application_Menu_Click);

        enterButtonDownThisFrame[rayCastType] |=
                devicePair.controllerState[i].inputState.buttons & LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_X_Click);
        enterButtonDownThisFrame[rayCastType] |=
                devicePair.controllerState[i].inputState.buttons & LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_A_Click);

        triggerDownThisFrame[rayCastType] |=
                devicePair.controllerState[i].inputState.buttons & LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Trigger_Click);

        bool backButtonDownLastFrame = back_button_down_last_frame_[rayCastType];
        back_button_down_last_frame_[rayCastType] = backButtonDownThisFrame[rayCastType];

        bool traggerButtonDownLastFrame = trigger_button_down_last_frame_[rayCastType];
        trigger_button_down_last_frame_[rayCastType] = triggerDownThisFrame[rayCastType];

        bool enterButtonDownLastFrame = enter_button_down_last_frame_[rayCastType];
        enter_button_down_last_frame_[rayCastType] = enterButtonDownThisFrame[rayCastType];

        inputState[rayCastType].backShortPressed = backButtonDownLastFrame && !backButtonDownThisFrame[rayCastType];
        inputState[rayCastType].enterShortPressed = enterButtonDownLastFrame && !enterButtonDownThisFrame[rayCastType];
        inputState[rayCastType].triggerShortPressed = traggerButtonDownLastFrame && !triggerDownThisFrame[rayCastType];
        inputState[rayCastType].backButtonDown = backButtonDownThisFrame[rayCastType];
        inputState[rayCastType].enterButtonDown = enterButtonDownThisFrame[rayCastType];
        inputState[rayCastType].triggerButtonDown = triggerDownThisFrame[rayCastType];

        //             call after pressup.
        if ( inputState[rayCastType].backShortPressed)
        {
//                LOGV( "back button short press %d", rayCastType);
            OnCloseApp();
        }

        // call ater pressup.
        if (inputState[rayCastType].triggerShortPressed) {
//                LOGV( "trigger button short press %d", rayCastType);
            Input::SetCurrentRayCastType(rayCastType);
        }
    }

    // update ui ray.
    navigation_->HandelInput(rays, 2);
}

void SceneLocal::OnCloseApp() {
    if (Application::instance()) {
        Application::instance()->CloseAppli();
    }
}

void SceneLocal::Draw(int eye, const glm::mat4 &project, const glm::mat4 &view) {
    SceneBase::Draw(eye, project, view);
    GLenum err = glGetError();
    if (err == GL_OUT_OF_MEMORY) {
        LOGE("glGetError(): GL_OUT_OF_MEMORY");
    } else if (err != GL_NO_ERROR) {
        LOGE("glGetError(): %d", err);
    } else {
//        LOGV("gl no error");
    }
}
