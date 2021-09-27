//
// Created by fcx@pingixngyun.com on 2019/11/25.
//

#include <VrApi.h>
#include "ovr_utils.h"
#include <VrApi_Input.h>
#include "log.h"
#include <unistd.h>
#include <utils.h>
#include "ovr_scene_cloud.h"
//#include "cloudlarkvr/vr_api.h"
//#include "cloudlarkvr/device.h"
#include "env_context.h"
#include "application.h"
#include "lark_xr/xr_latency_collector.h"
#include "lark_xr/app_list_task.h"

using namespace lark;

OvrSceneCloud::OvrSceneCloud(): OvrScene()
{
}

OvrSceneCloud::~OvrSceneCloud() = default;

bool OvrSceneCloud::InitGL(OvrFrameBuffer *frame_buffer, int num_buffers) {
    sky_box_ =  std::make_shared<SkyBox>("textures/skybox_8_2k.jpg");
    OvrScene::AddObject(sky_box_);

    loading_ = std::make_shared<Loading>(nullptr);
    loading_->Move(View::VIEW_POSITION_Y, View::VIEW_POSITION_X, View::VIEW_POSITION_Z);
    OvrScene::AddObject(loading_);

    rect_texture_ = std::make_shared<RectTexture>();
    OvrScene::AddObject(rect_texture_);
    // rect_texture_->set_active(false);

    // controllers
    controller_left_ = std::make_shared<Controller>(true, CONTROLLER_OCULUS_QUEST);
    controller_left_->Move(-0.3, 0, -0.3);
    // add to scene;
    OvrScene::AddObject(controller_left_);

    controller_right_ = std::make_shared<Controller>(false, CONTROLLER_OCULUS_QUEST);
    controller_right_->Move(0.3, 0, -0.3);
    // add to scene;
    OvrScene::AddObject(controller_right_);

    fake_hmd_ = std::make_shared<lark::Object>();
    OvrScene::AddObject(fake_hmd_);

    menu_view_ = std::make_shared<MenuView>(this);
    menu_view_->Move(-0.75, -0.75, -1.8);
    menu_view_->set_active(false);
    OvrScene::AddObject(menu_view_);
    fake_hmd_->AddChild(menu_view_);

    return OvrScene::InitGL(frame_buffer, num_buffers);
}

//bool OvrSceneCloud::InitJava(ovrJava *java, bool useMultiView) {
//    return OvrScene::InitJava(java, useMultiView);
//}

bool OvrSceneCloud::ShutdownGL() {
    return OvrScene::ShutdownGL();
}

bool OvrSceneCloud::Update(ovrMobile *ovr) {
    return OvrScene::Update(ovr);
}

bool OvrSceneCloud::UpdateAsync(ovrMobile *ovr) {
    // Update predicted first.
    // update frame index.
    if (tracking_frame_index_ == 0) {
        tracking_frame_index_ = frame_index_;
    }
    tracking_frame_index_++;
    const double predictedDisplayTime = vrapi_GetPredictedDisplayTime(ovr, tracking_frame_index_);
    display_time_ = predictedDisplayTime;
    device_pair_frame_ = {
            tracking_frame_index_,
            utils::GetTimestampUs(),
            display_time_,
            GetDevicePair(ovr, display_time_),
    };
    return true;
}

bool OvrSceneCloud::HandleInput(ovrMobile *ovr) {
    OvrScene::HandleInput(ovr);
    HandleInput();

    device_pair_frame_ = {
            frame_index_,
            utils::GetTimestampUs(),
            display_time_,
            GetDevicePair(ovr, display_time_),
    };
    return true;
}

void OvrSceneCloud::HandleInput() {
    bool backButtonDownThisFrame[Input::RayCast_Count] = {false, false};
    bool triggerDownThisFrame[Input::RayCast_Count] = {false, false};
    bool enterButtonDownThisFrame[Input::RayCast_Count] = {false, false};

    // update global input state.
    Input::InputState * inputState = Input::GetInputState();

    for (int i = 0; i < 2; i++) {
        int deviceIndex = i;
        backButtonDownThisFrame[deviceIndex] =
                (device_pair_frame_.devicePair.controllerState[deviceIndex].inputState.buttons & LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Back_Click)) ||
                (device_pair_frame_.devicePair.controllerState[deviceIndex].inputState.buttons & LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_B_Click)) ||
                (device_pair_frame_.devicePair.controllerState[deviceIndex].inputState.buttons & LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Y_Click));
        enterButtonDownThisFrame[deviceIndex] =
                (device_pair_frame_.devicePair.controllerState[deviceIndex].inputState.buttons & LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Joystick_Click)) ||
                (device_pair_frame_.devicePair.controllerState[deviceIndex].inputState.buttons & LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_X_Click)) ||
                (device_pair_frame_.devicePair.controllerState[deviceIndex].inputState.buttons & LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_A_Click));
        triggerDownThisFrame[deviceIndex] =
                device_pair_frame_.devicePair.controllerState[deviceIndex].inputState.buttons & LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Trigger_Click);

        bool backButtonDownLastFrame = back_button_down_last_frame_[deviceIndex];
        back_button_down_last_frame_[deviceIndex] = backButtonDownThisFrame[deviceIndex];

        bool traggerButtonDownLastFrame = trigger_button_down_last_frame_[deviceIndex];
        trigger_button_down_last_frame_[deviceIndex] = triggerDownThisFrame[deviceIndex];

        bool enterButtonDownLastFrame = enter_button_down_last_frame_[deviceIndex];
        enter_button_down_last_frame_[deviceIndex] = enterButtonDownThisFrame[deviceIndex];

        Input::RayCastType rayCastType = (Input::RayCastType)i;
        inputState[rayCastType].backShortPressed = backButtonDownLastFrame && !backButtonDownThisFrame[rayCastType];
        inputState[rayCastType].enterShortPressed = enterButtonDownLastFrame && !enterButtonDownThisFrame[rayCastType];
        inputState[rayCastType].triggerShortPressed = traggerButtonDownLastFrame && !triggerDownThisFrame[rayCastType];
        inputState[rayCastType].backButtonDown = backButtonDownThisFrame[rayCastType];
        inputState[rayCastType].enterButtonDown = enterButtonDownThisFrame[rayCastType];
        inputState[rayCastType].triggerButtonDown = triggerDownThisFrame[rayCastType];

        // short press backbutton
        if (inputState[rayCastType].backShortPressed) {
            if (menu_view_->active()) {
                HideMenu();
            }
        }

        // call after pressup.
        if ( triggerDownThisFrame[deviceIndex] && backButtonDownLastFrame && !backButtonDownThisFrame[deviceIndex] ) {
            LOGV("close app." );
            if (Application::instance()->ui_mode() == Application::ApplicationUIMode_Opengles_3D &&
                lark::AppListTask::run_mode() == GetVrClientRunMode::ClientRunMode::CLIENT_RUNMODE_SELF) {
                // 显示退出菜单
                ShowMenu();
            }
        }

        // call ater pressup.
        if (inputState[rayCastType].triggerShortPressed) {
            LOGV("trigger button short press %d", rayCastType);
            if (menu_view_->active()) {
                Input::SetCurrentRayCastType(rayCastType);
            }
        }

        // enter button.
        if (inputState[rayCastType].enterShortPressed) {
            LOGV("enter button short press %d", rayCastType);
        }
    }

    if (menu_view_->active()) {
        // update input state.
        menu_view_->Update();
    }
}

larkxrDevicePair OvrSceneCloud::GetDevicePair(ovrMobile *ovr, double absTimeInSeconds) {
    // update hmd pose.
    const ovrTracking2 tracking = vrapi_GetPredictedTracking2(ovr, absTimeInSeconds);
    // send event.
    larkxrDevicePair devicePair = {};
    devicePair.hmdPose = ovr::toLarkHMDTrakedPose(tracking);

    // save ray
    // 0->left, 1->right.
    lark::Ray rays[Input::RayCast_Count] = {};

    // update controller pose.
    for (uint32_t i = 0; ; i++) {
        ovrInputCapabilityHeader cap;
        ovrResult result = vrapi_EnumerateInputDevices(ovr, i, &cap);
        if (result < 0) {
            break;
        }

        if (cap.Type == ovrControllerType_TrackedRemote) {
            ovrInputTrackedRemoteCapabilities remoteCaps;
            remoteCaps.Header = cap;
            bool isLeft = false;
            if ( ovrSuccess == vrapi_GetInputDeviceCapabilities(ovr, &remoteCaps.Header)) {
                isLeft = (remoteCaps.ControllerCapabilities & ovrControllerCaps_LeftHand) != 0;
            } else {
                // TODO
                // controller disconnected.
                continue;
            }
            // setup device index.
            int deviceIndex = isLeft ? Input::RayCast_left : Input::RayCast_Right;
            larkxrControllerType xrdevice = isLeft ? Larkxr_Controller_Left : Larkxr_Controller_Right;
            // 更新手柄姿态
            if (cap.DeviceID != ovrDeviceIdType_Invalid) {
                ovrTracking remoteTracking;
                if (vrapi_GetInputTrackingState(ovr, cap.DeviceID, display_time(), &remoteTracking ) == ovrSuccess) {
                    // setup
                    devicePair.controllerState[xrdevice - 1].deviceType = xrdevice;
                    devicePair.controllerState[xrdevice - 1].pose = ovr::toLarkControllerTrackedPose(isLeft, remoteTracking);

                    glm::quat rotation = ovr::toGlm(remoteTracking.HeadPose.Pose.Orientation);
                    glm::vec3 positon = ovr::toGlm(remoteTracking.HeadPose.Pose.Position);
                    Transform transform(rotation, positon);

                    lark::Ray *ray = nullptr;
                    if (isLeft) {
                        controller_left_->set_transform(transform);
                        ray = &rays[0];
                    } else {
                        controller_right_->set_transform(transform);
                        ray = &rays[1];
                    }
                    ray->ori = transform.GetPosition();
                    ray->dir = transform.Forward();
                }
            }

            // 更新手柄按键
            ovrInputStateTrackedRemote trackedRemoteState;
            trackedRemoteState.Header.ControllerType = ovrControllerType_TrackedRemote;
            if (ovrSuccess == vrapi_GetCurrentInputState(ovr, cap.DeviceID, &trackedRemoteState.Header)) {
                // 更新
                devicePair.controllerState[xrdevice - 1].inputState = ovr::toLarkvrInputState(isLeft, trackedRemoteState);
                // update battery info to lark system.
                lark::XRClient::SetControlerBatteryLevel(isLeft, trackedRemoteState.BatteryPercentRemaining);
            }

            // 设置旋转
//            devicePair.controllerState[xrdevice - 1].rotateDeg = glm::half_pi<float>() / 3.0F;
//            devicePair.controllerState[xrdevice - 1].rotateAxis = glm::vec3(1, 0, 0);
        }
    }

    if (menu_view_->active()) {
        devicePair.controllerState[0].inputState.isConnected = false;
        devicePair.controllerState[1].inputState.isConnected = false;
        devicePair.controllerState[0].pose.isConnected = false;
        devicePair.controllerState[1].pose.isConnected = false;
        menu_view_->HandleInput(rays, 2);
    }
    return devicePair;
}

bool OvrSceneCloud::Render(ovrMobile *ovr) {
//    if (!inited()) {
//        // not inited.
//        return false;
//    }
    // render use clcoud
    return OvrScene::Render(ovr);
}

bool OvrSceneCloud::Render(ovrMobile *ovr, const larkxrTrackingFrame &trackingFrame,
                           const XRVideoFrame &videoFrame) {
    if (videoFrame.frame_type() == lark::XRVideoFrame::FrameType::kNative_Multiview) {
        rect_texture_->SetMutiviewModeTexture(videoFrame.texture());
    } else if (videoFrame.frame_type() == lark::XRVideoFrame::FrameType::kNative_Stereo) {
        rect_texture_->SetStereoTexture(videoFrame.texture_left(), videoFrame.texture_right());
    } else {
        return false;
    }
    return Render(ovr, trackingFrame);
}

bool OvrSceneCloud::Render(ovrMobile *ovr, const larkxrTrackingFrame &trackingFrame) {
//    if (!inited()) {
//        // not inited.
//        return false;
//    }
    frame_index_++;
    XRLatencyCollector::Instance().Rendered2(trackingFrame.frameIndex);

    ovrTracking2 tracking = ovr::fromtLarkvrTrackedHMDPose(trackingFrame.tracking);
    const ovrLayerProjection2 worldLayer = RenderFrame(&tracking, ovr);
    const ovrLayerHeader2 * layers[] =
            {
                    &worldLayer.Header
            };

    ovrSubmitFrameDescription2 frameDesc = { 0 };
    frameDesc.Flags = 0;
    frameDesc.SwapInterval = swap_interval();

    frameDesc.FrameIndex = trackingFrame.frameIndex;
    frameDesc.DisplayTime = trackingFrame.displayTime;

    // WARNING test local frameindex.
    //    frameDesc.FrameIndex = tracking_frame_index_;
//    frameDesc.FrameIndex = frame_index_;
//    frameDesc.DisplayTime = display_time_;

    frameDesc.LayerCount = 1;
    frameDesc.Layers = layers;

    // render LatencyCollector.
    larkxrTrackedPose hmdPose = device_pair_frame_.devicePair.hmdPose;
    glm::vec3 renderAng = glm::eulerAngles(trackingFrame.tracking.rotation);
    glm::vec3 trackingAng = glm::eulerAngles(hmdPose.rotation);
    float degree = glm::degrees(renderAng.y - trackingAng.y);
    XRLatencyCollector::Instance().Submit(trackingFrame.frameIndex, degree);

    // Hand over the eye images to the time warp.
    vrapi_SubmitFrame2( ovr, &frameDesc );
    return true;
}

void OvrSceneCloud::OnCloseApp() {
    LOGV("================on close app");
    // 集中管控模式下由教师端控制退出应用。
    if (lark::AppListTask::run_mode() == GetVrClientRunMode::ClientRunMode::CLIENT_RUNMODE_TEACHER) {
        return;
    }
    Application::instance()->CloseAppli();
}

void OvrSceneCloud::OnConnect() {
    if (loading_) {
        loading_->Enter();
    }
}

void OvrSceneCloud::OnMediaReady(int nativeTexture) {
    controller_left_->set_active(false);
    controller_right_->set_active(false);
    sky_box_->set_active(false);
    rect_texture_->SetMutiviewModeTexture(nativeTexture);
}

void OvrSceneCloud::OnMediaReady(int nativeTextureLeft, int nativeTextureRight) {
    controller_left_->set_active(false);
    controller_right_->set_active(false);
    sky_box_->set_active(false);
    rect_texture_->SetStereoTexture(nativeTextureLeft, nativeTextureRight);
}

void OvrSceneCloud::OnMediaReady() {
    controller_left_->set_active(false);
    controller_right_->set_active(false);
    sky_box_->set_active(false);
}

void OvrSceneCloud::OnClose() {
    LOGV("==========OnClose");
    menu_view_->set_active(false);
    controller_left_->set_active(true);
    controller_right_->set_active(true);
    sky_box_->set_active(true);
    tracking_frame_index_ = 0;
    rect_texture_->ClearTexture();
}

void OvrSceneCloud::OnMenuViewSelect(bool submit) {
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

void OvrSceneCloud::ShowMenu() {
    LOGV("show menu");
    fake_hmd_->set_transform(lark::Transform(device_pair_frame_.devicePair.hmdPose.rotation,
                                       device_pair_frame_.devicePair.hmdPose.position));
    menu_view_->set_active(true);
    controller_left_->set_active(true);
    controller_right_->set_active(true);
}

void OvrSceneCloud::HideMenu() {
    LOGV("hide menu menu");
    menu_view_->set_active(false);
    controller_left_->set_active(false);
    controller_right_->set_active(false);
}
