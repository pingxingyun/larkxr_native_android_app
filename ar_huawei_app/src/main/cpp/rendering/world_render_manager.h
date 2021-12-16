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

#ifndef C_ARENGINE_WORLD_AR_RENDER_MANAGER_H
#define C_ARENGINE_WORLD_AR_RENDER_MANAGER_H

#include <unordered_map>

#include <utils.h>

#include "huawei_arengine_interface.h"
#include "rendering/world_background_renderer.h"
#include "rendering/world_object_renderer.h"
#include "rendering/world_plane_renderer.h"
#include "rendering/world_point_cloud_renderer.h"

namespace gWorldAr {
    struct ColoredAnchor {
        HwArAnchor *anchor;
        float color[4];
    };

    class WorldRenderManager {
    public:
        WorldRenderManager() = default;

        ~WorldRenderManager() = default;

        std::shared_ptr<RectTexture> rect_render_ = nullptr;

        // Initialize the OpenGL status, including the background, virtual object, point cloud, and plane drawing.
        // @param assetManager Wrapper of the bottom native implementation.
        void Initialize(AAssetManager *assetManager);

        // The manager, including the background, virtual object, point cloud, and plane drawing.
        // @param arSession Implement the session function.
        // @param arFrame Information about each frame during drawing.
        // @param coloredAnchors Color parameters required for drawing virtual objects.
        void OnDrawFrame(HwArSession *arSession, HwArFrame *arFrame,
                         const std::vector<ColoredAnchor> &coloredAnchors,
                         std::shared_ptr<RectTexture> ptr);

        // Implement the Draw function of the virtual object module in the rendering manager.
        // @param arSession Implement the session function.
        // @param arFrame Information about each frame during drawing.
        // @param viewMat Draw the matrix of the virtual object view.
        // @param projectionMat Draw the matrix of the virtual object projection.
        // @param mColoredAnchors Color parameters required for drawing virtual objects.
        void RenderObject(HwArSession *arSession, HwArFrame *arFrame, const glm::mat4 &viewMat,
                          const glm::mat4 &projectionMat,
                          const std::vector<ColoredAnchor> &mColoredAnchors,
                          std::shared_ptr<RectTexture> ptr);

        // Implement the Draw function of the background module in the rendering manager.
        // @param arSession Implement the session function.
        // @param arFrame Information about each frame during drawing.
        // @param viewMat Draw the background view matrix.
        // @param projectionMat Draw the background projection matrix.
        // @return Background drawing result status.
        bool InitializeDraw(HwArSession *arSession, HwArFrame *arFrame,
                            glm::mat4 *viewMat, glm::mat4 *projectionMat);

        // Implement the Draw function of the point cloud module in the rendering manager.
        // @param arSession Implement the session function.
        // @param arFrame Information about each frame during point cloud drawing.
        // @param viewMat Draw the matrix of the point cloud view.
        // @param projectionMat Draw the point cloud view matrix.
        void RenderPointCloud(HwArSession *arSession, HwArFrame *arFrame, const glm::mat4 &viewMat,
                              const glm::mat4 &projectionMat, std::shared_ptr<RectTexture> ptr);

        // Implement the Draw function of the plane module in the rendering manager.
        // @param arSession Implement the session function.
        // @param viewMat Information about each frame during plane drawing.
        // @param projectionMat Draw the plane projection matrix.
        void RenderPlanes(HwArSession *arSession, const glm::mat4 &viewMat,
                          const glm::mat4 &projectionMat,
                          std::shared_ptr<RectTexture> ptr);

        bool HasDetectedPlanes();

        void Initialize();


    private:
        int32_t mPlaneCount = 0;

        // Store the randomly selected colors used by each plane.
        std::unordered_map<HwArPlane *, glm::vec3> mPlaneColorMap = {};

        // The first plane is always white, and if true, a plane has been found at a point.
        bool firstPlaneHasBeenFound = false;

        WorldBackgroundRenderer mBackgroundRenderer = gWorldAr::WorldBackgroundRenderer();

        WorldPointCloudRenderer mPointCloudRenderer = gWorldAr::WorldPointCloudRenderer();

        WorldPlaneRenderer mPlaneRenderer = gWorldAr::WorldPlaneRenderer();

        WorldObjectRenderer mObjectRenderer = gWorldAr::WorldObjectRenderer();

        void RendererPlane(HwArPlane *arPlane, HwArTrackable *arTrackable, glm::vec3 &color);
    };
}
#endif