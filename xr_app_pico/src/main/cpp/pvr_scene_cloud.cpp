//
// Created by fcx on 2020/7/2.
//

#include <log.h>
#include <application.h>
#include "pvr_scene_cloud.h"
#include "pvr_application.h"

PvrSceneCloud::PvrSceneCloud() {
}

PvrSceneCloud::~PvrSceneCloud() {
}

void PvrSceneCloud::InitGl(int eyeBufferWidth, int eyeBufferHeight) {
    PvrScene::InitGl(eyeBufferWidth, eyeBufferHeight);

    LOGV("obj size %lld", objects_.size());

    sky_box_ = std::make_shared<lark::SkyBox>("textures/skybox_8_2k.jpg");
    PvrScene::AddObject(sky_box_);

    loading_ = std::make_shared<Loading>(nullptr);
    loading_->Move(View::VIEW_POSITION_Y, View::VIEW_POSITION_X, View::VIEW_POSITION_Z);
    PvrScene::AddObject(loading_);

    lark::ControllerConfig controllerConfig = {};
    larkxrSystemInfo systemInfo = lark::XRClient::system_info();
    if (systemInfo.platFromType == larkxrPlatFromType::Larkxr_Platform_PICO_NEO_2 ||
        systemInfo.platFromType == larkxrPlatFromType::Larkxr_Platform_PICO_NEO_3) {
        controllerConfig = lark::CONTROLLER_PICO_NEO_2;
    } else  {
        controllerConfig = lark::CONTROLLER_PICO_NEO;
    }
    // controllers
    controller_left_ = std::make_shared<lark::Controller>(true, controllerConfig);
    controller_left_->Move(-0.3, 0, -0.3);
//    // add to pvr_xr_scene;
    PvrScene::AddObject(controller_left_);

    controller_right_ = std::make_shared<lark::Controller>(false, controllerConfig);
    controller_right_->Move(0.3, 0, -0.3);
    // add to pvr_xr_scene;
    PvrScene::AddObject(controller_right_);

    rect_texture_ = std::make_shared<RectTexture>();
    PvrScene::AddObject(rect_texture_);

    LOGV("obj size %lld", objects_.size());
}

void PvrSceneCloud::Draw(int eye) {
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

void PvrSceneCloud::UpdateHMDPose(glm::quat rotation, glm::vec3 position) {
    PvrScene::UpdateHMDPose(rotation, position);
    HandleInput();
}

void PvrSceneCloud::HandleInput() {
    bool backButtonDownThisFrame[Input::RayCast_Count] = {false, false};
    bool triggerDownThisFrame[Input::RayCast_Count] = {false, false};
    bool enterButtonDownThisFrame[Input::RayCast_Count] = {false, false};

    // 0->left, 1->right., 2->hmd
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
        lark::Transform transform(controllerState.pose.rotation, controllerState.pose.position);

        lark::Ray *ray = nullptr;
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
        if (inputState[rayCastType].triggerButtonDown && inputState[rayCastType].backShortPressed)
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
}

void PvrSceneCloud::OnCloseApp() {
    LOGV("================on close app");
    // 集中管控模式下由教师端控制退出应用。
    if (lark::AppListTask::run_mode() == lark::GetVrClientRunMode::ClientRunMode::CLIENT_RUNMODE_TEACHER) {
        return;
    }
    Application::instance()->CloseAppli();
}

void PvrSceneCloud::OnMediaReady(int nativeTexture) {
    LOGV("================OnMediaReady %d", nativeTexture);
    controller_left_->set_active(false);
    controller_right_->set_active(false);
    rect_texture_->SetMutiviewModeTexture(nativeTexture);
}

void PvrSceneCloud::OnMediaReady(int nativeTextrueLeft, int nativeTextureRight) {
    LOGV("================OnMediaReady %d %d", nativeTextrueLeft, nativeTextureRight);
    controller_left_->set_active(false);
    controller_right_->set_active(false);
    rect_texture_->SetStereoTexture(nativeTextrueLeft, nativeTextureRight);
}

void PvrSceneCloud::OnMediaReady() {
    LOGV("================OnMediaReady");
    controller_left_->set_active(false);
    controller_right_->set_active(false);
}

void PvrSceneCloud::OnClose() {
    LOGV("================OnClose");
    controller_left_->set_active(true);
    controller_right_->set_active(true);
    rect_texture_->ClearTexture();
}

void PvrSceneCloud::OnConnect() {
    LOGV("================OnConnect");
    if (loading_) {
        loading_->Enter();
    }
}

void PvrSceneCloud::SetVideoFrame(const lark::XRVideoFrame &videoFrame) {
    LOGV("================SetVideoFrame");
    if (videoFrame.frame_type() == lark::XRVideoFrame::FrameType::kNative_Multiview) {
        rect_texture_->SetMutiviewModeTexture(videoFrame.texture());
    } else if (videoFrame.frame_type() == lark::XRVideoFrame::FrameType::kNative_Stereo) {
        rect_texture_->SetStereoTexture(videoFrame.texture_left(), videoFrame.texture_right());
    }
}
