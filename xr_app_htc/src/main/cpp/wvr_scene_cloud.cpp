//
// Created by fcx on 2020/5/7.
//

#include <env_context.h>
#include <application.h>
#include <log.h>
#include <lark_xr/xr_latency_collector.h>
#include <lark_xr/app_list_task.h>
#include "wvr_scene_cloud.h"
#include "wvr_utils.h"
#include "utils.h"
#define LOG_TAG "wvr_scene_cloud"


WvrSceneCloud::~WvrSceneCloud() = default;


bool WvrSceneCloud::InitGL(void* left_eye_queue, void* right_eye_qeue,
                           std::vector<WvrFrameBuffer*>* left_eye_fbo, std::vector<WvrFrameBuffer*>* right_eye_fbo) {
    sky_box_ =  std::make_shared<lark::SkyBox>("textures/skybox_8_2k.jpg");
    WvrScene::AddObject(sky_box_);

    loading_ = std::make_shared<Loading>(nullptr);
    loading_->Move(View::VIEW_POSITION_Y, View::VIEW_POSITION_X, View::VIEW_POSITION_Z);
    WvrScene::AddObject(loading_);

    rect_texture_ = std::make_shared<RectTexture>();
//    rect_texture_->set_active(false);
    WvrScene::AddObject(rect_texture_);

    larkxrSystemInfo info = lark::XRClient::system_info();
    lark::ControllerConfig controllerConfig = lark::CONTROLLER_HTC_FOCUS_PLUS;
    if (info.platFromType == Larkxr_Platform_HTC_FOCUS_PLUS) {
        controllerConfig = lark::CONTROLLER_HTC_FOCUS_PLUS;
    } else if (info.platFromType == Larkxr_Platform_HTC_FOCUS) {
        controllerConfig = lark::CONTROLLER_HTC_FOCULS;
    } else if (info.platFromType == Larkxr_Platform_PICO_NEO) {
        controllerConfig = lark::CONTROLLER_PICO_NEO;
    }

    // controllers
    controller_left_ = std::make_shared<lark::Controller>(true, controllerConfig);
    WvrScene::AddObject(controller_left_);

    controller_right_ = std::make_shared<lark::Controller>(false, controllerConfig);
    WvrScene::AddObject(controller_right_);

    fake_hmd_ = std::make_shared<lark::Object>();
    WvrScene::AddObject(fake_hmd_);

    menu_view_ = std::make_shared<MenuView>(this);
    menu_view_->Move(-0.75, -0.75, -1.8);
    menu_view_->set_active(false);
    WvrScene::AddObject(menu_view_);
    fake_hmd_->AddChild(menu_view_);

    return WvrScene::InitGL(left_eye_queue, right_eye_qeue, left_eye_fbo, right_eye_fbo);
}

bool WvrSceneCloud::ShutdownGL() {
    return WvrScene::ShutdownGL();
}

void WvrSceneCloud::Update() {
    WvrScene::Update();
}

void WvrSceneCloud::UpdateAsync(larkxrTrackingDevicePairFrame *devicePairFrame) {
    devicePairFrame->displayTime = 0;
    devicePairFrame->frameIndex = frame_index_;
    devicePairFrame->fetchTime = utils::GetTimestampUs();
    devicePairFrame->devicePair = device_pair_;
}

bool WvrSceneCloud::Render() {
    frame_index_++;
    WvrScene::Render();
    return true;
}

bool WvrSceneCloud::Render(const larkxrTrackingFrame &trackingFrame,
                           const lark::XRVideoFrame &videoFrame) {
    if (videoFrame.frame_type() == lark::XRVideoFrame::FrameType::kNative_Multiview) {
        rect_texture_->SetMutiviewModeTexture(videoFrame.texture());
    } else if (videoFrame.frame_type() == lark::XRVideoFrame::FrameType::kNative_Stereo) {
        rect_texture_->SetStereoTexture(videoFrame.texture_left(), videoFrame.texture_right());
    } else {
        return false;
    }
    return Render(trackingFrame);
}

bool WvrSceneCloud::Render(const larkxrTrackingFrame &trackingFrame) {
    frame_index_++;

    if (menu_view_->active()) {
        // update hmd;
        UpdateEyeToHeadMatrix(trackingFrame.tracking.is6Dof);
        hmd_pose_ = glm::inverse<4, 4, float, glm::defaultp>(trackingFrame.tracking.rawPoseMatrix);
    }

    // latency
    lark::XRLatencyCollector::Instance().Rendered2(trackingFrame.frameIndex);

    index_left_ = WVR_GetAvailableTextureIndex(left_eye_q_);
    index_right_ = WVR_GetAvailableTextureIndex(right_eye_q_);
    // clear first
    glClearColor(0.30f, 0.30f, 0.37f, 1.0f); // nice background color, but not black
    // glClearColor( 0.125F, 0.0F, 0.125F, 1.0F );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    WvrFrameBuffer * fbo = NULL;

    // Left Eye
    fbo = left_eye_fbo_->at(index_left_);
    fbo->BindFrameBuffer(false);

    fbo->GlViewportFull();
    fbo->GlScissorFull();
    glClearColor(0.30f, 0.30f, 0.37f, 1.0f); // nice background color, but not black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for(auto it = objects_.begin(); it != objects_.end(); it ++) {
        if (it->get()->active()) {
            it->get()->Draw(lark::Object::EYE_LEFT, projection_left_, eye_pos_left_ * hmd_pose_);
        }
    }
    fbo->UnbindFrameBuffer(false);

    // Right Eye
    fbo = right_eye_fbo_->at(index_right_);
    fbo->BindFrameBuffer(false);

    fbo->GlViewportFull();
    fbo->GlScissorFull();
    glClearColor(0.30f, 0.30f, 0.37f, 1.0f); // nice background color, but not black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for(auto it = objects_.begin(); it != objects_.end(); it ++) {
        if (it->get()->active()) {
            it->get()->Draw(lark::Object::EYE_RIGHT, projection_right_, eye_pos_right_ * hmd_pose_);
        }
    }
    fbo->UnbindFrameBuffer(false);

    // latency
    larkxrTrackedPose hmdPose = device_pair_.hmdPose;
    glm::vec3 renderAng = glm::eulerAngles(trackingFrame.tracking.rotation);
    glm::vec3 trackingAng = glm::eulerAngles(hmdPose.rotation);
    float degree = glm::degrees(renderAng.y - trackingAng.y);
    lark::XRLatencyCollector::Instance().Submit(trackingFrame.frameIndex, degree);

    WVR_PoseState_t poseState = wvr::fromLarkvrTrackedPose(trackingFrame.tracking);

    // Left eye
    WVR_TextureParams_t leftEyeTexture = WVR_GetTexture(left_eye_q_, index_left_);
    WVR_SubmitError e;
    e = WVR_SubmitFrame(WVR_Eye_Left, &leftEyeTexture, &poseState);
    if (e != WVR_SubmitError_None) return true;

    // Right eye
    WVR_TextureParams_t rightEyeTexture = WVR_GetTexture(right_eye_q_, index_right_);
    e = WVR_SubmitFrame(WVR_Eye_Right, &rightEyeTexture, &poseState);
    if (e != WVR_SubmitError_None) return true;

    UpdateTime();
    // Clear
    {
        // We want to make sure the glFinish waits for the entire present to complete, not just the submission
        // of the command. So, we do a clear here right here so the glFinish will wait fully for the swap.
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    return false;
}

bool WvrSceneCloud::HandleInput() {
    // send event.
    larkxrDevicePair devicePair = {};
    bool backButtonDownThisFrame[Input::RayCast_Count] = {false, false};
    bool triggerDownThisFrame[Input::RayCast_Count] = {false, false};
    bool enterButtonDownThisFrame[Input::RayCast_Count] = {false, false};

    // 姿态预测头部 pose.
    WVR_PoseState_t poseState{};
//    WVR_GetPoseState(WVR_DeviceType_HMD, WVR_PoseOriginModel_OriginOnHead, 25, &poseState);
    WVR_GetPoseState(WVR_DeviceType_HMD, WVR_PoseOriginModel_OriginOnGround, 25, &poseState);
    devicePair.hmdPose = wvr::wToLarkHMDTrakedPose(poseState);
//    WVR_GetSyncPose(WVR_PoseOriginModel_OriginOnHead, vr_device_pairs_, WVR_DEVICE_COUNT_LEVEL_1);
    WVR_GetSyncPose(WVR_PoseOriginModel_OriginOnGround, vr_device_pairs_, WVR_DEVICE_COUNT_LEVEL_1);

    glm::mat4 hmdMat{};

    // save ray
    // 0->left, 1->right.
    lark::Ray rays[Input::RayCast_Count] = {};
    // update global input state.
    Input::InputState * inputState = Input::GetInputState();

    for (auto posePair: vr_device_pairs_) {
        if (posePair.type == WVR_DeviceType_HMD) {
            // skip hmd
            hmdMat = wvr::toGlm(posePair.pose.poseMatrix);
            continue;
        }

        bool isLeft = posePair.type == WVR_DeviceType_Controller_Left;

        lark::Controller* controller = isLeft ? controller_left_.get() : controller_right_.get();
        if (!posePair.pose.isValidPose) {
            controller->set_active(false);
            // skip invalid device.
            continue;
        }
        glm::mat4 mat = wvr::toGlm(posePair.pose.poseMatrix);

        lark::Transform transform(mat);
        controller->set_transform(transform);

        lark::Ray *ray = nullptr;
        ray = isLeft ? &rays[0] : &rays[1];
        ray->ori = transform.GetPosition();
        ray->dir = transform.Forward();

        larkxrControllerDeviceState* controllerDeviceState = &devicePair.controllerState[isLeft ? 0 : 1];
        controllerDeviceState->deviceType = isLeft ? Larkxr_Controller_Left : Larkxr_Controller_Right;
        controllerDeviceState->pose = wvr::wToLarkControllerTrackedPose(isLeft, posePair.pose);
        // battery

        // htc focus plus 设置旋转
        larkxrSystemInfo info = lark::XRClient::system_info();
        if (controllerDeviceState->pose.is6Dof) {
            // 3.1.8.0 remove rotate
            // remove rotate
//            controllerDeviceState->rotateDeg = glm::half_pi<float>() / 3.0F;
//            controllerDeviceState->rotateAxis = glm::vec3(1, 0, 0);
        } else if (info.platFromType == Larkxr_Platform_PICO_NEO || info.platFromType == Larkxr_Platform_PICO_G2_4k) {
            controllerDeviceState->pose.velocity = glm::vec3(0,0,0);
            controllerDeviceState->pose.angularVelocity = glm::vec3(0,0,0);
            controllerDeviceState->pose.angularAcceleration = glm::vec3(0,0,0);
            controllerDeviceState->pose.acceleration = glm::vec3(0,0,0);
        }

        int deviceIndex = isLeft ? Input::RayCast_left : Input::RayCast_Right;
        backButtonDownThisFrame[deviceIndex] = WVR_GetInputButtonState(posePair.type, WVR_InputId_Alias1_Menu);
        enterButtonDownThisFrame[deviceIndex] = WVR_GetInputButtonState(posePair.type, WVR_InputId_Alias1_Touchpad);
        triggerDownThisFrame[deviceIndex] = WVR_GetInputButtonState(posePair.type, WVR_InputId_Alias1_Trigger) ||
                WVR_GetInputButtonState(posePair.type, WVR_InputId_Alias1_Digital_Trigger);
        // local operate
        bool backButtonDownLastFrame = back_button_down_last_frame_[deviceIndex];
        back_button_down_last_frame_[deviceIndex] = backButtonDownThisFrame[deviceIndex];

        bool traggerButtonDownLastFrame = trigger_button_down_last_frame_[deviceIndex];
        trigger_button_down_last_frame_[deviceIndex] = triggerDownThisFrame[deviceIndex];

        bool enterButtonDownLastFrame = enter_button_down_last_frame_[deviceIndex];
        enter_button_down_last_frame_[deviceIndex] = enterButtonDownThisFrame[deviceIndex];

        Input::RayCastType rayCastType = isLeft ? Input::RayCast_left : Input::RayCast_Right;
        inputState[rayCastType].backShortPressed = backButtonDownLastFrame && !backButtonDownThisFrame[rayCastType];
        inputState[rayCastType].enterShortPressed = enterButtonDownLastFrame && !enterButtonDownThisFrame[rayCastType];
        inputState[rayCastType].triggerShortPressed = traggerButtonDownLastFrame && !triggerDownThisFrame[rayCastType];
        inputState[rayCastType].backButtonDown = backButtonDownThisFrame[rayCastType];
        inputState[rayCastType].enterButtonDown = enterButtonDownThisFrame[rayCastType];
        inputState[rayCastType].triggerButtonDown = triggerDownThisFrame[rayCastType];

        // back button.
        if (backButtonDownLastFrame && !backButtonDownThisFrame[deviceIndex]) {
            LOGV( "back button short press" );
            if (menu_view_->active()) {
                HideMenu();
            }
        }

        // call after pressup.
        if ( triggerDownThisFrame[deviceIndex] && backButtonDownLastFrame && !backButtonDownThisFrame[deviceIndex] )
        {
            // 显示退出菜单
            ShowMenu();
        }

        // call ater pressup.
        if ( traggerButtonDownLastFrame && !triggerDownThisFrame[deviceIndex] ) {
            LOGV( "trigger button short press" );
        }

        // enter button.
        if (enterButtonDownLastFrame && !enterButtonDownThisFrame[deviceIndex]) {
            LOGV( "enter button short press" );
        }

        // cloud controller input.
        controllerDeviceState->inputState.deviceType = static_cast<larkxrDeviceType>(controllerDeviceState->deviceType);
        controllerDeviceState->inputState.isConnected = posePair.pose.isValidPose;
        if (backButtonDownThisFrame[deviceIndex]) {
            controllerDeviceState->inputState.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Back_Click);
        }
        // 6 dof use trigger as trigger. 3dof use digit trigger as trigger
        if (posePair.pose.is6DoFPose) {
            if (WVR_GetInputButtonState(posePair.type, WVR_InputId_Alias1_Trigger)) {
                controllerDeviceState->inputState.buttons |= LARKXR_BUTTON_FLAG(larkxr_Input_Trigger_Click);
                controllerDeviceState->inputState.triggerValue = 1.0F;
            }
            if (WVR_GetInputButtonState(posePair.type, WVR_InputId_Alias1_Digital_Trigger)) {
                controllerDeviceState->inputState.buttons |= LARKXR_BUTTON_FLAG(larkxr_Input_Grip_Click);
                controllerDeviceState->inputState.buttons |= LARKXR_BUTTON_FLAG(larkxr_Input_Grip_Touch);
                controllerDeviceState->inputState.gripValue = 1.0F;
            } else {
                controllerDeviceState->inputState.gripValue = 0.0;
            }
        } else {
            if (WVR_GetInputButtonState(posePair.type, WVR_InputId_Alias1_Digital_Trigger)) {
                controllerDeviceState->inputState.buttons |= LARKXR_BUTTON_FLAG(larkxr_Input_Trigger_Click);
                controllerDeviceState->inputState.triggerValue = 1.0F;
            }
        }
        if (enterButtonDownThisFrame[deviceIndex]) {
            controllerDeviceState->inputState.buttons |= LARKXR_BUTTON_FLAG(larkxr_Input_Trackpad_Click);
        }
        if (WVR_GetInputTouchState(posePair.type, WVR_InputId_Alias1_Touchpad)) {
            controllerDeviceState->inputState.buttons |= LARKXR_BUTTON_FLAG(larkxr_Input_Trackpad_Touch);
        }

        // touchpad axis
        WVR_Axis touchpadAxis = WVR_GetInputAnalogAxis(posePair.type, WVR_InputId_Alias1_Touchpad);
        controllerDeviceState->inputState.touchPadAxis.x = touchpadAxis.x;
        controllerDeviceState->inputState.touchPadAxis.y = touchpadAxis.y;
    }

    if (menu_view_->active()) {
        devicePair.controllerState[0].inputState.isConnected = false;
        devicePair.controllerState[1].inputState.isConnected = false;
        devicePair.controllerState[0].pose.isConnected = false;
        devicePair.controllerState[1].pose.isConnected = false;
        menu_view_->HandleInput(rays, 2);

        // update input state.
        menu_view_->Update();
    }
    {
        // send pose
        device_pair_ = devicePair;
    }
    return WvrScene::HandleInput();
}

void WvrSceneCloud::OnCloseApp() {
    LOGV("================on close app");
    // TODO 关闭应用
    // 集中管控模式下由教师端控制退出应用。
    if (lark::AppListTask::run_mode() == lark::GetVrClientRunMode::ClientRunMode::CLIENT_RUNMODE_TEACHER) {
        return;
    }
    Application::instance()->CloseAppli();
}

void WvrSceneCloud::OnConnect() {
    if (loading_) {
        loading_->Enter();
    }
}

void WvrSceneCloud::OnMediaReady() {
    LOGV("====================OnMediaReady");
    controller_left_->set_active(false);
    controller_right_->set_active(false);
}

void WvrSceneCloud::OnMediaReady(int nativeTexture) {
    LOGV("====================OnMediaReady %d", nativeTexture);
    controller_left_->set_active(false);
    controller_right_->set_active(false);
    rect_texture_->set_frame_texture_(nativeTexture);
}

void WvrSceneCloud::OnClose() {
    LOGV("====================OnClose");
    menu_view_->set_active(false);
    controller_left_->set_active(true);
    controller_right_->set_active(true);
    rect_texture_->set_frame_texture_(0);
}

void WvrSceneCloud::OnMenuViewSelect(bool submit) {
    LOGV("OnMenuViewSelect %d", submit);
    if (submit) {
        // 用户选择退出
        OnCloseApp();
        HideMenu();
    } else {
        // 用户选择取消
        HideMenu();
    }
}

void WvrSceneCloud::ShowMenu() {
    LOGV("show menu");
    fake_hmd_->set_transform(lark::Transform(device_pair_.hmdPose.rotation,
                                       device_pair_.hmdPose.position));
    menu_view_->set_active(true);
    controller_left_->set_active(true);
    controller_right_->set_active(true);
}

void WvrSceneCloud::HideMenu() {
    LOGV("hide menu");
    menu_view_->set_active(false);
    controller_left_->set_active(false);
    controller_right_->set_active(false);
}
