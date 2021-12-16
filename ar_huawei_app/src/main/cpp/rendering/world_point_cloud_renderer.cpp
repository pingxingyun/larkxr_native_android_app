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

#include <utils.h>
#include "world_point_cloud_renderer.h"

#include "util.h"

namespace gWorldAr {
    namespace {
        constexpr char VERTEX_SHADER[] = R"(
        attribute vec4 vertex;
        uniform mat4 mvp;
        void main() {
            gl_PointSize = 5.0;
            gl_Position = mvp * vec4(vertex.xyz, 1.0);
        })";

        constexpr char FRAGMENT_SHADER[] = R"(
        precision lowp float;
        void main() {
            gl_FragColor = vec4(0.1215, 0.7372, 0.8235, 1.0);
        })";
    }

    void WorldPointCloudRenderer::InitializePointCloudGlContent()
    {
        mShaderProgram = util::CreateProgram(VERTEX_SHADER, FRAGMENT_SHADER);
        CHECK(mShaderProgram);
        mAttributeVertices = glGetAttribLocation(mShaderProgram, "vertex");
        mUniformMvpMat = glGetUniformLocation(mShaderProgram, "mvp");
        util::CheckGlError("WorldPointCloudRenderer::InitializeBackGroundGlContent()");
    }

    void WorldPointCloudRenderer::Draw(glm::mat4 mvpMatrix, const HwArSession *arSession,
                                       const HwArPointCloud *arPointCloud,
                                       std::shared_ptr<RectTexture> ptr)
    {
        CHECK(mShaderProgram);
        glUseProgram(mShaderProgram);

        int32_t numberOfPoints = 0;
        HwArPointCloud_getNumberOfPoints(arSession, arPointCloud, &numberOfPoints);
        if (numberOfPoints <= 0) {
            return;
        }

        const float *pointCloudData;
        HwArPointCloud_getData(arSession, arPointCloud, &pointCloudData);
        glUniformMatrix4fv(mUniformMvpMat, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
        glEnableVertexAttribArray(mAttributeVertices);

        // The point dimension is 4.
        glVertexAttribPointer(mAttributeVertices, 4, GL_FLOAT, GL_FALSE, 0,
                              pointCloudData);
        glDrawArrays(GL_POINTS, 0, numberOfPoints);
        glUseProgram(0);

//        ptr->DrawMultiview(glm::mat4(), glm::mat4());
        util::CheckGlError("WorldPointCloudRenderer::Draw");
    }
}