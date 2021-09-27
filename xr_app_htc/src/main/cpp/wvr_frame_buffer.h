//
// Created by fcx on 2020/5/7.
//

#ifndef CLOUDLARKXR_WVR_FRAME_BUFFER_H
#define CLOUDLARKXR_WVR_FRAME_BUFFER_H


#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>

class WvrFrameBuffer {
public:
    static inline WvrFrameBuffer * GetFBOInstance(int width, int height) {
        WvrFrameBuffer * fbo = new WvrFrameBuffer(0, width, height);
        if (fbo->frame_buffer_id_ == 0)
            return NULL;
        else
            return fbo;
    }
    WvrFrameBuffer(int textureId, int width, int height, bool msaa = false, bool multiview = false);
    ~WvrFrameBuffer();
    void InitMSAA();
    void Init();
    void InitMultiviewMSAA();
    void InitMultiview();

    void BindFrameBuffer(bool multiview);
    void UnbindFrameBuffer(bool multiview);
    void Clear();

    inline void BindTexture() { glBindTexture(GL_TEXTURE_2D, texture_id_); }
    inline void UnbindTexture() { glBindTexture(GL_TEXTURE_2D, 0); }
    inline void GlViewportFull() { glViewport(0, 0, width_, height_); }
    inline void GlScissorFull() { glScissor(0, 0, width_, height_); }
    inline int texture_id() { return texture_id_; }
    inline int has_error() { return has_error_; }
private:
    void InitializeMultiView();

    bool msaa_ = false;
    int width_ = 0;
    int height_ = 0;
    GLuint render_frame_buffer_id_ = 0;
    GLuint frame_buffer_id_ = 0;
    GLuint depth_buffer_id_ = 0;
    GLuint render_texture_id_ = 0;
    GLuint texture_id_ = 0;
    bool has_error_ = false;
};

#endif //CLOUDLARKXR_WVR_FRAME_BUFFER_H
