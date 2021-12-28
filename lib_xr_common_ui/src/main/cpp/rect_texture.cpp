// 
// Created by fcx@pingxingyun.com
// 2020-04-27 19:37
//
#include "rect_texture.h"
#include "env_context.h"
#include "log.h"
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>
#define LOG_TAG "rect_texture"
namespace  {
    float verticesTexture[] = {
            // 位置              // 颜色           - 纹理坐标 -
            -1.0F, -1.0F, -1.0F, 1.0F, 0.0F, 0.0F,  0.0F, 0.0F,  // 左下角
            1.0F, -1.0F, -1.0F,  0.0F, 1.0F, 0.0F,  1.0F, 0.0F,  // 右下角
            1.0F, 1.0F, -1.0F,   0.0F, 0.0F, 1.0F,  1.0F, 1.0F,  // 右上角
            -1.0F, 1.0F, -1.0F,  0.0F, 0.0F, 1.0F,  0.0F, 1.0F  // 左上角
    };

    // uv
    //  左上角          右上角
    // (0,0)---(0.5,0)---(1,0)
    //  |        |        |
    // (0,1)---(0.5,1)---(1,1)
    //  左下角           右下角
    //  double mode
    float verticesLeft[] = {
            // 位置              // 颜色           - 纹理坐标 -
            -1.0F, -1.0F, -1.0F, 1.0F, 0.0F, 0.0F,  0.0F, 1.0F,  // 左下角
            1.0F, -1.0F, -1.0F,  0.0F, 1.0F, 0.0F,  0.5F, 1.0F,  // 右下角
            1.0F, 1.0F, -1.0F,   0.0F, 0.0F, 1.0F,  0.5F, 0.0F,  // 右上角
            -1.0F, 1.0F, -1.0F,  0.0F, 0.0F, 1.0F,  0.0F, 0.0F   // 左上角
    };

    float verticesRight[] = {
            // 位置              // 颜色           - 纹理坐标 -
            -1.0F, -1.0F, -1.0F, 1.0F, 0.0F, 0.0F,  0.5F, 1.0F,  // 左下角
            1.0F, -1.0F, -1.0F,  0.0F, 1.0F, 0.0F,  1.0F, 1.0F,  // 右下角
            1.0F, 1.0F, -1.0F,   0.0F, 0.0F, 1.0F,  1.0F, 0.0F,  // 右上角
            -1.0F, 1.0F, -1.0F,  0.0F, 0.0F, 1.0F,  0.5F, 0.0F   // 左上角
    };

    float verticesAll[] = {
            // 位置              // 颜色           - 纹理坐标 -
            -1.0F, -1.0F, -1.0F, 1.0F, 0.0F, 0.0F,  0.0F, 1.0F,  // 左下角
            1.0F, -1.0F, -1.0F,  0.0F, 1.0F, 0.0F,  1.0F, 1.0F,  // 右下角
            1.0F, 1.0F, -1.0F,   0.0F, 0.0F, 1.0F,  1.0F, 0.0F,  // 右上角
            -1.0F, 1.0F, -1.0F,  0.0F, 0.0F, 1.0F,  0.0F, 0.0F   // 左上角
    };

    GLushort indicesAll[] = { // 注意索引从0开始!
            0, 1, 3, // 第一个三角形
            1, 2, 3  // 第二个三角形
    };
    //
    const char* vertexShaderSource = "#version 300 es\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "layout (location = 1) in vec3 aColor;\n"
                                     "layout (location = 2) in vec2 aTexCoord;\n"
                                     "\n"
                                     "out vec3 ourColor;\n"
                                     "out vec2 TexCoord;\n"
                                     "\n"
                                     "void main()\n"
                                     "{\n"
                                     "    gl_Position = vec4(aPos, 1.0);\n"
                                     "    ourColor = aColor;\n"
                                     "    TexCoord = aTexCoord;\n"
                                     "}";

    const char * fragmentShaderSource = "#version 300 es\n"
                                        "precision highp float;\n"
                                        "\n"
                                        "in vec3 ourColor;\n"
                                        "in vec2 TexCoord;\n"
                                        "out vec4 FragColor;\n"
                                        "\n"
                                        "uniform sampler2D ourTexture;\n"
                                        "\n"
                                        "void main()\n"
                                        "{\n"
                                        "    FragColor = texture(ourTexture, TexCoord);\n"
                                        "}";
}

RectTexture::RectTexture() {
    name_ = "RectTexture";
    enable_ = false;
    InitGL();
    //InitMixGL();
}

RectTexture::~RectTexture() {
}

void RectTexture::InitGL() {
    LoadShader("shader/vertex/rect_vertex.glsl", "shader/fragment/rect_fragment.glsl", vertexShaderSource, fragmentShaderSource);

    if (has_error_) {
        LOGW("loadShaderFromAsset rect texture has error");
        return;
    }

    vao_ = std::make_shared<lark::VertexArrayObject>(true, true);
    if (!vao_) return;
    // init texture vao
    InitVao(verticesTexture, sizeof(verticesTexture), indicesAll, sizeof(indicesAll), vao_.get());

    vao_left_ = std::make_shared<lark::VertexArrayObject>(true, true);
    if (!vao_left_) return;
    // init left vao
    InitVao(verticesLeft, sizeof(verticesLeft), indicesAll, sizeof(indicesAll), vao_left_.get());

    vao_right_ = std::make_shared<lark::VertexArrayObject>(true, true);
    if (!vao_right_) return;
    InitVao(verticesRight, sizeof(verticesRight), indicesAll, sizeof(indicesAll), vao_right_.get());

    vao_all_ =std::make_shared<lark::VertexArrayObject>(true, true);
    if (!vao_all_) return;
    InitVao(verticesAll, sizeof(verticesAll), indicesAll, sizeof(indicesAll), vao_all_.get());

    enable_ = true;
}

void RectTexture::Draw(lark::Object::Eye eye, const glm::mat4 &projection, const glm::mat4 &view) {
    Object::Draw(eye, projection, view);

//    LOGV("rect draw frameIndex %d; enable %d; hasError %d", frame_texture_, enable_, has_error_);

    if (!multiview_mode_) {
        DrawStereo(eye, projection, view);
        return;
    }

    if (!enable_ || has_error_ || !frame_texture_)
        return;

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    lark::VertexArrayObject * vao = eye == EYE_LEFT ? vao_left_.get() : vao_right_.get();
    shader_->UseProgram();

    glBindTexture(GL_TEXTURE_2D, frame_texture_);
    vao->BindVAO();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    shader_->UnUseProgram();
    vao->UnbindVAO();
    glBindTexture(GL_TEXTURE_2D, 0);

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);

    if (HasGLError()) {
        LOGD("render cloudtexturehas error. %d", frame_texture_);
    }
}

void RectTexture::DrawMultiview(const glm::mat4 &projection, const glm::mat4 &view) {
    Object::DrawMultiview(projection, view);
    lark::VertexArrayObject * vao = vao_all_.get();
    shader_->UseProgram();

    glBindTexture(GL_TEXTURE_2D, frame_texture_);
    vao->BindVAO();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    shader_->UnUseProgram();
    vao->UnbindVAO();
    glBindTexture(GL_TEXTURE_2D, 0);

    if (HasGLError()) {
        LOGD("render cloudtexturehas error. %d", frame_texture_);
    }
}

void RectTexture::InitVao(void *vertices, int verticesSize, void *indices, int indicesSize,
                         lark::VertexArrayObject *vao) {
    vao->BindVAO();
    vao->BindArrayBuffer();
    glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STREAM_DRAW);
    vao->BindElementArrayBuffer();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    // 纹理坐标
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    vao->UnbindVAO();
    vao->UnbindArrayBuffer();
}

void RectTexture::DrawStereo(lark::Object::Eye eye, const glm::mat4 &projection, const glm::mat4 &view) {
    // LOGV("draw stereo %d %d %d %d %d", frame_texture_left_, frame_texture_right_, multiview_mode_, enable_, has_error_);

    if (multiview_mode_) {
        LOGE("call draw stereo on multiview mode.");
        return;
    }
    if (!enable_ || has_error_ || !frame_texture_left_ || !frame_texture_right_)
        return;

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    Object::DrawMultiview(projection, view);
    lark::VertexArrayObject * vao = vao_all_.get();
    shader_->UseProgram();
    glBindTexture(GL_TEXTURE_2D, eye == EYE_LEFT ? frame_texture_left_ : frame_texture_right_);
    vao->BindVAO();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    shader_->UnUseProgram();
    vao->UnbindVAO();
    glBindTexture(GL_TEXTURE_2D, 0);

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);

    if (HasGLError()) {
        LOGD("render cloudtexturehas error. %d", frame_texture_);
    }
}

