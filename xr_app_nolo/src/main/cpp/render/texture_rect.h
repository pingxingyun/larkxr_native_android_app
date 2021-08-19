//
// Created by Admin on 2021/7/2.
//

#ifndef NOLOLARKXR_TEXTURE_RECT_H
#define NOLOLARKXR_TEXTURE_RECT_H
#include <GLES3/gl3.h>
#include <string>
#include "MatrixState.h"
#include "nibiru/NVRUtils.h"

class TextureRect {
public:
    TextureRect();
    ~TextureRect();
    void Init();

    void drawSelf(int textureId);
    void setAssets(JNIEnv *env, jobject &assets);
private:
    int mProgram;
    int muMVPMatrixHandle;
    int maPositionHandle;
    int maTexCoorHandle;

    NVRUtils mUtils;
    const GLvoid * pVertices;
    const GLvoid * pTexCoors;
};

const float vertices_rect[] =//顶点坐标数据
        {
                -1, 1, 0,
                -1, -1, 0,
                1, -1, 0,

                1, -1, 0,
                1, 1, 0,
                -1, 1, 0
        };


const float texcoors[] = //顶点着色数据
        {
//                0, 0,
//                0, 1,
//                1, 1,
//                1, 1,
//                1, 0,
//                0, 0

                1,0,
                1,1,
                0,1,
                0,1,
                0,0,
                1,0
        };

#endif //NOLOLARKXR_TEXTURE_RECT_H
