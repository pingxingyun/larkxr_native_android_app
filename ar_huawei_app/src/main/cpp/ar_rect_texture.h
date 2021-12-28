// 
// Created by Hayasi-Yumito on 2021/12/14.
//
#pragma once
#ifndef RECT_TEXTURE_INCLUDE
#define RECT_TEXTURE_INCLUDE

#include "object.h"
#include "vertex_array_object.h"

class ArRectTexture : public lark::Object {
public:
    ArRectTexture();

    ArRectTexture(int mix);

    ~ArRectTexture();

    void InitGL();
    void InitMixGL();

    // draw
    virtual void Draw(Eye eye, const glm::mat4& projection, const glm::mat4& view);
    virtual void DrawStereo(Eye eye, const glm::mat4& projection, const glm::mat4& view);
    virtual void DrawMixStereo(const glm::mat4& projection, const glm::mat4& view);
    virtual void DrawMultiview(const glm::mat4& projection, const glm::mat4& view);

    inline void SetStereoTexture(int texture_left, int texture_right) {
        multiview_mode_ = false;
        frame_texture_left_ = texture_left;
        frame_texture_right_ = texture_right;
    }
    inline void SetMutiviewModeTexture(int texture) {
        multiview_mode_ = true;
        frame_texture_ = texture;
    }
    inline void ClearTexture() {
        frame_texture_ = 0;
        frame_texture_left_ = 0;
        frame_texture_right_ = 0;
    }

    inline void set_frame_texture_(int texture) { frame_texture_ = texture; }
    inline void set_multiview_mode(bool multiview) { multiview_mode_ = multiview; }
private:
    void InitVao(void* vertices, int verticesSize, void* indices, int indicesSize, lark::VertexArrayObject * vao);

    std::shared_ptr<lark::VertexArrayObject> vao_left_;
    std::shared_ptr<lark::VertexArrayObject> vao_right_;
    std::shared_ptr<lark::VertexArrayObject> vao_all_;

    int frame_texture_ = 0;
    int frame_texture_left_ = 0;
    int frame_texture_right_ = 0;
    bool multiview_mode_ = true;

};
#endif // RECT_TEXTURE_INCLUDE