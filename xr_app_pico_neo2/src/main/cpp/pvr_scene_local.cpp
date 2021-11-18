//
// Created by fcx on 2020/6/30.
//

#include <log.h>
#include "pvr_scene_local.h"
#include "pvr_utils.h"
#include "pvr_application.h"

#define LOG_TAG "pvr_scene_local"

using namespace lark;

PvrSceneLocal::PvrSceneLocal():
        sky_box_(),
        controller_left_(),
        controller_right_(),
        navigation_(),
        test_obj_()
{
}

PvrSceneLocal::~PvrSceneLocal() = default;

void PvrSceneLocal::InitGl(int eyeBufferWidth, int eyeBufferHeight) {
    PvrScene::InitGl(eyeBufferWidth, eyeBufferHeight);

    LOGV("obj size %ld", objects_.size());

    LOGV("skybox %p", sky_box_.get());

    sky_box_ = std::make_shared<SkyBox>("textures/skybox_8_2k.jpg");
    PvrScene::AddObject(sky_box_);

    LOGV("skybox %p", sky_box_.get());

    ControllerConfig controllerConfig = {};
    larkxrSystemInfo systemInfo = lark::XRClient::system_info();
    if (systemInfo.platFromType == larkxrPlatFromType::Larkxr_Platform_PICO_NEO_2 ||
            systemInfo.platFromType == larkxrPlatFromType::Larkxr_Platform_PICO_NEO_3) {
        controllerConfig = CONTROLLER_PICO_NEO_2;
    } else  {
        controllerConfig = CONTROLLER_PICO_NEO;
    }

    // controllers
    controller_left_ = std::make_shared<Controller>(true, controllerConfig);
    controller_left_->Move(-0.3, 0, -0.3);
//     add to pvr_xr_scene;
    PvrScene::AddObject(controller_left_);

    controller_right_ = std::make_shared<Controller>(false, controllerConfig);
    controller_right_->Move(0.3, 0, -0.3);
    // add to pvr_xr_scene;
    PvrScene::AddObject(controller_right_);

    // navigation.
    navigation_ = std::make_shared<Navigation>();
    navigation_->SetSupport2DUI();
    PvrScene::AddObject(navigation_);

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

void PvrSceneLocal::UpdateHMDPose(glm::quat rotation, glm::vec3 position) {
    PvrScene::UpdateHMDPose(rotation, position);
    HandleInput();
}

void PvrSceneLocal::Draw(int eye) {
    PvrScene::Draw(eye);
    GLenum err = glGetError();
    if (err == GL_OUT_OF_MEMORY) {
        LOGE("glGetError(): GL_OUT_OF_MEMORY");
    } else if (err != GL_NO_ERROR) {
        LOGE("glGetError(): %d", err);
    } else {
//        LOGV("gl no error");
    }
}

void PvrSceneLocal::HandleInput() {
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

        pvr::PvrControllerState controllerState = ((PvrApplication*)PvrApplication::instance())->GetControllerState(
                static_cast<pvr::PvrControllerType>(i));

        bool isLeft = i == 0;
        Input::RayCastType rayCastType = isLeft ? Input::RayCast_left : Input::RayCast_Right;
        Transform transform(controllerState.pose.rotation, controllerState.pose.position);

        Ray *ray = nullptr;
        if (isLeft) {
            controller_left_->set_transform(transform);
            controller_left_->set_active(controllerState.input.isConnected);
            ray = &rays[0];
        } else {
            controller_right_->set_transform(transform);
            controller_right_->set_active(controllerState.input.isConnected);
            ray = &rays[1];
        }
        ray->ori = transform.GetPosition();
        ray->dir = transform.Forward();

        backButtonDownThisFrame[rayCastType] |= controllerState.input.buttonB;
        backButtonDownThisFrame[rayCastType] |= controllerState.input.buttonY;
        backButtonDownThisFrame[rayCastType] |= controllerState.input.app;

        enterButtonDownThisFrame[rayCastType] |= controllerState.input.buttonA;
        enterButtonDownThisFrame[rayCastType] |= controllerState.input.buttonX;
        enterButtonDownThisFrame[rayCastType] |= controllerState.input.joyStick;

        triggerDownThisFrame[rayCastType] |= controllerState.input.trigger;

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
            //OnCloseApp();
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

void PvrSceneLocal::HomePage() {
    LOGV("================HomePage");
    navigation_->SetRouter(Navigation::HOME);
}

void PvrSceneLocal::LoadingPage() {
    LOGV("================LoadingPage");
    navigation_->SetRouter(Navigation::LOADING);
}

void PvrSceneLocal::OnCloseApp() {
    LOGV("================on close app");
    // TODO 关闭应用
    Application::instance()->CloseAppli();
}

