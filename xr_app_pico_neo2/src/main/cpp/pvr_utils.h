//
// Created by fcx on 2020/7/2.
//

#ifndef CLOUDLARKXR_PVR_UTILS_H
#define CLOUDLARKXR_PVR_UTILS_H

#include <glm/detail/type_quat.hpp>
#include <lark_xr/types.h>
#include <log.h>
#include "glm/glm.hpp"
#include "env_context.h"

namespace pvr {
    struct PvrPose {
        glm::quat rotation;
        glm::vec3 position;
        glm::vec3 acc;
        glm::vec3 agl;
        long poseTimeStampNs;
        long poseFetchTimeNs;
        long expectedDisplayTimeNs;
    };
    enum PvrControllerType {
        PvrConTroller_Left = 0,
        PvrConTroller_Right = 1,
        PvrController_Cont = 2,
    };
    struct PvrControllerInput {
        PvrControllerType type;
        int battery;
        bool isConnected;
        bool is6dof;
        bool home;
        bool app;
        bool joyStick;
        bool trigger;
        bool grip;
        bool buttonX;
        bool buttonA;
        bool buttonB;
        bool buttonY;
        int triggerNumber;
        /// d            ! x = 255
        /// d            |
        /// d            |
        /// d  0 --------|--------> x 255
        /// d            |
        /// d            | 0
        /// d        openvr
        glm::vec2 joyStickAxis;
    };
    struct PvrControllerState {
        PvrControllerType type;
        PvrPose pose;
        PvrControllerInput input;
    };
    enum PvrTrackingOrigin {
        PvrTrackingOrigin_EyeLevel = 0,
        PvrTrackingOrigin_FloorLevel = 1,
        PvrTrackingOrigin_StageLevel = 2,
    };

    inline larkxrTrackedPose toLarkTrackedPose(const PvrPose& tracking) {
        larkxrTrackedPose pose = {};
        pose.isConnected = true;
        pose.is6Dof = true;
        pose.isValidPose = true;
        pose.rotation = tracking.rotation;
        pose.position = tracking.position;
        pose.acceleration = tracking.acc;
        pose.angularVelocity = tracking.agl;
        // timestamp
        pose.timestamp = tracking.poseTimeStampNs;
        pose.poseFetchTime = tracking.poseFetchTimeNs;
        pose.expectedDisplayTime = tracking.expectedDisplayTimeNs;
        return pose;
    }
    inline larkxrTrackedPose toLarkHMDTrakedPose(const PvrPose& tracking) {
        larkxrTrackedPose pose = toLarkTrackedPose(tracking);
        pose.device = Larkxr_Device_Type_HMD;
        return pose;
    }
    // to larrk controller pose
    inline larkxrTrackedPose toLarkControllerTrackedPose(bool isLeft, const PvrPose& tracking) {
        larkxrTrackedPose pose = toLarkTrackedPose(tracking);
        pose.device = isLeft ? Larkxr_Device_Type_Controller_Left : Larkxr_Device_Type_Controller_Right;
        return pose;
    }

    inline larkxrControllerInputState toLarkvrInputState(bool isLeft, const PvrControllerInput& input) {
        larkxrControllerInputState state = {};
        state.isConnected = input.isConnected;
        state.deviceType = isLeft ? Larkxr_Device_Type_Controller_Left : Larkxr_Device_Type_Controller_Right;

        if (input.app) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Application_Menu_Click);
        }
        if (input.buttonA) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_A_Click);
        }
        if (input.buttonB) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_B_Click);
        }
        if (input.buttonX) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_X_Click);
        }
        if (input.buttonY) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Y_Click);
        }
        if (input.trigger) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Trigger_Click);
            state.triggerValue = 1.0F;
        }
        if (input.grip) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Grip_Click);
            state.gripValue = 1.0F;
        }

        state.touchPadAxis = input.joyStickAxis;

        if (abs(state.touchPadAxis.x) > 0.1 || abs(state.touchPadAxis.y) > 0.1) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Joystick_Touch);
        }

        if (input.joyStick) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Joystick_Click);
        }

        state.batteryPercentRemaining = static_cast<uint8_t>(input.battery);
        return state;
    }

    inline larkxrControllerDeviceState toLarkxrControllerState(const PvrControllerState& pvrControllerState) {
        bool isLeft = pvrControllerState.type == PvrConTroller_Left;
        larkxrControllerDeviceState state = {
                isLeft ? Larkxr_Controller_Left : Larkxr_Controller_Right,
                toLarkControllerTrackedPose(isLeft, pvrControllerState.pose),
                toLarkvrInputState(isLeft, pvrControllerState.input)
        };
        // TODO pose connected setup.
        state.pose.isConnected = state.inputState.isConnected;
        return state;
    }

    inline PvrPose fromLarkTrackedPose(const larkxrTrackedPose& trackedPose) {
        PvrPose pose = {};
        pose.position = trackedPose.position;
        pose.rotation = trackedPose.rotation;
        pose.poseTimeStampNs = trackedPose.timestamp;
        pose.poseFetchTimeNs = trackedPose.poseFetchTime;
        pose.expectedDisplayTimeNs = trackedPose.expectedDisplayTime;
        return pose;
    }
    // java api
    inline bool getHmdPose(jobject pvrSdkObj, PvrPose& nativePose) {
        if (pvrSdkObj == nullptr) {
            return false;
        }
        EnvWrapper envWrapper = Context::instance()->GetEnv();
        JNIEnv* env = envWrapper.get();
        if (env == nullptr) {
            return false;
        }
        jclass clazz = env->GetObjectClass(pvrSdkObj);
        jmethodID mid_getPredicateDisplayTime = env->GetStaticMethodID(clazz, "getPredictedDisplayTime", "()F");
        jmethodID mid_getPredicatedHeadPoseState = env->GetStaticMethodID(clazz, "getPredictedHeadPoseState", "(F[F[J)I");
        jfloat predicateTime = env->CallStaticFloatMethod(clazz, mid_getPredicateDisplayTime);
        jfloatArray poseState = env->NewFloatArray(7);
        jlongArray timeStamp = env->NewLongArray(3);
        int state = env->CallStaticIntMethod(clazz, mid_getPredicatedHeadPoseState, predicateTime, poseState, timeStamp);

        jfloat* array_pose = env->GetFloatArrayElements(poseState, JNI_FALSE);

        nativePose.rotation.x = array_pose[0];
        nativePose.rotation.y = array_pose[1];
        nativePose.rotation.z = array_pose[2];
        nativePose.rotation.w = array_pose[3];

        nativePose.position.x = array_pose[4];
        nativePose.position.y = array_pose[5];
        nativePose.position.z = array_pose[6];

        jlong* array_timestamp = env->GetLongArrayElements(timeStamp, JNI_FALSE);
        nativePose.poseTimeStampNs = array_timestamp[0];
        nativePose.poseFetchTimeNs = array_timestamp[1];
        nativePose.expectedDisplayTimeNs = array_timestamp[2];

        env->ReleaseFloatArrayElements(poseState, array_pose, 0);
        env->ReleaseLongArrayElements(timeStamp, array_timestamp, 0);

        env->DeleteLocalRef(timeStamp);
        env->DeleteLocalRef(poseState);
        env->DeleteLocalRef(clazz);

        return true;
    }
    inline bool setTrackingOriginType(jobject pvrSdkObj, PvrTrackingOrigin trackingOrigin) {
        if (pvrSdkObj == nullptr) {
            return false;
        }
        EnvWrapper envWrapper = Context::instance()->GetEnv();
        JNIEnv* env = envWrapper.get();
        if (env == nullptr) {
            return false;
        }
        jclass clazz = env->GetObjectClass(pvrSdkObj);
//        jclass clazz = env->FindClass("com/psmart/vrlib/PicovrSDK");
        jmethodID mid_setTrackingOriginType = env->GetStaticMethodID(clazz, "setTrackingOriginType", "(I)Z");
        bool res = env->CallStaticBooleanMethod(clazz, mid_setTrackingOriginType, (int)trackingOrigin);
        env->DeleteLocalRef(clazz);
        return res;
    }
}

#endif //CLOUDLARKXR_PVR_UTILS_H
