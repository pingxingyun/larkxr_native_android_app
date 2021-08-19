//
// Created by Admin on 2021/6/30.
//

#include <dev_api/gsxr_dev_system.h>
#include <lark_xr/types.h>
#include "gsxr_dev.h"

gsxr_dev_id_t GsxrDev::hmd_dev_id = 0;
gsxr_dev_id_t GsxrDev::l_hand_dev_id = 0;
gsxr_dev_id_t GsxrDev::r_hand_dev_id = 0;

GsxrDev::GsxrDev() {

}

GsxrDev::~GsxrDev() {

}

void GsxrDev::Init() {

    hmd = gsxr_dev_get_hmd_instance();
    hand = gsxr_dev_get_controller_instance();

    dev_hmd = hmd->hmd_ops->common.open();
    dev_hand = hand->controller_ops->common.open();

//    gsxr_dev_get_sys_instance()->sys_ops->set_log_level(dev_hmd,gsxr_log_level_t::GSXR_LOG_LEVEL_ERROR);

    gsxr_callback_fn_hmd =[](gsxr_dev_event_callback_type_t event,void* param1,void* param2,void* param3,void* cookie){
        if (event == gsxr_dev_event_callback_type_t::GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_CONNECTED){
            if(*(int*)cookie == 1){
                memcpy(&hmd_dev_id,param1,sizeof(gsxr_dev_id_t));
                LOGV("接收头[id]: {%d}",hmd_dev_id);
            }
            if(*(int*)cookie == 2){
                std::cout<< "============h15555==========:" << param1 <<std::endl;
                if(*(int*)param2 == 1) {
                    memcpy(&l_hand_dev_id, param1, sizeof(gsxr_dev_id_t));
                    LOGV("接收l_hand[id]: {%d}", l_hand_dev_id);
                }
                if(*(int*)param2 == 2) {
                    memcpy(&r_hand_dev_id, param1, sizeof(gsxr_dev_id_t));
                    LOGV("接收l_hand[id]: {%d}", r_hand_dev_id);
                }
            }
        }
    };

    gsxr_callback_fn_hand = [](gsxr_dev_event_callback_type_t event,void* param1,void* param2,void* param3,void* cookie){
        if (event == gsxr_dev_event_callback_type_t::GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_CONNECTED){
            if(*(int*)cookie == 1){
                memcpy(&hmd_dev_id,param1,sizeof(gsxr_dev_id_t));
                LOGV("接收头[id]: {%d}",hmd_dev_id);
            }
            if(*(int*)cookie == 2){
//                std::cout<< "============h15555==========:" << r_hand_dev_id <<std::endl;
                if(*(int*)param2 == 1) {
                    memcpy(&l_hand_dev_id, param1, sizeof(gsxr_dev_id_t));
                    LOGV("接收l_hand[id]: {%d}", l_hand_dev_id);
                }
                if(*(int*)param2 == 2) {
                    memcpy(&r_hand_dev_id, param1, sizeof(gsxr_dev_id_t));
                    LOGV("接收r_hand[id]: {%d}", r_hand_dev_id);
                }
            }
        }
    };

    hmd->hmd_ops->common.set_event_callback(dev_hmd,gsxr_callback_fn_hmd,hmd_cookie);
    hand->controller_ops->common.set_event_callback(dev_hand,gsxr_callback_fn_hand,hand_cookie);
}

void GsxrDev::GetHmdTracking(gsxr_pose_data_t &hmd_pose_data) {
    hmd->hmd_ops->tracking.get_pose_data(dev_hmd, hmd_dev_id, &hmd_pose_data);
}

void GsxrDev::GetControllerTracking(gsxr_pose_data_t &controller_pose_data,bool isLeft){
    if (isLeft) {
        hand->controller_ops->tracking.get_pose_data(dev_hand, l_hand_dev_id, &controller_pose_data);
    } else {
        hand->controller_ops->tracking.get_pose_data(dev_hand, r_hand_dev_id, &controller_pose_data);
    }
}

void GsxrDev::GetRightControllerState( const gsxr_pose_data_t& gsxr_pose_hmd, larkxrControllerDeviceState &deviceState) {
    deviceState.deviceType = Larkxr_Controller_Right;

    //姿态
    gsxr_pose_data_t gsxr_poseData;
    GetControllerTracking(gsxr_poseData, false);

    glm::quat orientation_hmd;
    orientation_hmd.x = gsxr_pose_hmd.pose.orientation.x;
    orientation_hmd.y = gsxr_pose_hmd.pose.orientation.y;
    orientation_hmd.z = gsxr_pose_hmd.pose.orientation.z;
    orientation_hmd.w = gsxr_pose_hmd.pose.orientation.w;
    orientation_hmd = glm::normalize(orientation_hmd);
    orientation_hmd = glm::inverse(orientation_hmd);

    glm::vec3 position_hmd ;

    position_hmd.x = gsxr_pose_hmd.pose.position.x;
    position_hmd.y = gsxr_pose_hmd.pose.position.y;
    position_hmd.z = gsxr_pose_hmd.pose.position.z;

    glm::vec3 position_controller;
    position_controller.x = gsxr_poseData.pose.position.x;
    position_controller.y = gsxr_poseData.pose.position.y;
    position_controller.z = gsxr_poseData.pose.position.z;

    glm::quat orientation_controller;
    orientation_controller.x = gsxr_poseData.pose.orientation.x;
    orientation_controller.y = gsxr_poseData.pose.orientation.y;
    orientation_controller.z = gsxr_poseData.pose.orientation.z;
    orientation_controller.w = gsxr_poseData.pose.orientation.w;

    orientation_controller = glm::normalize(orientation_controller);


//    LOGV("controller_orientation x:%f y:%f z:%f w:%f",orientation_controller.x,orientation_controller.y,orientation_controller.z,orientation_controller.w);

    auto position_controller_result = position_hmd + (orientation_hmd * position_controller);
    auto orientation_controller_result = orientation_hmd * orientation_controller;

    orientation_controller_result = glm::normalize(orientation_controller_result);


    larkxrTrackedPose larkxrTrackedPoseController = {};
    larkxrTrackedPoseController.device = Larkxr_Device_Type_Controller_Right;
    larkxrTrackedPoseController.isConnected=true;
    larkxrTrackedPoseController.is6Dof = true;
    larkxrTrackedPoseController.isValidPose = true;


//    larkxrTrackedPoseController.rotation.x = gsxr_poseData.pose.orientation.x;
//    larkxrTrackedPoseController.rotation.y = gsxr_poseData.pose.orientation.y;
//    larkxrTrackedPoseController.rotation.z = gsxr_poseData.pose.orientation.z;
//    larkxrTrackedPoseController.rotation.w = gsxr_poseData.pose.orientation.w;

    larkxrTrackedPoseController.rotation.x = -orientation_controller_result.x;
    larkxrTrackedPoseController.rotation.y = -orientation_controller_result.y;
    larkxrTrackedPoseController.rotation.z = orientation_controller_result.z;
    larkxrTrackedPoseController.rotation.w = orientation_controller_result.w;

//    larkxrTrackedPoseController.position.x = gsxr_poseData.pose.position.x + position_hmd.x;
//    larkxrTrackedPoseController.position.y = gsxr_poseData.pose.position.y + position_hmd.x;
//    larkxrTrackedPoseController.position.z = gsxr_poseData.pose.position.z + position_hmd.x;

    larkxrTrackedPoseController.position.x = position_controller_result.x;
    larkxrTrackedPoseController.position.y = position_controller_result.y;
    larkxrTrackedPoseController.position.z = -position_controller_result.z;

    //LOGV("controller pose right x->%f y->%f z->%f",gsxr_poseData.pose.position.x, gsxr_poseData.pose.position.y, gsxr_poseData.pose.position.z);

    larkxrTrackedPoseController.timestamp = gsxr_poseData.timestamp;
    larkxrTrackedPoseController.poseFetchTime = gsxr_poseData.timestamp;
    larkxrTrackedPoseController.expectedDisplayTime = gsxr_poseData.timestamp + 5;

    deviceState.pose = larkxrTrackedPoseController;

    // 设置旋转
    deviceState.rotateDeg = -glm::half_pi<float>() / 3.0F;
    deviceState.rotateAxis = glm::vec3(1, 0, 0);

    //按键
    deviceState.inputState.buttons = 0;
    bool click = false;
    hand->controller_ops->input.get_input_click_state(dev_hand, r_hand_dev_id,
                                                      GSXR_INPUT_ID_ALIAS1_A, &click);
    if (click) {
        deviceState.inputState.buttons |= LARKXR_BUTTON_FLAG(
                larkxrInput::larkxr_Input_A_Click);
    }

    hand->controller_ops->input.get_input_click_state(dev_hand, r_hand_dev_id,
                                                      GSXR_INPUT_ID_ALIAS1_B, &click);
    if (click) {
        deviceState.inputState.buttons |= LARKXR_BUTTON_FLAG(
                larkxrInput::larkxr_Input_B_Click);
    }

    hand->controller_ops->input.get_input_click_state(dev_hand, r_hand_dev_id,
                                                      GSXR_INPUT_ID_ALIAS1_TRIGGER, &click);
    if (click) {
        deviceState.inputState.buttons |= LARKXR_BUTTON_FLAG(
                larkxrInput::larkxr_Input_Trigger_Click);
    }

    hand->controller_ops->input.get_input_click_state(dev_hand, r_hand_dev_id,
                                                      GSXR_INPUT_ID_ALIAS1_GRIP, &click);
    if (click) {
        deviceState.inputState.buttons |= LARKXR_BUTTON_FLAG(
                larkxrInput::larkxr_Input_Grip_Click);
    }

    hand->controller_ops->input.get_input_click_state(dev_hand, r_hand_dev_id,
                                                      GSXR_INPUT_ID_ALIAS1_SYSTEM, &click);
    if (click) {
        deviceState.inputState.buttons |= LARKXR_BUTTON_FLAG(
                larkxrInput::larkxr_Input_System_Click);
    }

    gsxr_vector2f_t vector2F = {};
    hand->controller_ops->input.get_input_analog_state(dev_hand, r_hand_dev_id,
                                                       GSXR_INPUT_ID_ALIAS1_THUMBSTICK, &vector2F);

//    LOGV("touchPadAxis x->%f y->%f", vector2F.x, vector2F.y);

    deviceState.inputState.touchPadAxis.x = vector2F.x;
    deviceState.inputState.touchPadAxis.y = vector2F.y;

    hand->controller_ops->input.get_input_analog_state(dev_hand, r_hand_dev_id,
                                                       GSXR_INPUT_ID_ALIAS1_TRIGGER, &vector2F);
    deviceState.inputState.triggerValue = vector2F.x;
}
void GsxrDev::GetLeftControllerState(const gsxr_pose_data_t& gsxr_pose_hmd, larkxrControllerDeviceState &deviceState) {
    deviceState.deviceType = Larkxr_Controller_Left;

    //姿态
    gsxr_pose_data_t gsxr_poseData;
    GetControllerTracking(gsxr_poseData, true);

    glm::quat orientation_hmd;
    orientation_hmd.x = gsxr_pose_hmd.pose.orientation.x;
    orientation_hmd.y = gsxr_pose_hmd.pose.orientation.y;
    orientation_hmd.z = gsxr_pose_hmd.pose.orientation.z;
    orientation_hmd.w = gsxr_pose_hmd.pose.orientation.w;
    orientation_hmd = glm::normalize(orientation_hmd);
    orientation_hmd = glm::inverse(orientation_hmd);

    glm::vec3 position_hmd ;

    position_hmd.x = gsxr_pose_hmd.pose.position.x;
    position_hmd.y = gsxr_pose_hmd.pose.position.y;
    position_hmd.z = gsxr_pose_hmd.pose.position.z;

    glm::vec3 position_controller;
    position_controller.x = gsxr_poseData.pose.position.x;
    position_controller.y = gsxr_poseData.pose.position.y;
    position_controller.z = gsxr_poseData.pose.position.z;

    glm::quat orientation_controller;
    orientation_controller.x = gsxr_poseData.pose.orientation.x;
    orientation_controller.y = gsxr_poseData.pose.orientation.y;
    orientation_controller.z = gsxr_poseData.pose.orientation.z;
    orientation_controller.w = gsxr_poseData.pose.orientation.w;

    orientation_controller = glm::normalize(orientation_controller);

    auto position_controller_result = position_hmd + (orientation_hmd * position_controller);
    auto orientation_controller_result = orientation_hmd * orientation_controller;

    orientation_controller_result = glm::normalize(orientation_controller_result);

    larkxrTrackedPose larkxrTrackedPoseController = {};
    larkxrTrackedPoseController.device = Larkxr_Device_Type_Controller_Left;
    larkxrTrackedPoseController.isConnected=true;
    larkxrTrackedPoseController.is6Dof = true;
    larkxrTrackedPoseController.isValidPose = true;

//    larkxrTrackedPoseController.rotation.x = gsxr_poseData.pose.orientation.x;
//    larkxrTrackedPoseController.rotation.y = gsxr_poseData.pose.orientation.y;
//    larkxrTrackedPoseController.rotation.z = gsxr_poseData.pose.orientation.z;
//    larkxrTrackedPoseController.rotation.w = gsxr_poseData.pose.orientation.w;

    larkxrTrackedPoseController.rotation.x = -orientation_controller_result.x;
    larkxrTrackedPoseController.rotation.y = -orientation_controller_result.y;
    larkxrTrackedPoseController.rotation.z = orientation_controller_result.z;
    larkxrTrackedPoseController.rotation.w = orientation_controller_result.w;

//    larkxrTrackedPoseController.position.x = gsxr_poseData.pose.position.x;
//    larkxrTrackedPoseController.position.y = gsxr_poseData.pose.position.y;
//    larkxrTrackedPoseController.position.z = gsxr_poseData.pose.position.z;

    larkxrTrackedPoseController.position.x = position_controller_result.x;
    larkxrTrackedPoseController.position.y = position_controller_result.y;
    larkxrTrackedPoseController.position.z = -position_controller_result.z;

    larkxrTrackedPoseController.timestamp = gsxr_poseData.timestamp;
    larkxrTrackedPoseController.poseFetchTime = gsxr_poseData.timestamp;
    larkxrTrackedPoseController.expectedDisplayTime = gsxr_poseData.timestamp + 5;

    deviceState.pose = larkxrTrackedPoseController;

    // 设置旋转
    deviceState.rotateDeg = -glm::half_pi<float>() / 3.0F;
    deviceState.rotateAxis = glm::vec3(1, 0, 0);

    //按键
    deviceState.inputState.buttons = 0;
    bool click = false;
    hand->controller_ops->input.get_input_click_state(dev_hand, l_hand_dev_id,
                                                      GSXR_INPUT_ID_ALIAS1_X, &click);
    if (click) {
        deviceState.inputState.buttons |= LARKXR_BUTTON_FLAG(
                larkxrInput::larkxr_Input_X_Click);
    }

    hand->controller_ops->input.get_input_click_state(dev_hand, l_hand_dev_id,
                                                      GSXR_INPUT_ID_ALIAS1_Y, &click);
    if (click) {
        deviceState.inputState.buttons |= LARKXR_BUTTON_FLAG(
                larkxrInput::larkxr_Input_Y_Click);
    }

    hand->controller_ops->input.get_input_click_state(dev_hand, l_hand_dev_id,
                                                      GSXR_INPUT_ID_ALIAS1_TRIGGER, &click);
    if (click) {
        deviceState.inputState.buttons |= LARKXR_BUTTON_FLAG(
                larkxrInput::larkxr_Input_Trigger_Click);
    }

    hand->controller_ops->input.get_input_click_state(dev_hand, l_hand_dev_id,
                                                      GSXR_INPUT_ID_ALIAS1_GRIP, &click);
    if (click) {
        deviceState.inputState.buttons |= LARKXR_BUTTON_FLAG(
                larkxrInput::larkxr_Input_Grip_Click);
    }

    hand->controller_ops->input.get_input_click_state(dev_hand, l_hand_dev_id,
                                                      GSXR_INPUT_ID_ALIAS1_MENU, &click);
    if (click) {
        deviceState.inputState.buttons |= LARKXR_BUTTON_FLAG(
                larkxrInput::larkxr_Input_Application_Menu_Click);
    }

    gsxr_vector2f_t vector2F = {};
    hand->controller_ops->input.get_input_analog_state(dev_hand, l_hand_dev_id,
                                                       GSXR_INPUT_ID_ALIAS1_THUMBSTICK, &vector2F);
    deviceState.inputState.touchPadAxis.x = vector2F.x;
    deviceState.inputState.touchPadAxis.y = vector2F.y;

    hand->controller_ops->input.get_input_analog_state(dev_hand, l_hand_dev_id,
                                                       GSXR_INPUT_ID_ALIAS1_TRIGGER, &vector2F);
    deviceState.inputState.triggerValue = vector2F.x;
}

void GsxrDev::trigger_vibration(bool isLeft, const gsxr_haptic_vibration_t* haptics) {
    gsxr_dev_id_t hand_dev_id = isLeft ? l_hand_dev_id : r_hand_dev_id;
    hand->controller_ops->trigger_vibration(dev_hand, hand_dev_id, haptics);

}
