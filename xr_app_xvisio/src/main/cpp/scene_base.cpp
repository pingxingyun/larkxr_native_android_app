//
// Created by fcx on 2020/6/30.
//

#include <log.h>
#include "scene_base.h"
#include "lark_xr/xr_config.h"

SceneBase::SceneBase() {

}

SceneBase::~SceneBase() {

}

void SceneBase::InitGl(int eyeBufferWidth, int eyeBufferHeight) {
    eye_buffer_width_ = eyeBufferWidth;
    eye_buffer_height_ = eyeBufferHeight;

    float rr = M_PI / 180.0F;

    for (int eye = 0; eye < 2; eye++) {
        larkxrRenderFov fov = lark::XRConfig::fov[eye];
        float l = near_z_ * tanf( fov.left * rr );
        float r = near_z_ * tanf( fov.right * rr );
        float t = near_z_ * tanf( fov.top * rr );
        float b = near_z_ * tanf( fov.bottom * rr );
        projection_[eye] = glm::frustum(-l,r,-t,b, near_z_, far_z_);
    }

    ipd_ = lark::XRConfig::ipd;

    LOGV("local ui use eye distance %f", ipd_);
//    float halfW = near_z_ * tanf( fov_0_ * 0.5F * 3.14159260F /180.0F );
//    float halfH = near_z_ * tanf( fov_1_ * 0.5F * 3.14159260F /180.0F );

//    LOGI("FOV is %f , halfW is %f", fov_0_, halfW);
//    projection_[0] = glm::frustum(-halfW,halfW,-halfH,halfH, near_z_, far_z_);
//    projection_[1] = glm::frustum(-halfW,halfW,-halfH,halfH, near_z_, far_z_);
}

void SceneBase::UpdateHMDPose(glm::quat rotation, glm::vec3 position) {
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

    // oMat[0] = hmdDiff[0];
    // oMat[1] = hmdDiff[1];

    view_[0] = oMat[0]*qMat*pMat;
    view_[1] = oMat[1]*qMat*pMat;

    for(auto it = objects_.begin(); it != objects_.end(); it ++) {
        if (it->get()->active()) {
            it->get()->Update();
        }
    }
}

void SceneBase::Draw(int eye) {
    // 开启透明同道混合
    glEnable( GL_BLEND );
    //配置混合方程式，默认为 GL_FUNC_ADD 方程
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glEnable( GL_SCISSOR_TEST );
//    glDepthMask( GL_TRUE );
//    glEnable( GL_DEPTH_TEST );
//    glDepthFunc( GL_LEQUAL );
//    glEnable( GL_CULL_FACE );
//    glCullFace( GL_BACK );
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

void SceneBase::AddObject(std::shared_ptr<lark::Object> object) {
    objects_.push_back(object);
}

void SceneBase::RemoveObject(std::shared_ptr<lark::Object> object) {
    // TODO objects manager
    for (auto it = objects_.begin(); it != objects_.end(); it++) {
        if (object->id() == it->get()->id()) {
            objects_.erase(it);
        }
    }
}

void SceneBase::ClearObject() {
    if (objects_.empty()) {
        return;
    }
    objects_.clear();
}

void SceneBase::Draw(int eye, const glm::mat4 &project, const glm::mat4 &view) {
    // 开启透明同道混合
//    glEnable( GL_BLEND );
//    //配置混合方程式，默认为 GL_FUNC_ADD 方程
//    glBlendEquation(GL_FUNC_ADD);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glViewport( 0, 0, 1920, 2160);
//    glScissor( 0, 0, 1920, 2160);
//    glClearColor( 0.125F, 0.0F, 0.125F, 1.0F );
//    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
//    for(auto it = objects_.begin(); it != objects_.end(); it ++) {
//        if (it->get()->active()) {
//            it->get()->Draw(eye == 0 ? lark::Object::EYE_LEFT : lark::Object::EYE_RIGHT, project, view);
//        }
//    }
//    glDisable(GL_BLEND);
}
