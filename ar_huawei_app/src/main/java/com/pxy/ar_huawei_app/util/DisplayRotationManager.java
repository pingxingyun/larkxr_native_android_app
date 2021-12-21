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

package com.pxy.ar_huawei_app.util;

import android.content.Context;
import android.hardware.display.DisplayManager;
import android.hardware.display.DisplayManager.DisplayListener;
import android.util.Log;
import android.view.Display;
import android.view.WindowManager;

import androidx.annotation.NonNull;

import com.pxy.ar_huawei_app.JniInterface;

/**
 * Device rotation manager, which is used by the demo to adapt to device rotations
 *
 * @author HW
 * @since 2020-09-20
 */
public class DisplayRotationManager implements DisplayListener {
    private static final String TAG = DisplayRotationManager.class.getSimpleName();

    private boolean mIsDeviceRotation;

    private final Context mContext;

    private final Display mDisplay;

    private int mViewPx;

    private int mViewPy;

    /**
     * Use the context to construct DisplayRotationManage.
     *
     * @param context Context.
     */
    public DisplayRotationManager(@NonNull Context context) {
        mContext = context;
        WindowManager systemService = mContext.getSystemService(WindowManager.class);
        if (systemService != null) {
            mDisplay = systemService.getDefaultDisplay();
        } else {
            mDisplay = null;
        }
    }

    /**
     * Register a listener on display changes. Call this method when onResume is called.
     */
    public void registerDisplayListener() {
        DisplayManager systemService = mContext.getSystemService(DisplayManager.class);
        if (systemService != null) {
            systemService.registerDisplayListener(this, null);
        }
    }

    /**
     * Unregister the listener on display changes. Call this method when onPause is called.
     */
    public void unregisterDisplayListener() {
        DisplayManager systemService = mContext.getSystemService(DisplayManager.class);
        if (systemService != null) {
            systemService.unregisterDisplayListener(this);
        }
    }

    /**
     * When the device is rotated, the size of the viewfinder and whether the viewfinder is rotated need
	 * to be updated to correctly display the geometric information returned by AR Engine.
     * Call this method onSurfaceChanged.
     *
     * @param width Set the width of the updated surface.
     * @param height Set the height of the updated surface.
     */
    public void updateViewportRotation(int width, int height) {
        mViewPx = width;
        mViewPy = height;
        mIsDeviceRotation = true;
    }

    /**
     * Check whether the current device is rotated.
     *
     * @return Device rotation result.
     */
    public boolean getDeviceRotation() {
        return mIsDeviceRotation;
    }

    /**
     * If the device is rotated, update the device window of the current ARSession.
     * Call this method when calling onDrawFrame.
     *
     * @param nativeApplication nativeApplication
     */
    public void updateArSessionDisplayGeometry(long nativeApplication) {
        int displayRotation = 0;
        if (mDisplay != null) {
            displayRotation = mDisplay.getRotation();
        } else {
            Log.e(TAG, "updateArSessionDisplayGeometry mDisplay null!");
        }

        JniInterface.onDisplayGeometryChanged(nativeApplication, displayRotation, mViewPx, mViewPy);

        mIsDeviceRotation = false;
    }

    @Override
    public void onDisplayAdded(int displayId) {
    }

    @Override
    public void onDisplayRemoved(int displayId) {
    }

    @Override
    public void onDisplayChanged(int displayId) {
        mIsDeviceRotation = true;
    }
}