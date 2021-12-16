
#ifndef XRDEMO_H
#define XRDEMO_H

#define XR_USE_PLATFORM_ANDROID
#define XR_USE_GRAPHICS_API_OPENGL_ES
#include <android/native_window_jni.h>
#include <jni.h>
#include <EGL/egl.h>
#include <pthread.h>

#include "openxr/openxr.h"
#include "openxr/openxr_platform.h"
#include "model.h"
#include "shader.h"
#include "texture.h"
#include <thread>
#include <time.h>
#include <application.h>
#include <rect_texture.h>

enum AppState{
    ON_RESUME,
    ON_PAUSE,
    ON_DESTROY
};
class XrDemo : public Application
{
public:
    virtual bool InitVR(android_app* app) override;
    virtual bool InitVR() override { return false; };
    virtual void InitJava() override;
    virtual bool InitGL() override;
    virtual void ShutdownVR() override;
    virtual void ShutdownGL() override;

    //  callback from android native activity.
    virtual void HandleVrModeChange() override;
    virtual bool OnUpdate() override;
    // 进入应用
    virtual void EnterAppli(const std::string& appId) override;
    virtual void CloseAppli() override;

    ANativeWindow *	native_window_ = nullptr;
    int nativeTextrureFromMedia;
    int nativeTextrureFromMediaLeft;
    int nativeTextrureFromMediaRight;
    std::shared_ptr<RectTexture> rect_render_ = nullptr;

    larkxrTrackingDevicePairFrame mTrackingStateS[2]={};

public:
    // andoird lifecycle
    void OnResume();
    void OnPause();
    void OnInitWindow(ANativeWindow * window);
    void OnDestory();

public:
    XrDemo(JavaVM* _vm, jobject act,JNIEnv* _Env);
	~XrDemo();

    //Set Native window surface
    void setSurface(JNIEnv* jni, jobject surface);
    void initSkyboxMethod(JNIEnv* jni);

    void start();
    void stop();

    double GetSeconds();
    double GetNanos();

    // hw decoder callback textrue.
    virtual void OnMediaReady() override;
    virtual void OnMediaReady(int nativeTexture) override;
    virtual void OnMediaReady(int nativeTextureLeft, int nativeTextureRight) override;
    virtual void RequestTrackingInfo() override;
    virtual void OnTrackingFrame(const larkxrTrackingFrame& trackingFrame) override;

public:
    bool Run();
    void XrDemoBeginSession();

    //OpenGLESGraphics
    void InitGLGraphics();
    void DeInitGLGraphics();
    Shader mSkyboxShader;
    Model mSkyboxModel;
    Texture mSkyboxTexture;

    //OpenXR
    void CreateInstance();
    void CreateActions();
    void InitializeSystem();
    void InitializeSession();
    void ProcessEvents(bool* exitRenderLoop, bool* sessionRunning);
    void PollActions();

    void RenderFrame();
    void RenderLayer(XrTime time, XrCompositionLayerProjection& layer);
    void renderView(const XrCompositionLayerProjectionView &layerView,
                    const XrSwapchainImageBaseHeader *swapchainImage, const XrPosef &ctlPose,
                    int i);
    void DeInitializeSession();
    void DestroyActions();
    void DestroyInstance();
    bool initEGLSurface();
    XrInstance mInstance;
    XrSystemId mSystemId;
    XrSession mSession;
    XrSpace mAppSpace;
    //add
    XrActionSet m_actionSet;
    XrAction m_XrAction[13];
    XrPath LRPath[2];
    XrPath headPath;
    XrSpace LRSpace[2];
    XrPath HomePath[2];
    XrPath BackPath[2];
    XrPath VolumeUpPath[2];
    XrPath VolumeDownPath[2];
    XrPath TrackpadClickPath[2];
    XrPath TrackpadTouchPath[2];
    XrPath TriggerClickPath[2];
    XrPath TriggerValuePath[2];
    XrPath TrackpadValuePath[2];
    XrPath TrackpadXValuePath[2];
    XrPath TrackpadYValuePath[2];
    XrPath PoseValuePath[2];
    XrPath HapticPath[2];
    XrPath khrSimpleInteractionProfilePath;
    XrPath huaweiInteractionProfilePath;

    std::vector<XrSwapchain> mSwapchains;
    std::vector<std::vector<XrSwapchainImageOpenGLESKHR>> mSwapchainsImageArray;
    std::thread *pollActionLoop = nullptr;
    //Jvm & Activity
    jobject mActivity;
    JavaVM* mJvm;
    JNIEnv*	Env;

    //Native window surface
    ANativeWindow* mNativeWindow;

    //Egl Environment
    EGLConfig mConfig;
    EGLContext mContext;
    EGLDisplay mDisplay;

    //time
    double mStart;
    double mFinish;

    //Instance State
    bool mInstanceLossState;
    double mInstanceLossStateTime;
private:

    static void* threadStartCallback(void *myself);
    pthread_t _threadId;

};

#endif //XRDEMO_H
