//
// Created by Admin on 2021/7/7.
//

#ifndef NOLOLARKXR_NOLO_VR_UTILS_H
#define NOLOLARKXR_NOLO_VR_UTILS_H


#include <dev_api/gsxr_dev_types.h>
#include "nibiru/matrix.h"
#include "glm/glm.hpp"
#include "log.h"

namespace nolo {
inline glm::mat4 toGlm(const mat4& m) {
    return glm::mat4(
            m.x[0], m.y[0], m.z[0], m.w[0],
            m.x[1], m.y[1], m.z[1], m.w[1],
            m.x[2], m.y[2], m.z[2], m.w[2],
            m.x[3], m.y[3], m.z[3], m.w[3]
    );
}
inline larkxrTrackedPose toLark(larkxrDeviceType deviceType, const gsxr_pose_data_t& gsxr_poseData) {
    //Nibiru SDK获取rotation
//    mat4 nbrMatRotation;
//    nibiruVrApi->nvr_getPredictiveHeadPose(&nbrMatRotation, 16);
//
//    matrixToQuat(nbrMatRotation, larkxrTrackedPoseHmd.rotation);

//    LOGV("nibiru_rotation x->%f y->%f z->%f w->%f", larkxrTrackedPoseHmd.rotation.x,
//         larkxrTrackedPoseHmd.rotation.y,
//         larkxrTrackedPoseHmd.rotation.z, larkxrTrackedPoseHmd.rotation.w);
//
//    LOGV("larkxr_rotation x->%f y->%f z->%f w->%f", gsxr_poseData.pose.orientation.x,
//         gsxr_poseData.pose.orientation.y,
//         -gsxr_poseData.pose.orientation.z, gsxr_poseData.pose.orientation.w);

    larkxrTrackedPose larkxrTrackedPoseHmd = {};
    larkxrTrackedPoseHmd.device = deviceType;
    larkxrTrackedPoseHmd.isConnected = true;
    larkxrTrackedPoseHmd.is6Dof = true;
    larkxrTrackedPoseHmd.isValidPose = true;

    larkxrTrackedPoseHmd.position.x = gsxr_poseData.pose.position.x;
    larkxrTrackedPoseHmd.position.y = gsxr_poseData.pose.position.y;
    larkxrTrackedPoseHmd.position.z = -gsxr_poseData.pose.position.z;

    larkxrTrackedPoseHmd.rotation.x = gsxr_poseData.pose.orientation.x;
    larkxrTrackedPoseHmd.rotation.y = gsxr_poseData.pose.orientation.y;
    larkxrTrackedPoseHmd.rotation.z = -gsxr_poseData.pose.orientation.z;
    larkxrTrackedPoseHmd.rotation.w = gsxr_poseData.pose.orientation.w;

    time_t now = time(0);
    larkxrTrackedPoseHmd.timestamp = now;
    larkxrTrackedPoseHmd.poseFetchTime = now;
    larkxrTrackedPoseHmd.expectedDisplayTime = now + 10;
    return  larkxrTrackedPoseHmd;
}
}

void matrixToQuat( const mat4& matrix, glm::quat &rotation){
    float m11 = matrix.x.x;
    float m12 = matrix.x.y;
    float m13 = matrix.x.z;

    float m21 = matrix.y.x;
    float m22 = matrix.y.y;
    float m23 = matrix.y.z;

    float m31 = matrix.z.x;
    float m32 = matrix.z.y;
    float m33 = matrix.z.z;

    float w,x,y,z;
    float fourWSquaredMinus1 = m11 + m22 + m33;
    float fourXSquaredMinus1 = m11 - m22 - m33;
    float fourYSquaredMinus1 = m22 - m11 - m33;
    float fourZSquareMinus1 = m33 - m11 - m22;
    int biggestIndex = 0;
    float fourBiggestSquaredMinus1 = fourWSquaredMinus1;
    if (fourXSquaredMinus1 > fourBiggestSquaredMinus1) {
        fourBiggestSquaredMinus1 = fourXSquaredMinus1;
        biggestIndex = 1;
    }

    if (fourYSquaredMinus1 > fourBiggestSquaredMinus1) {
        fourBiggestSquaredMinus1 = fourYSquaredMinus1;
        biggestIndex = 2;
    }

    if (fourZSquareMinus1 > fourBiggestSquaredMinus1) {
        fourBiggestSquaredMinus1 = fourZSquareMinus1;
        biggestIndex = 3;
    }

    float biggestVal = sqrt(fourBiggestSquaredMinus1 + 1.0f) * 0.5f;
    float mult = 0.25f / biggestVal;

    switch (biggestIndex) {
        case 0:
            w = biggestVal;
            x = (m23 - m32) * mult;
            y = (m31 - m13) * mult;
            z = (m12 - m21) * mult;
            break;
        case 1:
            x = biggestVal;
            w = (m23 - m32) * mult;
            y = (m12 + m21) * mult;
            z = (m31 + m13) + mult;
            break;

        case 2:
            y = biggestVal;
            w = (m31 - m13) * mult;
            x = (m12 + m21) * mult;
            z = (m23 + m32) * mult;
            break;
        case 3:
            z = biggestVal;
            w = (m12 - m21) * mult;
            x = (m31 + m13) * mult;
            y = (m23 + m32) * mult;
            break;
    }
    rotation.x = -x;
    rotation.y = -y;
    rotation.z = -z;
    rotation.w = w;
}


#endif //NOLOLARKXR_NOLO_VR_UTILS_H
