//
// Created by Admin on 2021/7/2.
//

#include <nibiru/NVRUtils.h>
#include "texture_rect.h"
#include "../log.h"

TextureRect::TextureRect() {

}

TextureRect::~TextureRect() {

}

void TextureRect::Init() {

    ShaderUnit shaderUnit;
    mUtils.loadShaderUnit(&shaderUnit, "texture_rect_vertex.sh", "texture_rect_frag.sh");
    mProgram = shaderUnit.mProgram;

    muMVPMatrixHandle = glGetUniformLocation(mProgram,"uMVPMatrix");
    maPositionHandle = glGetAttribLocation(mProgram,"aPosition");
    maTexCoorHandle = glGetAttribLocation(mProgram,"aTexCoor");

    float ratio = 3840/2160;
    MatrixState::setProjectFrustum(-ratio, ratio, -1, 1, 1, 100);
    MatrixState::setCamera(0, 0, 0, 0, 0, 1, 0, 1, 0);
    MatrixState::setInitStack();
    MatrixState::translate(0, 0, 1);

    pVertices = vertices_rect;
    pTexCoors = texcoors;
}

void TextureRect::drawSelf(int textureId) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (textureId <= 0) {
        return;
    }

    glUseProgram(mProgram);
    glUniformMatrix4fv(muMVPMatrixHandle, 1, 0, MatrixState::getFinalMatrix());

////    glBindBuffer(GL_ARRAY_BUFFER, verticesArrayBuffer);
//    glVertexAttribPointer(maPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, 0);
//    glEnableVertexAttribArray(maPositionHandle);
//
//    glBindBuffer(GL_ARRAY_BUFFER, texCoorArrayBuffer);
//    glVertexAttribPointer(maTexCoorHandle, 2, GL_FLOAT, GL_FALSE, 0, 0);
//    glEnableVertexAttribArray(maTexCoorHandle);

    glVertexAttribPointer(maPositionHandle, 3, GL_FLOAT, GL_FALSE, 3*4, pVertices);   //将顶点位置数据传送进渲染管线
    glVertexAttribPointer(maTexCoorHandle, 2, GL_FLOAT, GL_FALSE, 2*4, pTexCoors);  //将顶点着色数据传送进渲染管线

    glEnableVertexAttribArray(maPositionHandle);
    glEnableVertexAttribArray(maTexCoorHandle);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);

    LOGE("glError %d",glGetError());
    glDrawArrays(GL_TRIANGLES, 0, 6);

//    glUseProgram(0);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TextureRect::setAssets(JNIEnv *env, jobject &assets) {
    mUtils.setAssetsManger(env, assets);
}
