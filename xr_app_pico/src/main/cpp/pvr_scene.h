//
// Created by fcx on 2020/6/30.
//

#ifndef CLOUDLARKXR_PVR_SCENE_H
#define CLOUDLARKXR_PVR_SCENE_H


//#include <glm/glm.hpp>
//#include <glm/detail/type_quat.hpp>
#include <object.h>

class PvrScene {
public:
    PvrScene();
    virtual ~PvrScene();

    virtual void InitGl(int eyeBufferWidth, int eyeBufferHeight);
    virtual void UpdateHMDPose(glm::quat rotation, glm::vec3 position);
    virtual void Draw(int eye);

protected:
    void AddObject(std::shared_ptr<lark::Object> object);
    void RemoveObject(std::shared_ptr<lark::Object> object);
    void ClearObject();

    float fov_0_ = 101.0f;
    float fov_1_ = 101.0f;
    float near_z_ = 0.01f;
    float far_z_ = 100.0f;
    float ipd_ = 0.06003f;

    int eye_buffer_width_ = 1920;
    int eye_buffer_height_ = 1920;

    glm::mat4 view_[2];
    glm::mat4 projection_[2];

    // objects.
    std::vector<std::shared_ptr<lark::Object>> objects_{};
};


#endif //CLOUDLARKXR_PVR_SCENE_H
