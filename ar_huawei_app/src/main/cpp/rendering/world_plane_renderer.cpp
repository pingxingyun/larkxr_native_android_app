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

#include <Ar_Demo.h>
#include "world_plane_renderer.h"

#include "util.h"

namespace gWorldAr {
    namespace {
        constexpr char VERTEX_SHADER[] = R"(
        precision highp float;
        precision highp int;
        attribute vec3 vertex;
        varying vec2 v_textureCoords;
        varying float v_alpha;

        uniform mat4 mvp;
        uniform mat4 model_mat;
        uniform vec3 normal;

        void main() {
            v_alpha = vertex.z;
            vec4 local_pos = vec4(vertex.x, 0.0, vertex.y, 1.0);
            gl_Position = mvp * local_pos;
            vec4 world_pos = model_mat * local_pos;
            const vec3 arbitrary = vec3(1.0, 1.0, 0.0);
            vec3 vec_u = normalize(cross(normal, arbitrary));
            vec3 vec_v = normalize(cross(normal, vec_u));
            v_textureCoords = vec2(dot(world_pos.xyz, vec_u), dot(world_pos.xyz, vec_v));
        })";

        constexpr char FRAGMENT_SHADER[] = R"(
        precision highp float;
        precision highp int;
        uniform sampler2D texture;
        uniform vec3 color;
        varying vec2 v_textureCoords;
        varying float v_alpha;
        void main() {
            float r = texture2D(texture, v_textureCoords).r;
            gl_FragColor = vec4(color.xyz, r * v_alpha);
        })";
    }

    void WorldPlaneRenderer::InitializePlaneGlContent()
    {
        mShaderProgram = util::CreateProgram(VERTEX_SHADER, FRAGMENT_SHADER);
        if (!mShaderProgram) {
            LOGE("Could not create program.");
        }

        mUniformMvpMat = glGetUniformLocation(mShaderProgram, "mvp");
        mUniformTexture = glGetUniformLocation(mShaderProgram, "texture");
        mUniformModelMat = glGetUniformLocation(mShaderProgram, "model_mat");
        mUniformNormalVec = glGetUniformLocation(mShaderProgram, "normal");
        mUniformColor = glGetUniformLocation(mShaderProgram, "color");
        mAttriVertices = glGetAttribLocation(mShaderProgram, "vertex");

        // 生成纹理
        glGenTextures(1, &textureId);
        // 绑定当前纹理，开始纹理的设置
        glBindTexture(GL_TEXTURE_2D, textureId);
        // 设置纹理的显示方式，拉伸平铺之类的
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // 从 asset 里面载入图片
        // 实际调用了 java JniInterface 里面的 loadImage 和 loadTexture
        // 先用 loadImage 加载到 Bitmap 然后用 loadTexture 将 Bitmap 绑定到当前纹理 GLUtils.texImage2D(target, 0, bitmap, 0);
        if (!util::LoadPngFromAssetManager(GL_TEXTURE_2D, "trigrid.png")) {
            LOGE("Could not load png texture for planes.");
        }

        glGenerateMipmap(GL_TEXTURE_2D);
        // 当纹理用完了，通知 opengl 设置为空纹理
        glBindTexture(GL_TEXTURE_2D, 0);

        //util::CheckGlError("WorldPlaneRenderer::InitializeBackGroundGlContent()"
    }

    void WorldPlaneRenderer::Draw(const glm::mat4 &projectionMat, const glm::mat4 &viewMat,
                                  const HwArSession *session,
                                  const HwArPlane *plane, const glm::vec3 &color,
                                  const std::shared_ptr<ArRectTexture> &ptr) {
        if (!mShaderProgram) {
            LOGE("mShaderProgram is null.");
            return;
        }
        UpdateForPlane(session, plane);

        glUseProgram(mShaderProgram);
        glDepthMask(GL_FALSE);

        glActiveTexture(GL_TEXTURE0);
        glUniform1i(mUniformTexture, 0);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // Write the final mvp matrix for this plane renderer.
        //编写此平面渲染器的最终mvp矩阵。
        glUniformMatrix4fv(mUniformMvpMat, 1, GL_FALSE,
                           glm::value_ptr(projectionMat * viewMat * modelMat));

        glUniformMatrix4fv(mUniformModelMat, 1, GL_FALSE,
                           glm::value_ptr(modelMat));

        glUniform3f(mUniformNormalVec, normalVec.x, normalVec.y, normalVec.z);
        glUniform3f(mUniformColor, color.x, color.y, color.z);


        glEnableVertexAttribArray(mAttriVertices);

        // When the GL vertex attribute is a pointer, the number of vertices is 3.
        //当GL顶点属性为指针时，顶点数为3。
        glVertexAttribPointer(mAttriVertices, 3, GL_FLOAT, GL_FALSE, 0,
                              vertices.data());

        glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_SHORT,
                       triangles.data());

        glUseProgram(0);
        glDepthMask(GL_TRUE);

        //ptr->DrawMixMultiview(projectionMat,viewMat);
        //ptr->DrawStereo(lark::Object::EYE_LEFT,projectionMat,viewMat);
        //ptr->DrawStereo(lark::Object::EYE_RIGHT,projectionMat,viewMat);

        util::CheckGlError("WorldPlaneRenderer::Draw()");
    }

    void WorldPlaneRenderer::UpdateForPlane(const HwArSession *session, const HwArPlane *plane) {
        vertices.clear();
        triangles.clear();

        int32_t polygonLength = 0;
        HwArPlane_getPolygonSize(session, plane, &polygonLength);
        if (polygonLength == 0) {
            LOGE("WorldPlaneRenderer::UpdateForPlane, no valid plane polygon is found");
            return;
        }

        const int32_t verticesSize = polygonLength / 2;
        std::vector<glm::vec2> raw_vertices(verticesSize);
        HwArPlane_getPolygon(session, plane, glm::value_ptr(raw_vertices.front()));

        // Fill in vertices 0 to 3. Use the vertex.xy coordinates for the x and z coordinates of the vertex.
        // The z coordinate of the vertex is used for alpha. The alpha value of the outer polygon is 0.
        for (int32_t i = 0; i < verticesSize; ++i) {
            vertices.emplace_back(raw_vertices[i].x, raw_vertices[i].y, 0.0f);
        }

        util::Util scopedArPose(session);
        HwArPlane_getCenterPose(session, plane, scopedArPose.GetArPose());

        HwArPose_getMatrix(session, scopedArPose.GetArPose(),
                           glm::value_ptr(modelMat));

        normalVec = util::GetPlaneNormal(*session, *scopedArPose.GetArPose());

        poseValue = util::GetArPose(*session, *scopedArPose.GetArPose());

        const float kFeatherLength = 0.2f;

        // Feather scale of the distance between the plane center and the vertex.
        const float kFeatherScale = 0.2f;

        // Fill vertex 0 to vertex 3, and set alpha to 1.
        for (int32_t i = 0; i < verticesSize; ++i) {
            glm::vec2 v = raw_vertices[i];
            const float scale =
                    1.0f - std::min((kFeatherLength / glm::length(v)), kFeatherScale);
            const glm::vec2 result_v = scale * v;
            vertices.emplace_back(result_v.x, result_v.y, 1.0f);
        }

        const int32_t verticesLength = vertices.size();

        // Obtain the number of vertices.
        const int32_t halfVerticesLength = verticesLength / 2;

        // Generate triangles (4, 5, 6) and (4, 6, 7).
        for (int i = halfVerticesLength + 1; i < verticesLength - 1; ++i) {
            triangles.push_back(halfVerticesLength);
            triangles.push_back(i);
            triangles.push_back(i + 1);
        }

        // Generate triangles (0, 1, 4), (4, 1, 5), (5, 1, 2), and (5, 2, 6).
        // (6, 2, 3), (6, 3, 7), (7, 3, 0), (7, 0, 4)
        for (int i = 0; i < halfVerticesLength; ++i) {
            triangles.push_back(i);
            triangles.push_back((i + 1) % halfVerticesLength);
            triangles.push_back(i + halfVerticesLength);

            triangles.push_back(i + halfVerticesLength);
            triangles.push_back((i + 1) % halfVerticesLength);
            triangles.push_back(
                    (i + halfVerticesLength + 1) % halfVerticesLength + halfVerticesLength);
        }
    }


}