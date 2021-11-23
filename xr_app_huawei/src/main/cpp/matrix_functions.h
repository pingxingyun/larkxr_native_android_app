#ifndef MATRIX_FUNCTION_H
#define MATRIX_FUNCTION_H

#include <math.h>
#include "openxr/openxr.h"

typedef struct _XrMatrix4x4f {
    float    m[4][4];
} XrMatrix4x4f;

typedef struct _XrMatrix3x3f {
    float    m[3][3];
} XrMatrix3x3f;

static XrVector3f XrGetXrVector3f(XrMatrix3x3f quat, XrVector3f xrVec)
{
    XrVector3f vec3f;
    vec3f.x = quat.m[0][0] * xrVec.x + quat.m[0][1] * xrVec.y + quat.m[0][2] * xrVec.z;
    vec3f.y = quat.m[1][0] * xrVec.x + quat.m[1][1] * xrVec.y + quat.m[1][2] * xrVec.z;
    vec3f.z = quat.m[2][0] * xrVec.x + quat.m[2][1] * xrVec.y + quat.m[2][2] * xrVec.z;

    return vec3f;
}

static XrMatrix4x4f XrMatrix4x4f_CreateFromQuaternion_INV(XrMatrix4x4f matrix)
{
    XrMatrix4x4f result;
    XrMatrix3x3f quat;
    XrVector3f vec3fTmp;
    XrVector3f vec3f;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result.m[i][j] = matrix.m[j][i];
            quat.m[i][j] = -matrix.m[j][i];
        }
    }

    vec3fTmp.x = matrix.m[0][3];
    vec3fTmp.y = matrix.m[1][3];
    vec3fTmp.z = matrix.m[1][3];

    vec3f = XrGetXrVector3f(quat, vec3fTmp);

    result.m[0][3] = vec3f.x;
    result.m[1][3] = vec3f.y;
    result.m[2][3] = vec3f.z;
    result.m[3][3] = 1.0f;

    result.m[3][0] = 0.0f;
    result.m[3][1] = 0.0f;
    result.m[3][2] = 0.0f;

    return result;

}

static XrMatrix4x4f XrMatrix4x4f_CreateFromQuaternion_4(const XrQuaternionf* quat, const XrVector3f* xrvec)
{
    const float x2 = quat->x + quat->x;
    const float y2 = quat->y + quat->y;
    const float z2 = quat->z + quat->z;

    const float xx2 = quat->x * x2;
    const float yy2 = quat->y * y2;
    const float zz2 = quat->z * z2;

    const float yz2 = quat->y * z2;
    const float wx2 = quat->w * x2;
    const float xy2 = quat->x * y2;
    const float wz2 = quat->w * z2;
    const float xz2 = quat->x * z2;
    const float wy2 = quat->w * y2;

    XrMatrix4x4f result;
    XrMatrix4x4f resultX;

    result.m[0][0] = 1.0f - yy2 - zz2;
    result.m[0][1] = xy2 + wz2;
    result.m[0][2] = xz2 - wy2;
    result.m[0][3] = 0.0f;

    result.m[1][0] = xy2 - wz2;
    result.m[1][1] = 1.0f - xx2 - zz2;
    result.m[1][2] = yz2 + wx2;
    result.m[1][3] = 0.0f;

    result.m[2][0] = xz2 + wy2;
    result.m[2][1] = yz2 - wx2;
    result.m[2][2] = 1.0f - xx2 - yy2;
    result.m[2][3] = 0.0f;

    result.m[3][0] = xrvec->x;
    result.m[3][1] = xrvec->y;
    result.m[3][2] = xrvec->z;
    result.m[3][3] = 1.0f;

    resultX = XrMatrix4x4f_CreateFromQuaternion_INV(result);

    return resultX;
}

static XrMatrix4x4f XrMatrix4x4f_CreateFromQuaternion(const XrQuaternionf* quat )
{
    const float x2 = quat->x + quat->x;
    const float y2 = quat->y + quat->y;
    const float z2 = quat->z + quat->z;

    const float xx2 = quat->x * x2;
    const float yy2 = quat->y * y2;
    const float zz2 = quat->z * z2;

    const float yz2 = quat->y * z2;
    const float wx2 = quat->w * x2;
    const float xy2 = quat->x * y2;
    const float wz2 = quat->w * z2;
    const float xz2 = quat->x * z2;
    const float wy2 = quat->w * y2;

    XrMatrix4x4f result;

    result.m[0][0] = 1.0f - yy2 - zz2;
    result.m[0][1] = xy2 + wz2;
    result.m[0][2] = xz2 - wy2;
    result.m[0][3] = 0.0f;

    result.m[1][0] = xy2 - wz2;
    result.m[1][1] = 1.0f - xx2 - zz2;
    result.m[1][2] = yz2 + wx2;
    result.m[1][3] = 0.0f;

    result.m[2][0] = xz2 + wy2;
    result.m[2][1] = yz2 - wx2;
    result.m[2][2] = 1.0f - xx2 - yy2;
    result.m[2][3] = 0.0f;

    result.m[3][0] = 0.0f;
    result.m[3][1] = 0.0f;
    result.m[3][2] = 0.0f;
    result.m[3][3] = 1.0f;

    return result;
}

static XrMatrix4x4f XrMatrix4x4f_CreateProjection(const float tanAngleLeft, const float tanAngleRight, const float tanAngleUp, float const tanAngleDown, const float nearZ, const float farZ )
{
    const float tanAngleWidth = tanAngleRight - tanAngleLeft;

    // Set to tanAngleUp - tanAngleDown for a clip space with positive Y up (OpenGL / D3D / Metal).
    const float tanAngleHeight = tanAngleUp - tanAngleDown;

    // Set to nearZ for a [-1,1] Z clip space (OpenGL / OpenGL ES).
    const float offsetZ = nearZ;

    XrMatrix4x4f result;

    if ( farZ <= nearZ )
    {
        // place the far plane at infinity
        result.m[0][0] = 2 / tanAngleWidth;
        result.m[1][0] = 0;
        result.m[2][0] = ( tanAngleRight + tanAngleLeft ) / tanAngleWidth;
        result.m[3][0] = 0;

        result.m[0][1] = 0;
        result.m[1][1] = 2 / tanAngleHeight;
        result.m[2][1] = ( tanAngleUp + tanAngleDown ) / tanAngleHeight;
        result.m[3][1] = 0;

        result.m[0][2] = 0;
        result.m[1][2] = 0;
        result.m[2][2] = -1;
        result.m[3][2] = -( nearZ + offsetZ );

        result.m[0][3] = 0;
        result.m[1][3] = 0;
        result.m[2][3] = -1;
        result.m[3][3] = 0;
    }
    else
    {
        // normal projection
        result.m[0][0] = 2 / tanAngleWidth;
        result.m[1][0] = 0;
        result.m[2][0] = ( tanAngleRight + tanAngleLeft ) / tanAngleWidth;
        result.m[3][0] = 0;

        result.m[0][1] = 0;
        result.m[1][1] = 2 / tanAngleHeight;
        result.m[2][1] = ( tanAngleUp + tanAngleDown ) / tanAngleHeight;
        result.m[3][1] = 0;

        result.m[0][2] = 0;
        result.m[1][2] = 0;
        result.m[2][2] = -( farZ + offsetZ ) / ( farZ - nearZ );
        result.m[3][2] = -( farZ * ( nearZ + offsetZ ) ) / ( farZ - nearZ );

        result.m[0][3] = 0;
        result.m[1][3] = 0;
        result.m[2][3] = -1;
        result.m[3][3] = 0;
    }
    return result;
}


static XrMatrix4x4f  XrMatrix4x4f_CreateOffset()
{
    XrMatrix4x4f result;
    result.m[0][0] = 1.0f;
    result.m[0][1] = 0.0f;
    result.m[0][2] = 0.0f;
    result.m[0][3] = 0.0f;

    result.m[1][0] = 0.0f;
    result.m[1][1] = 1.0f;
    result.m[1][2] = 0.0f;
    result.m[1][3] = 0.0f;

    result.m[2][0] = 0.0f;
    result.m[2][1] = 0.0f;
    result.m[2][2] = 1.0f;
    result.m[2][3] = 0.0f;

    result.m[3][0] = 0.8f;
    result.m[3][1] = -0.8f;
    result.m[3][2] = -1.2f;
    result.m[3][3] = 1.0f;
    return result;
}

#endif