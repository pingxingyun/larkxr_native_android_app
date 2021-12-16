/*
 * Copyright (c) 2019-2020, NVIDIA CORPORATION. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef __CLOUDXR_CLIENT_H__
#define __CLOUDXR_CLIENT_H__

#include "CloudXRCommon.h"
#include "CloudXRInputEvents.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup groupStruct
 *  @{
 */

typedef struct cxrReceiver* cxrReceiverHandle;

typedef struct cxrClientCallbacks
{
    // Requests the HMD and controller states and poses.
    void (*GetTrackingState)(void* context, cxrVRTrackingState* trackingState);

    // Notifies the client app to trigger a controller vibration.
    void (*TriggerHaptic)(void* context, const cxrHapticFeedback* haptic);

    // Delivers an audio buffer for playback, application should return false if
    // it chooses to discard the data otherwise it must pace the callback at realtime
    cxrBool (*RenderAudio)(void* context, const cxrAudioFrame* audioFrame);

    // Delivers custom user data from a server application.
    void (*ReceiveUserData)(void* context, const void* data, uint32_t size);

    // Client lib will send connection status to app with this callback
    void (*UpdateClientState)(void* context, cxrClientState state, cxrStateReason reason);
} cxrClientCallbacks;

/**
 * The receiver descriptor.
 * TO DO:  describe structure
 */
 typedef struct cxrReceiverDesc
{
    uint32_t requestedVersion;              ///< Set to CLOUDXR_VERSION_DWORD.

    cxrDeviceDesc deviceDesc;               ///< Describes the client device hardware.
    cxrClientCallbacks clientCallbacks;     ///< Pointers to the client callbacks.
    void* clientContext;                    ///< User data passed back to the callback functions.
    const cxrGraphicsContext* shareContext; ///< The client graphics context to be shared with CloudXR.
    
    uint32_t numStreams;                    ///< Number of video streams to open, typically one per eye.

    cxrStreamingMode receiverMode;          ///< Generic (up to 6 streams) or XR streaming.

    uint32_t debugFlags;                    ///< Bitfields to aid debugging - see the cxrDebugFlags enum.
    int32_t logMaxSizeKB;                   ///< Maximum size for the client log in KB. -1 for default, 0 to disable size limit.
    int32_t logMaxAgeDays;                  ///< Delete log and diagnostic files older than this. -1 for default, 0 to disable.
} cxrReceiverDesc;


/** @} */ // end of groupStruct


/** @defgroup groupFunc
 *  @{
 */

// A typical main loop looks like this:
//
// - @ref cxrCreateReceiver()
// - @ref cxrConnect()
// - @ref cxrSendChaperone()
// - while (connected & !quitting)
//   + @ref cxrLatchFrameXR()
//   + @ref cxrBlitFrame() // Called per eye, Android only.
//   + @ref cxrReleaseFrame()
//   + In parallel, handle the async client callbacks
// - @ref cxrDestroyReceiver()


/**
 * Initialize the CloudXR library and create a receiver handle to be used with all other APIs.
 * @param[in]  description  Pointer to cxrReceiverDesc.
 * @param[out] receiver     Pointer to cxrReceiverHandle.  This handle must be passed to other APIs.
 */
CLOUDXR_PUBLIC_API cxrError cxrCreateReceiver(const cxrReceiverDesc* description, cxrReceiverHandle* receiver);

/**
 * Establish a connection to a CloudXR server.
 * @param[in] receiver   cxrReceiverHandle handle that was obtained from cxrCreateReceiver().
 * @param[in] serverAddr IP address of CloudXR server.
 * @param[in] connectionFlags Optional flags that control connection behavior (such as async connection)
 */
CLOUDXR_PUBLIC_API cxrError cxrConnect(cxrReceiverHandle receiver, const char* serverAddr, uint64_t connectionFlags);

// Terminate a streaming session and free the associated CloudXR resources.
CLOUDXR_PUBLIC_API void cxrDestroyReceiver(cxrReceiverHandle handle);


// Acquire the next available decoded video frames from network stream
CLOUDXR_PUBLIC_API cxrError cxrLatchFrame(cxrReceiverHandle handle, cxrFramesLatched* framesLatched, uint32_t frameMask, uint32_t timeoutMs);

#if defined(ANDROID)
// Render the latched video frame to the currently bound target surface.
CLOUDXR_PUBLIC_API cxrBool cxrBlitFrame(cxrReceiverHandle handle, cxrFramesLatched* framesLatched, uint32_t frameMask);
#endif

// Release a previously latched set of video frames from cxrLatchFrame.
CLOUDXR_PUBLIC_API cxrError cxrReleaseFrame(cxrReceiverHandle handle, cxrFramesLatched* framesLatched);



// Provide estimated world lighting properties to the server, such as ARCore's lighting estimation.
CLOUDXR_PUBLIC_API void cxrSendLightProperties(cxrReceiverHandle handle, const cxrLightProperties* lightProps);

// Send non-VR-controller input events to the server for app-specific handling.
CLOUDXR_PUBLIC_API void cxrSendInputEvent(cxrReceiverHandle handle, const cxrInputEvent* inputEvent);

// Send client input audio (i.e. microphone) to the server.
CLOUDXR_PUBLIC_API void cxrSendAudio(cxrReceiverHandle handle, const cxrAudioFrame* audioFrame);

/** @} */ // end of groupFunc

#ifdef __cplusplus
}
#endif

#endif
