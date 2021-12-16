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

#ifndef C_ARENGINE_WORLD_AR_POINT_CLOUD_RENDERER_H
#define C_ARENGINE_WORLD_AR_POINT_CLOUD_RENDERER_H

#include <cstdlib>
#include <vector>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <rect_texture.h>

#include "huawei_arengine_interface.h"

namespace gWorldAr {
    class WorldPointCloudRenderer {
    public:
        WorldPointCloudRenderer() = default;

        ~WorldPointCloudRenderer() = default;

        // Initial OpenGL status, which needs to be called on the GL thread.
        void InitializePointCloudGlContent();

        // AR point cloud rendering.
        // @param mvpMatrix Projection matrix of the point cloud model view.
        // @param arSession Query a point cloud session.
        // @param arPointCloud Point the cloud data to the point cloud for rendering.
        void Draw(glm::mat4 mvpMatrix, const HwArSession *arSession, const HwArPointCloud *arPointCloud,
             std::shared_ptr<RectTexture> ptr);

    private:
        GLuint mShaderProgram;
        GLuint mAttributeVertices;
        GLuint mUniformMvpMat;
    };
}
#endif