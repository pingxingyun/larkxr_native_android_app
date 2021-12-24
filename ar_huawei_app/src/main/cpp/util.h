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

#ifndef C_ARENGINE_HELLOE_AR_UTIL_H
#define C_ARENGINE_HELLOE_AR_UTIL_H

#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <vector>

#include <android/asset_manager.h>
#include <android/log.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <jni.h>
#include <utils.h>

#include "huawei_arengine_interface.h"
#include "Ar_Demo.h"

#ifndef LOGI
#define LOGI(...) \
    __android_log_print(ANDROID_LOG_INFO, "worldArCPP", __VA_ARGS__)
#endif

#ifndef LOGE
#define LOGE(...) \
    __android_log_print(ANDROID_LOG_ERROR, "worldArCPP", __VA_ARGS__)
#endif

#ifndef CHECK
#define CHECK(condition)                                                   \
    if (!(condition)) {                                                      \
        LOGE("*** CHECK FAILED at %d: %s", __LINE__, #condition); \
        abort();                                                               \
    }
#endif

namespace gWorldAr {
    // Utilities for C hello AR project.
    namespace util {
        class Util {
        public:
            explicit Util(const HwArSession *session)
            {
                HwArPose_create(session, nullptr, &pose);
            }

            ~Util()
            {
                HwArPose_destroy(pose);
            }

            HwArPose *GetArPose() const
            {
                return pose;
            }

            // Delete copy constructors.
            Util(const Util &) = delete;

            void operator=(const Util &) = delete;

        private:
            HwArPose *pose = nullptr;
        };

        using FileInfor = struct {
            AAssetManager *mgr;
            std::string fileName;
        };

        using FileData = struct {
            std::vector<char *> perVertInfoList;
            int i;
        };

        using DrawTempData = struct {
            std::vector<GLfloat> tempPositions;
            std::vector<GLfloat> tempNormals;
            std::vector<GLfloat> tempUvs;
            std::vector<GLushort> vertexIndices;
            std::vector<GLushort> normalIndices;
            std::vector<GLushort> uvIndices;
        };

        // 检查GL错误，如果遇到错误则中止.
        // @param 操作， GL函数调用的名称.
        void CheckGlError(const char *operation);

        // 创建着色器程序ID.
        // @param 顶点源，顶点着色源.
        // @param 片段源，片段着色器源.
        // @return GL状态值
        GLuint CreateProgram(const char *vertexSource, const char *fragmentSource);

        void GetTransformMatrixFromAnchor(HwArSession *arSession,
                                          const HwArAnchor *arAnchor,
                                          glm::mat4 *outModelMat);

        glm::vec3 GetPlaneNormal(const HwArSession &arSession, const HwArPose &planePose);

        glm::quat GetArPose(const HwArSession &arSession, const HwArPose &planePose);

        // 计算从照相机到平面的法线距离，给定平面的y轴应平行于平面的法线.
        // 例如平面的中心位置或命中测试位置
        float CalculateDistanceToPlane(const HwArSession &arSession,
                                       const HwArPose &planePose,
                                       const HwArPose &cameraPose);

        bool LoadPngFromAssetManager(int target, const std::string &path);

        // 从应用程序中从assets文件夹加载obj文件.
        //
        // @param fileInformation, AAssetManager指针，obj文件名称.
        // @param outVertices, 输出顶点.
        // @param outNormals, 输出法线.
        // @param outUv, 输出纹理UV坐标.
        // @param outIndices, 输出三角指数.
        // @return 如果正确加载了obj，则为true，否则为false.
        bool LoadObjFile(FileInfor fileInformation,
                         std::vector<GLfloat> &outVertices,
                         std::vector<GLfloat> &outNormals,
                         std::vector<GLfloat> &outUv,
                         std::vector<GLushort> &outIndices);

    }
}
#endif