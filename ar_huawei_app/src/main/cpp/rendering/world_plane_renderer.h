/**
 * Copyright 2020. Huawei Technologies Co., Ltd. All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#ifndef C_ARENGINE_WORLD_AR_PLANE_RENDERER_H
#define C_ARENGINE_WORLD_AR_PLANE_RENDERER_H

#include <vector>

#include <GLES2/gl2.h>
#include <utils.h>

#include "huawei_arengine_interface.h"
#include <ar_rect_texture.h>

namespace gWorldAr {

    class WorldPlaneRenderer {
    public:

        std::shared_ptr<ArRectTexture> rect_render_ = nullptr;

        WorldPlaneRenderer() = default;

        ~WorldPlaneRenderer() = default;

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
             const std::shared_ptr<ArRectTexture> &ptr);

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
#endif

