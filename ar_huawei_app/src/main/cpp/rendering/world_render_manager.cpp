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

#include "rendering/world_render_manager.h"

#include <android/asset_manager.h>
#include <array>
#include <jni.h>

#include "util.h"
#include "Ar_Demo.h"

namespace gWorldAr {
/*    void WorldRenderManager::Initialize(AAssetManager *assetManager)
    {
        LOGI("WorldRenderManager-----Initialize() start.");
        mBackgroundRenderer.InitializeBackGroundGlContent();
        mPointCloudRenderer.InitializePointCloudGlContent();
        mObjectRenderer.InitializeObjectGlContent(assetManager, "AR_logo.obj", "AR_logo.png");
        mPlaneRenderer.InitializePlaneGlContent();
        LOGI("WorldRenderManager-----Initialize() end.");
    }*/

    //void WorldRenderManager::Initialize(const Ar_Demo& gApp)
    void WorldRenderManager::Initialize()
    {
//        App=pDemo;

        LOGI("WorldRenderManager-----Initialize() start.");
        mBackgroundRenderer.InitializeBackGroundGlContent();
        mPointCloudRenderer.InitializePointCloudGlContent();
        mPlaneRenderer.InitializePlaneGlContent();
        LOGI("WorldRenderManager-----Initialize() end.");
    }

    void WorldRenderManager::OnDrawFrame(HwArSession *arSession, HwArFrame *arFrame,
                                         const std::vector<ColoredAnchor> &coloredAnchors,
                                         std::shared_ptr<RectTexture> ptr)
    {
        rect_render_=ptr;
        glm::mat4 viewMat;
        glm::mat4 projectionMat;
        // If the initialization fails, AR scene rendering is not performed.
        if (!InitializeDraw(arSession, arFrame, &viewMat, &projectionMat)) {
            return;
        }
        RenderObject(arSession, arFrame, viewMat, projectionMat, coloredAnchors, ptr);
        RenderPlanes(arSession, viewMat, projectionMat, ptr);
        RenderPointCloud(arSession, arFrame, viewMat, projectionMat, ptr);
    }

    bool WorldRenderManager::InitializeDraw(HwArSession *arSession,
                                            HwArFrame *arFrame,
                                            glm::mat4 *viewMat,
                                            glm::mat4 *projectionMat)
    {
        // Render the scene.
        glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (arSession == nullptr) {
            return false;
        }

        HwArSession_setCameraTextureName(arSession, mBackgroundRenderer.GetTextureId());

        // HwArSession update: Obtains the latest HwArFrame.
        if (HwArSession_update(arSession, arFrame) != HWAR_SUCCESS) {
            LOGE("WorldRenderManager::InitializeDraw ArSession_update error");
        }

        HwArCamera *arCamera = nullptr;
        HwArFrame_acquireCamera(arSession, arFrame, &arCamera);

        HwArCamera_getViewMatrix(arSession, arCamera, glm::value_ptr(*viewMat));

        // Near (0.1) Far (100).
        HwArCamera_getProjectionMatrix(arSession, arCamera, 0.1f, 100.f, glm::value_ptr(*projectionMat));
        HwArTrackingState cameraTrackingState = HWAR_TRACKING_STATE_STOPPED;
        HwArCamera_getTrackingState(arSession, arCamera, &cameraTrackingState);
        HwArCamera_release(arCamera);

        mBackgroundRenderer.Draw(arSession, arFrame);

        // If the camera is not in tracking state, the current frame is not drawn.
        return !(cameraTrackingState != HWAR_TRACKING_STATE_TRACKING);
    }

    void WorldRenderManager::RenderPointCloud(HwArSession *arSession, HwArFrame *arFrame,
                                              const glm::mat4 &viewMat,
                                              const glm::mat4 &projectionMat,
                                              std::shared_ptr<RectTexture> ptr)
    {
        // Update and render the point cloud.
        HwArPointCloud *arPointCloud = nullptr;

        HwArStatus pointCloudStatus = HwArFrame_acquirePointCloud(arSession, arFrame, &arPointCloud);
        if (pointCloudStatus == HWAR_SUCCESS) {
            mPointCloudRenderer.Draw(projectionMat * viewMat, arSession, arPointCloud, ptr);
            HwArPointCloud_release(arPointCloud);
        }
    }

    void WorldRenderManager::RenderObject(HwArSession *arSession, HwArFrame *arFrame,
                                          const glm::mat4 &viewMat,
                                          const glm::mat4 &projectionMat,
                                          const std::vector<ColoredAnchor> &mColoredAnchors,
                                          std::shared_ptr<RectTexture> ptr)
    {
        // Obtain the estimated lighting.
        HwArLightEstimate *arLightEstimate = nullptr;
        HwArLightEstimateState arLightEstimateState = HWAR_LIGHT_ESTIMATE_STATE_NOT_VALID;
        HwArLightEstimate_create(arSession, &arLightEstimate);

        HwArFrame_getLightEstimate(arSession, arFrame, arLightEstimate);
        HwArLightEstimate_getState(arSession, arLightEstimate, &arLightEstimateState);

        // Set the lighting intensity. The value range from 0.0f to 1.0f.
        float lightIntensity = 0.8f;
        if (arLightEstimateState == HWAR_LIGHT_ESTIMATE_STATE_VALID) {
            HwArLightEstimate_getPixelIntensity(arSession, arLightEstimate, &lightIntensity);
        }

        HwArLightEstimate_destroy(arLightEstimate);
        arLightEstimate = nullptr;

        // Initialize the model matrix.
        glm::mat4 modelMat(1.0f);
        for (const auto &coloredAnchor :mColoredAnchors) {
            HwArTrackingState trackingState = HWAR_TRACKING_STATE_STOPPED;
            HwArAnchor_getTrackingState(arSession, coloredAnchor.anchor, &trackingState);
            if (trackingState == HWAR_TRACKING_STATE_TRACKING) {
                LOGI("WorldRenderManager::RenderObject RenderObject is HWAR_TRACKING_STATE_TRACKING!");
                // Draw a virtual object only when the tracking status is AR_TRACKING_STATE_TRACKING.
                util::GetTransformMatrixFromAnchor(arSession, coloredAnchor.anchor, &modelMat);

                // The size of the drawn virtual object is 0.2 times the actual size.
                modelMat = glm::scale(modelMat, glm::vec3(0.2f, 0.2f, 0.2f));
                mObjectRenderer.Draw(projectionMat, viewMat, modelMat, lightIntensity,
                                     coloredAnchor.color, ptr);
            }
        }
    }

    void WorldRenderManager::RenderPlanes(HwArSession *arSession, const glm::mat4 &viewMat,
                                          const glm::mat4 &projectionMat,
                                          std::shared_ptr<RectTexture> ptr)
    {

        // Update and render the plane.
        HwArTrackableList *planeList = nullptr;
        HwArTrackableList_create(arSession, &planeList);
        CHECK(planeList != nullptr);

        HwArTrackableType planeTrackedType = HWAR_TRACKABLE_PLANE;
        HwArSession_getAllTrackables(arSession, planeTrackedType, planeList);

        int32_t planeListSize = 0;
        HwArTrackableList_getSize(arSession, planeList, &planeListSize);
        mPlaneCount = planeListSize;

        for (int i = 0; i < planeListSize; ++i) {
            HwArTrackable *arTrackable = nullptr;
            HwArTrackableList_acquireItem(arSession, planeList, i, &arTrackable);
            HwArPlane *arPlane = HwArAsPlane(arTrackable);
            HwArTrackingState outTrackingState;
            HwArTrackable_getTrackingState(arSession, arTrackable,
                &outTrackingState);

            HwArPlane *subsumePlane = nullptr;
            HwArPlane_acquireSubsumedBy(arSession, arPlane, &subsumePlane);

            if (subsumePlane != nullptr) {
                HwArTrackable_release(HwArAsTrackable(subsumePlane));
                continue;
            }

            if (HwArTrackingState::HWAR_TRACKING_STATE_TRACKING != outTrackingState) {
                continue;
            }

            HwArTrackingState planeTrackingState;
            HwArTrackable_getTrackingState(arSession, HwArAsTrackable(arPlane),
                &planeTrackingState);
            if (planeTrackingState == HWAR_TRACKING_STATE_TRACKING) {
                glm::vec3 color;
                RendererPlane(arPlane, arTrackable, color);
                mPlaneRenderer.Draw(projectionMat, viewMat, arSession, arPlane, color, ptr);
            }
        }

        HwArTrackableList_destroy(planeList);
        planeList = nullptr;
    }

    void WorldRenderManager::RendererPlane(HwArPlane *arPlane, HwArTrackable *arTrackable, glm::vec3 &color)
    {
        const auto iter = mPlaneColorMap.find(arPlane);
        if (iter != mPlaneColorMap.end()) {
            color = iter->second;

            HwArTrackable_release(arTrackable);
        } else {
            // Set the plane color. The first plane is white, and the other planes are blue.
            if (!firstPlaneHasBeenFound) {
                firstPlaneHasBeenFound = true;
                color = {255, 255, 255};
            } else {
                color = {0, 206, 209};
            }
            mPlaneColorMap.insert(std::pair<HwArPlane *, glm::vec3>(arPlane, color));
        }
    }

    bool WorldRenderManager::HasDetectedPlanes()
    {
        return mPlaneCount > 0;
    }

}