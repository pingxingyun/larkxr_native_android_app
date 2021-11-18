//
// Created by fcx on 2020/6/30.
//

#include <log.h>
#include "pvr_scene.h"

PvrScene::PvrScene() {

}

PvrScene::~PvrScene() {

}

void PvrScene::InitGl(int eyeBufferWidth, int eyeBufferHeight) {
    eye_buffer_width_ = eyeBufferWidth;
    eye_buffer_height_ = eyeBufferHeight;

    float halfW = near_z_ * tanf( fov_0_ * 0.5F * 3.14159260F /180.0F );
    float halfH = near_z_ * tanf( fov_1_ * 0.5F * 3.14159260F /180.0F );
    LOGI("FOV is %f , halfW is %f", fov_0_, halfW);
    projection_[0] = glm::frustum(-halfW,halfW,-halfH,halfH, near_z_, far_z_);
    projection_[1] = glm::frustum(-halfW,halfW,-halfH,halfH, near_z_, far_z_);
}

void PvrScene::UpdateHMDPose(glm::quat rotation, glm::vec3 position) {
    glm::fquat hmdOri = glm::fquat(
            rotation.w,
            -rotation.x,
            -rotation.y,
            -rotation.z
    );

    glm::vec3 hmdPos = glm::vec3(
            -position.x,
            -position.y,
            -position.z
    );

    glm::mat4 hmdDiff[2];
    hmdDiff[0] = glm::translate( glm::mat4(1.0f), glm::vec3(-ipd_ * 0.5, 0.0f, 0.0f) );
    hmdDiff[1] = glm::translate( glm::mat4(1.0f), glm::vec3(ipd_ * 0.5, 0.0f, 0.0f) );

    glm::mat4 qMat = glm::mat4_cast(hmdOri);
    glm::mat4 pMat = glm::translate(glm::mat4(1.0f), hmdPos);

    glm::mat4 oMat[2];
    oMat[0] = glm::inverse(hmdDiff[0]);
    oMat[1] = glm::inverse(hmdDiff[1]);

    view_[0] = oMat[0]*qMat*pMat;
    view_[1] = oMat[1]*qMat*pMat;

    for(auto it = objects_.begin(); it != objects_.end(); it ++) {
        if (it->get()->active()) {
            it->get()->Update();
        }
    }
}

void PvrScene::Draw(int eye) {
    // 开启透明同道混合
    glEnable( GL_BLEND );
    //配置混合方程式，默认为 GL_FUNC_ADD 方程
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glViewport( 0, 0, eye_buffer_width_, eye_buffer_height_);
//    glScissor( 0, 0, eye_buffer_width_, eye_buffer_height_);
    glClearColor( 0.125F, 0.0F, 0.125F, 1.0F );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    for(auto it = objects_.begin(); it != objects_.end(); it ++) {
        if (it->get()->active()) {
            it->get()->Draw(eye == 0 ? lark::Object::EYE_LEFT : lark::Object::EYE_RIGHT,
                            projection_[eye], view_[eye]);
        }
    }
    glDisable(GL_BLEND);
}

void PvrScene::AddObject(std::shared_ptr<lark::Object> object) {
    objects_.push_back(object);
}

void PvrScene::RemoveObject(std::shared_ptr<lark::Object> object) {
    // TODO objects manager
    for (auto it = objects_.begin(); it != objects_.end(); it++) {
        if (object->id() == it->get()->id()) {
            objects_.erase(it);
        }
    }
}

void PvrScene::ClearObject() {
    if (objects_.empty()) {
        return;
    }
    objects_.clear();
}
