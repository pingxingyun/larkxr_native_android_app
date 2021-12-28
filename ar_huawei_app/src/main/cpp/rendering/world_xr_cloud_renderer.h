//
// Created by Hayasi-Yumito on 2021/12/27.
//

#ifndef LARKXR_WORLD_XR_CLOUD_RENDERER_H
#define LARKXR_WORLD_XR_CLOUD_RENDERER_H

#include <vector>

#include <GLES2/gl2.h>
#include <utils.h>

#include "huawei_arengine_interface.h"
#include <ar_rect_texture.h>

namespace gWorldAr {

    class WorldXrCloudRenderer {
    public:

        std::shared_ptr<RectTexture> rect_render_ = nullptr;

        WorldXrCloudRenderer() = default;

        ~WorldXrCloudRenderer() = default;

        // Initialize the OpenGL state used by the plane renderer.
        void InitializePlaneGlContent();

        // Draw the provided plane.
        // @param projectionMat Draw the plane projection information matrix.
        // @param viewMat Draw the plane view information matrix.
        // @param session Query the sessions in the plane drawing.
        // @param plane Plane information of the real world in plane drawing.
        // @param color Color configuration of a plane.
        void
        Draw(const glm::mat4 &projectionMat, const glm::mat4 &viewMat, const HwArSession *session,
             const HwArPlane *plane, const glm::vec3 &color,
             const std::shared_ptr<RectTexture> &ptr);

        glm::quat normalVec = glm::vec3(0.0f);

        glm::quat poseValue;

    private:
        void UpdateForPlane(const HwArSession *session, const HwArPlane *plane);

        std::vector<glm::vec3> vertices;
        std::vector<GLushort> triangles;
        glm::mat4 modelMat = glm::mat4(1.0f);
        //trigrid纹理
        GLuint textureId;

        GLuint mShaderProgram;
        GLint mAttriVertices;
        GLint mUniformMvpMat;
        GLint mUniformTexture;
        GLint mUniformModelMat;
        GLint mUniformNormalVec;
        GLint mUniformColor;
    };
}

#endif //LARKXR_WORLD_XR_CLOUD_RENDERER_H
