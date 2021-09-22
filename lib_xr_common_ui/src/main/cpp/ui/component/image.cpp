//
// Created by fcx@pingxingyun.com on 2019/11/11.
//

#include <memory>
#include <asset_loader.h>
#include "env_context.h"
#include "texture.h"
#include "vertex_array_object.h"
#include "image.h"
#include "log.h"
#include "bitmap_factory.h"

#define LOG_TAG "image"

using namespace glm;
using namespace std;

Image::Image(): load_mutex_()
{
    name_ = LOG_TAG;
    size_ = vec2(0.0F, 0.0F);
    enable_ = false;

    vao_ = std::make_shared<lark::VertexArrayObject>(true, false);

    LoadShaderFromAsset(Context::instance()->asset_manager(),
            "shader/vertex/image_vertex.glsl", "shader/fragment/image_fragment.glsl");

    if (has_error_) {
        LOGW("loadShaderFromAsset image has error");
        return;
    }

    model_location_ = shader_->GetUniformLocation("uModel");
    view_location_ = shader_->GetUniformLocation("uView");
    projection_location_ = shader_->GetUniformLocation("uProjection");

    enable_ = true;
}

Image::~Image() = default;


void Image::LoadTexture(const char *buffer, int len) {
    std::lock_guard<std::mutex> lock(load_mutex_);
    image_buffer_ = std::vector<char>(buffer, buffer + len);
    need_load_ = true;
    // clear loclpath.
    path_ = "";
}

void Image::SetPath(const std::string & path, bool isLocal) {
    if (path != path_) {
        LOGV("update image url %s; old %s; isLocal %d;", path.c_str(), path_.c_str(), (int)isLocal);
        path_ = path;
        lark::BitmapFactory* bitmapFactory = Context::instance()->bitmap_factory();
        EnvWrapper envWrapper = Context::instance()->GetEnv();
        lark::TextureAsset asset = {
                isLocal ? lark::TextureAssetType_Local_Normal : lark::TextureAssetType_Net_Normal,
                path
        };
        lark::AndroidAssetContext context = {
                Context::instance()->native_activity(),
                envWrapper.get(),
                bitmapFactory
        };
        texture_ = lark::AssetLoader::instance()->LoadTexture(&context, asset);
        if (texture_ == nullptr) {
            return;
        }
        if (callback_ != nullptr) {
            callback_->OnImageInited(this);
        }
        need_update_cover_ = true;
    }
}

void Image::set_scale(float scale) {
    component::Base::set_scale(scale);
    need_update_cover_ = true;
}

void Image::set_size(const glm::vec2& size) {
    component::Base::set_size(size);
    need_update_cover_ = true;
}

void Image::set_position(const glm::vec3& position) {
    component::Base::set_position(position);
    need_update_cover_ = true;
}

void Image::set_position(float x, float y, float z) {
    Base::set_position(x, y, z);
    need_update_cover_ = true;
}

void Image::SetPositionX(float x) {
    Base::SetPositionX(x);
    need_update_cover_ = true;
}

void Image::SetPositionY(float y) {
    Base::SetPositionY(y);
    need_update_cover_ = true;
}

void Image::SetPositionZ(float z) {
    Base::SetPositionZ(z);
    need_update_cover_ = true;
}

void Image::Draw(Eye eye, const glm::mat4 &projection, const glm::mat4 &eyeView) {
    Object::Draw(eye, projection, eyeView);
    //
    if (need_load_) {
        LOGV("LoadTexture cover need load url");
        lark::BitmapFactory* bitmapFactory = Context::instance()->bitmap_factory();
        EnvWrapper envWrapper = Context::instance()->GetEnv();

        lark::Texture* texture = lark::Texture::LoadTexture(bitmapFactory, envWrapper.get(),
                &image_buffer_[0], image_buffer_.size(), 0);
        if (texture != nullptr) {
            texture_.reset(texture);
            need_update_cover_ = true;
            if (callback_ != nullptr) {
                callback_->OnImageInited(this);
            }
        }
        image_buffer_.clear();
        need_load_ = false;
    }

    if (has_error_ || texture_ == nullptr || !enable_)
        return;

    if (need_update_cover_) {
        texture_->BindTexture();
//        mCover->bindBitmap(GL_RGB5_A1);
        texture_->BindBitmap();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 3);

        texture_->UnBindTexture();
        texture_->CleanBitmap();

        vao_->BindVAO();
        vao_->BindArrayBuffer();

        const float xpos = position_.x;
        const float ypos = position_.y;
        const float z = position_.z;
        //        const float w = 1.0f * scale;
//        const float w = (float) texture_->getWidth() * UNIT_PIXEL_SCALE;
//        const float h = 1.0f * scale * (float) texture_->getHeight() / (float) texture_->getWidth();
//        const float h = (float) texture_->getHeight() * UNIT_PIXEL_SCALE;
//        const float y = 1.0f * scale;
        vec2 size = GetScaledSize();
        const float w = size.x;
        const float h = size.y;

        // Update VBO for each character
        const GLfloat vertices[] = {
                xpos,     ypos + h, z, 0.0, 0.0,
                xpos,     ypos,     z, 0.0, 1.0,
                xpos + w, ypos,     z, 1.0, 1.0,

                xpos,     ypos + h, z, 0.0, 0.0,
                xpos + w, ypos,     z, 1.0, 1.0,
                xpos + w, ypos + h, z, 1.0, 0.0
        };

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);

        int stride = (2 + 3) * sizeof(float);
        GLuint offset = 0;
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (const void*) offset);

        offset += sizeof(float) * 3;
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (const void*) offset);

        vao_->UnbindVAO();
        vao_->UnbindArrayBuffer();

        // update done
        need_update_cover_ = false;
    }

    shader_->UseProgram();
    // mvp
    glUniformMatrix4fv(model_location_, 1, GL_FALSE, glm::value_ptr(GetTransforms()));
    glUniformMatrix4fv(view_location_, 1, GL_FALSE, glm::value_ptr(eyeView));
    glUniformMatrix4fv(projection_location_, 1, GL_FALSE, glm::value_ptr(projection));

    glActiveTexture(GL_TEXTURE0);
    texture_->BindTexture();
    vao_->BindVAO();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    shader_->UnUseProgram();
    vao_->UnbindVAO();
    texture_->UnBindTexture();
}

glm::vec2 Image::GetScaledSize() {
    if (texture_ == nullptr) {
        return vec2();
    }
    float textureWidth = (float)texture_->width() * component::UNIT_PIXEL_SCALE * scale_;
    float textureHeight = (float)texture_->height() * component::UNIT_PIXEL_SCALE  * scale_;
    float sizeW = size_.x == 0 ? textureWidth : size_.x * scale_;
    float sizeH = size_.y == 0 ? textureHeight :  size_.y  * scale_;

    switch (scale_type_) {
        case CONTAIN:
        {
            float s = glm::max(sizeW / textureWidth, sizeH / textureHeight);
            return vec2(textureWidth * s, textureHeight * s);
        }
        case FILL:
            return vec2(sizeW, sizeH);
        default:
            return vec2();
    }
}

glm::vec2 Image::GetSize() {
    return GetScaledSize();
}

glm::vec2 Image::GetTextureSize() {
    if (!texture_) {
        return glm::vec2();
    }
    return glm::vec2(texture_->width(), texture_->height());
}
