//
// Created by fcx@pingxingyun.com on 2019/10/25.
//


#include <VrApi.h>
#include <VrApi_Helpers.h>
#include <VrApi_Input.h>
// GLM
#include <glm/ext.hpp>
#include <glm/gtx/intersect.inl>
#include "ovr_utils.h"
#include "ovr_scene_local.h"
#include "log.h"
#include "env_context.h"
#include "utils.h"
#include "application.h"
#include "input.h"

namespace {
    const char * DOT           = "textures/ui/circle.png";
    const char * ACTIVIVE  = "textures/ui/circle_active.png";
}

using namespace lark;

OvrSceneLocal::OvrSceneLocal():
    sky_box_(nullptr),
    controller_left_(nullptr),
    controller_right_(nullptr),
    navigation_(nullptr) {
};

OvrSceneLocal::~OvrSceneLocal() = default;

bool OvrSceneLocal::InitGL(OvrFrameBuffer *frame_buffer, int num_buffers) {
    // test skybox
    // add to scenen;
    sky_box_ = std::make_shared<SkyBox>("textures/skybox_8_2k.jpg");
    OvrScene::AddObject(sky_box_);

    // controllers
    controller_left_ = std::make_shared<Controller>(true, CONTROLLER_OCULUS_QUEST);
    controller_left_->Move(-0.3, 0, -0.3);
    // add to scene;
    OvrScene::AddObject(controller_left_);

    controller_right_ = std::make_shared<Controller>(false, CONTROLLER_OCULUS_QUEST);
    controller_right_->Move(0.3, 0, -0.3);
    // add to scene;
    OvrScene::AddObject(controller_right_);

    // navigation.
    navigation_ = std::make_shared<Navigation>();
    OvrScene::AddObject(navigation_);

    return OvrScene::InitGL(frame_buffer, num_buffers);
}

bool OvrSceneLocal::ShutdownGL() {
    return OvrScene::ShutdownGL();
}

//bool OvrSceneLocal::InitJava(ovrJava *java, bool useMultiView) {
//    return OvrScene::InitJava(java, useMultiView);
//}

bool OvrSceneLocal::Update(ovrMobile *ovr) {
    return OvrScene::Update(ovr);
}

bool OvrSceneLocal::HandleInput(ovrMobile * ovr) {

    bool backButtonDownThisFrame[Input::RayCast_Count] = {false, false};
    bool triggerDownThisFrame[Input::RayCast_Count] = {false, false};
    bool enterButtonDownThisFrame[Input::RayCast_Count] = {false, false};

    // update global input state.
    Input::InputState * inputState = Input::GetInputState();
    // save ray
    // 0->left, 1->right.
    Ray rays[Input::RayCast_Count] = {};
    for (uint32_t i = 0; ; i++)
    {
        ovrInputCapabilityHeader cap;
        ovrResult result = vrapi_EnumerateInputDevices(ovr, i, &cap);
        if (result < 0)
        {
            break;
        }

        if (cap.Type == ovrControllerType_TrackedRemote)
        {
            ovrInputTrackedRemoteCapabilities remoteCaps;
            remoteCaps.Header = cap;
            bool isLeft = false;
            // 更新手柄姿态
            if ( ovrSuccess == vrapi_GetInputDeviceCapabilities(ovr, &remoteCaps.Header)) {
                isLeft = (remoteCaps.ControllerCapabilities & ovrControllerCaps_LeftHand) != 0;

                if (isLeft && !controller_left_->active()) {
                    controller_left_->set_active(true);
                }
                if (!isLeft && !controller_right_->active()) {
                    controller_right_->set_active(true);
                }

                if (cap.DeviceID != ovrDeviceIdType_Invalid) {
                    ovrTracking remoteTracking;
                    if (vrapi_GetInputTrackingState(ovr, cap.DeviceID, display_time(), &remoteTracking ) == ovrSuccess) {
//                        LOGV("get controller pose success. isLeft %d idRight %d", isLeft, isRight);

                        glm::quat rotation = ovr::toGlm(remoteTracking.HeadPose.Pose.Orientation);
                        glm::vec3 position = ovr::toGlm(remoteTracking.HeadPose.Pose.Position);
                        Transform transform(rotation, position);

                        Ray *ray = nullptr;
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
            }

            Input::RayCastType rayCastType = isLeft ? Input::RayCast_left : Input::RayCast_Right;
            // 按钮状态
            ovrInputStateTrackedRemote trackedRemoteState;
            trackedRemoteState.Header.ControllerType = ovrControllerType_TrackedRemote;
            if (ovrSuccess == vrapi_GetCurrentInputState(ovr, cap.DeviceID, &trackedRemoteState.Header)) {
                backButtonDownThisFrame[rayCastType] |= trackedRemoteState.Buttons & ovrButton_B;
                backButtonDownThisFrame[rayCastType] |= trackedRemoteState.Buttons & ovrButton_Y;
                enterButtonDownThisFrame[rayCastType] |= trackedRemoteState.Buttons & ovrButton_A;
                enterButtonDownThisFrame[rayCastType] |= trackedRemoteState.Buttons & ovrButton_X;
                triggerDownThisFrame[rayCastType] |= trackedRemoteState.Buttons & ovrButton_Trigger;
                // update battery info to lark system.
                lark::XRClient::SetControlerBatteryLevel(isLeft, trackedRemoteState.BatteryPercentRemaining);
            }

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
               // OnCloseApp();
            }

            // call ater pressup.
            if (inputState[rayCastType].triggerShortPressed) {
                Input::SetCurrentRayCastType(rayCastType);
            }
        }
    }
    // update ui ray.
    navigation_->HandelInput(rays, 2);

    return OvrScene::HandleInput(ovr);
}

bool OvrSceneLocal::Render(ovrMobile * ovr) {
    return OvrScene::Render(ovr);
}

void OvrSceneLocal::HomePage() {
    LOGV("================HomePage");
    navigation_->SetRouter(Navigation::HOME);
}

void OvrSceneLocal::OnCloseApp() {
    LOGV("================on close app");
    // TODO 关闭应用
    Application::instance()->CloseAppli();
}

void OvrSceneLocal::LoadingPage() {
    LOGV("================LoadingPage");
    navigation_->SetRouter(Navigation::HOME);
}
