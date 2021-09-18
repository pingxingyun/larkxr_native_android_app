//
// Created by Admin on 2021/7/7.
//

#ifndef NOLOLARKXR_NOLO_VR_UTILS_H
#define NOLOLARKXR_NOLO_VR_UTILS_H

#include "nibiru/matrix.h"
#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>
#include "log.h"
#include "nibiru/NVRLog.h"
#include "lark_xr/types.h"

namespace xvisio {
inline glm::mat4 toGlm(const mat4& m) {
    return glm::mat4(
            m.x[0], m.y[0], m.z[0], m.w[0],
            m.x[1], m.y[1], m.z[1], m.w[1],
            m.x[2], m.y[2], m.z[2], m.w[2],
            m.x[3], m.y[3], m.z[3], m.w[3]
    );
}
inline void matrixToQuat( const mat4& matrix, glm::quat &rotation) {
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


inline larkxrRenderFov PerspectiveOffCenter(int eye, float fx, float fy, float u0, float v0,
                                 float w, float h, float near, float far) {
    float x = 2.0f * fx / w;
    float y = 2.0f * fy / h;
    float a = 1.0f - 2.0f * u0 / w;
    float b = -1.0f + 2.0f * v0 / h;
    float c = -(far + near) / (far - near);
    float d = -(2.0f * far * near) / (far - near);
    float e = -1.0f;

    float n = d;

    NLOGD("getFov x=%f y=%f a=%f b=%f c=%f d=%f e=%f", x, y, a, b, c, d, e);

    double w1 = 2.0F * n / x;
    double h1 = 2.0F * n / y;
    double w2 = a * w1;
    double h2 = b * h1;

    double maxX = (w1 + w2) / 2.0F;
    double minX = w2 - maxX;
    double maxY = (h1 + h2) / 2.0F;
    double minY = h2 - maxY;

    double rr = 180 / M_PI;

    NLOGD("getFov maxX=%f minX=%f maxY=%f minY=%f a=%f b=%f c=%f d=%f n=%f", maxX, minX, maxY,
         minY, a, b, c, d, n);
    float left = atan(minX / -n) * rr; // left (minX)
    float right = -atan(maxX / -n) * rr; // right (maxX)
    float top = atan(minY / -n) * rr; // top (minY)
    float bottom = -atan(maxY / -n) * rr; // bottom (maxY)

    NLOGD("getFov[eye %d](D) l=%f r=%f t=%f b=%f", eye, left, right, top, bottom);

    return {
        left, right, top, bottom
    };
}
}

#endif //NOLOLARKXR_NOLO_VR_UTILS_H
