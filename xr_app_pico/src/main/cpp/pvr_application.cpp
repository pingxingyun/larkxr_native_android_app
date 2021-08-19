//
// Created by fcx on 2020/6/30.
//

#include <env_context.h>
#include <asset_loader.h>
#include <asset_files.h>
#include <log.h>
#include <lark_xr/xr_latency_collector.h>
#include <lark_xr/xr_config.h>
#include <unistd.h>
#include <utils.h>
#include "pvr_application.h"

//#define USE_RENDER_QUEUE = 1;

namespace {
    const int HEAD_SETY_TYPE_OTHER = 0;
    const static int HEAD_SETY_TYPE_NEO_2 = 1;
    const static int HEAD_SETY_TYPE_NEO_3 = 2;
}

PvrApplication* PvrApplication::s_pvr_instance_ = nullptr;

void PvrApplication::InitInstance() {
    LOGV("PvrApplication InitInstance");
    if (!s_pvr_instance_) {
        s_pvr_instance_ = new PvrApplication();
    }
    RegiseredInstance(s_pvr_instance_);
}

void PvrApplication::ReleaseInstance() {
    LOGV("PvrApplication ReleaseInstance");
    UnRegiseredInstance();
    delete s_pvr_instance_;
    s_pvr_instance_ = nullptr;
}

PvrApplication::PvrApplication() {
    LOGV("PvrApplication create");
}

PvrApplication::~PvrApplication() {
    LOGV("PvrApplication release");
    EnvWrapper envWrapper = Context::instance()->GetEnv();
    if (envWrapper.get() && pvr_sdk_object_) {
        envWrapper.get()->DeleteGlobalRef(pvr_sdk_object_);
    }
    if (envWrapper.get() && mainactivity_object_) {
        envWrapper.get()->DeleteGlobalRef(mainactivity_object_);
    }
}

void PvrApplication::InitJava(JavaVM *vm, jobject pvrSdk, jobject mainactivity_object, int type) {
    LOGV("PvrApplication InitJava");
    EnvWrapper envWrapper = Context::instance()->GetEnv();
    if (envWrapper.get()) {
        if (pvrSdk != nullptr) {
            pvr_sdk_object_ = envWrapper.get()->NewGlobalRef(pvrSdk);
        }
        if (mainactivity_object != nullptr) {
            mainactivity_object_ = envWrapper.get()->NewGlobalRef(mainactivity_object);
        }
    }

    lark::XRConfig::fps = 72;
    for (int eye = 0; eye < 2; eye++) {
        lark::XRConfig::fov[eye] = {
                101.0F * 0.5F, 101.0F * 0.5F, 101.0F * 0.5F, 101.0F * 0.5F
        };
    }

    lark::XRConfig::render_width = 2560;
    lark::XRConfig::render_height = 1440;
#ifdef USE_RENDER_QUEUE
    lark::XRConfig::use_render_queue = true;
#else
    lark::XRConfig::use_render_queue = false;
#endif
    lark::XRConfig::use_multiview = true;

    if (type == HEAD_SETY_TYPE_NEO_2) {
        lark::XRConfig::headset_desc.type = larkHeadSetType_PICO_2;
    } else if (type == HEAD_SETY_TYPE_NEO_3) {
        lark::XRConfig::headset_desc.type = larkHeadSetType_PICO_3;
    } else {
        lark::XRConfig::headset_desc.type = larkHeadSetType_HTC;
    }
//    lark::XRConfig::use_multiview = false;

    // 初始化客户端接入凭证
    InitCertificate();

    xr_client_.reset();
    xr_client_ = std::make_shared<lark::XRClient>();
    // gl context not ready. init share context later.
    // init_share_context should be false
    xr_client_->Init(vm, false);
    xr_client_->RegisterObserver(this);
#ifdef LARK_SDK_SECRET
    // 初始化 cloudlark sdk
    std::string timestamp = utils::GetTimestampMillStr();
    std::string signature = utils::GetSignature(LARK_SDK_ID, LARK_SDK_SECRET, timestamp);
    if (!xr_client_->InitSdkAuthorization(LARK_SDK_ID, signature, timestamp)) {
        LOGV("init sdk auth faild %d %s", xr_client_->last_error_code(), xr_client_->last_error_message().c_str());
        Navigation::ShowToast(xr_client_->last_error_message());
    }
#else
    if (!xr_client_->InitSdkAuthorization(LARK_SDK_ID)) {
        LOGV("init sdk auth faild %d %s", xr_client_->last_error_code(), xr_client_->last_error_message().c_str());
        Navigation::ShowToast(xr_client_->last_error_message());
    }
#endif
    ResetTracking(pvr::PvrTrackingOrigin_EyeLevel);
}

bool PvrApplication::InitGL(int eyeWidth, int eyeHeight) {
    LOGV("PvrApplication InitGL");
    // init lark opengl share context
    xr_client_->InitGLShareContext();

    //打开抗锯齿功能
    //1.开启混合功能
    glEnable(GL_BLEND);
    //配置混合方程式，默认为 GL_FUNC_ADD 方程
    glBlendEquation(GL_FUNC_ADD);
    //2.指定混合因子
    //注意:如果你修改了混合方程式,当你使用混合抗锯齿功能时,请一定要改为默认混合方程式
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(true);

    {
        // WARNING
        // pico may recreate gl context mutitimes.
        // must release old resource and recreate.
        // load assets.
        scene_local_.reset();
        scene_cloud_.reset();
        lark::AssetLoader::Release();
    }

    auto env = Context::instance()->GetEnv();
    lark::AndroidAssetContext context = {
            Context::instance()->native_activity(),
            env.get(),
            Context::instance()->bitmap_factory(),
    };
    lark::AssetLoader::instance()->Load(&context, Assetlist);

    // reset first to make sure release old point.
    scene_local_ = std::make_shared<PvrSceneLocal>();
    scene_local_->InitGl(eyeWidth, eyeHeight);

    scene_cloud_ = std::make_shared<PvrSceneCloud>();
    scene_cloud_->InitGl(eyeWidth, eyeHeight);

//    scene_cloud_.reset();

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    // setup
    lark::XRConfig::render_width = eyeWidth * 2;
    lark::XRConfig::render_height = eyeHeight;

    // WARNING
    // call resume on init gl
    // gl not ready on resume call. must call here
    if (xr_client_) {
        xr_client_->OnResume();
    }
    return true;
}

void PvrApplication::ShutdownGL() {
    LOGD("ShutdownGL");
    // release cloudlark
    xr_client_->UnRegisterObserver();
    xr_client_->Release();
    xr_client_.reset();

    // reset all state.
    Input::ResetInput();
    Navigation::ClearToast();

    scene_local_.reset();
    scene_cloud_.reset();
    lark::AssetLoader::Release();
    LOGD("ShutdownGL finished");
}

void PvrApplication::deInitGL() {
    LOGD("deInitGL");
    // release cloudlark
    // reset all state.
    Input::ResetInput();
    Navigation::ClearToast();

    // WARNING
    // pico may recreate gl context mutitimes.
    // must release old resource and recreate.
    // load assets.
    scene_local_.reset();
    scene_cloud_.reset();
    lark::AssetLoader::Release();
    LOGD("deInitGL finished");
}

void PvrApplication::FrameBegin(const pvr::PvrPose& pose) {
    if (!xr_client_) {
        return;
    }

    if (connected_) {
        scene_cloud_->UpdateHMDPose(pose.rotation, pose.position);
    } else {
        scene_local_->UpdateHMDPose(pose.rotation, pose.position);
    }
    hmd_pose_ = pose;
#ifdef USE_RENDER_QUEUE
    Poco::Timestamp timestamp;
    lark::XRVideoFrame xrVideoFrame(0);
    do {
        if (!connected_) {
            break;
        }
        if (xr_client_->Render(&cloud_tracking_, &xrVideoFrame)) {
            scene_cloud_->SetVideoFrame(xrVideoFrame);
            has_new_frame_ = true;
        } else {
            if (xr_client_->media_ready()) {
                Poco::Timestamp::TimeDiff elapsed =  timestamp.elapsed();
                if (elapsed > 100 * 1000) {
                    LOGW("wait for new frame timeout. skip. %ld", elapsed);
                    break;
                }
                usleep(1);
                continue;
            } else {
                break;
            }
        }
    } while (true);
#else
    if (xr_client_->media_ready()) {
        timeval timestamp{};
        gettimeofday(&timestamp, nullptr);
        do {
            if (!connected_) {
                break;
            }
            if (xr_client_->HasNewFrame()) {
                has_new_frame_ = xr_client_->Render(&cloud_tracking_);
                break;
            }
            timeval now{};
            gettimeofday(&now, nullptr);
            uint64_t elapsed = timeval_subtract_millsecond(now, timestamp);
            if (elapsed > 100) {
                LOGW("wait for new frame timeout. skip. %ld", elapsed);
                break;
            }
            usleep(1);
        } while (true);
    }
#endif


    if (has_new_frame_) {
        lark::XRLatencyCollector::Instance().Rendered2(cloud_tracking_.frameIndex);
        glm::vec3 renderAng = glm::eulerAngles(cloud_tracking_.tracking.rotation);
        glm::vec3 trackingAng = glm::eulerAngles(hmd_pose_.rotation);
        float degree = glm::degrees(renderAng.y - trackingAng.y);
        lark::XRLatencyCollector::Instance().Submit(cloud_tracking_.frameIndex, degree);
    }

#if 0
    larkxrDevicePair devicePair = {};
    devicePair.hmdPose = pvr::toLarkHMDTrakedPose(hmd_pose_);
    for (int i = 0; i < LARKVR_TOTAL_CONTROLLER_COUNT; i++) {
        devicePair.controllerState[i] = pvr::toLarkxrControllerState(controller_[i]);
    }
    larkxrTrackingDevicePairFrame devicePairFrame = {
            0, 0, 0,
            devicePair,
    };
    xr_client_.SendDevicePair(devicePairFrame);
#endif
}

void PvrApplication::FrameEnd() {
#ifdef USE_RENDER_QUEUE
    if (xr_client_ && has_new_frame_) {
        xr_client_->ReleaseRenderTexture();
        has_new_frame_ = false;
    }
#else
#endif
}

void PvrApplication::Draw(int eye) {
    if (!xr_client_) {
        return;
    }
    if (connected_) {
        scene_cloud_->Draw(eye);
    } else {
        scene_local_->Draw(eye);
    }
}

void PvrApplication::OnResume() {
}

void PvrApplication::OnPause() {
    if (xr_client_) {
        // must releae gl share context for pico sdk has release old
        xr_client_->ReleaseGLShareContext();
        xr_client_->OnPause();
    }
}

void PvrApplication::OnDestory() {
    if (xr_client_) {
        xr_client_->OnDestory();
    }
}

void PvrApplication::UpdateControler(const pvr::PvrControllerState& controllerState) {
    controller_[controllerState.type] = controllerState;
}

void PvrApplication::EnterAppli(const std::string &appId) {
    LOGV("on enter appli");
    if (xr_client_) {
        xr_client_->EnterAppli(appId);
    }
}

void PvrApplication::CloseAppli() {
    LOGV("on close appli");
    if (xr_client_) {
        xr_client_->Close();
    }
}

// callbacks
void PvrApplication::OnConnected() {
    LOGV("=========on connected");
    connected_ = true;
    scene_cloud_->OnConnect();
}

void PvrApplication::OnClose(int code) {
    LOGV("=========on close %d", code);
    connected_ = false;
    has_new_frame_ = false;
    cloud_tracking_ = {};
    scene_cloud_->OnClose();
    scene_local_->HomePage();
    switch(code) {
        case LK_XR_MEDIA_TRANSPORT_CHANNEL_CLOSED:
            Navigation::ShowToast("与服务器媒体连接关闭");
            break;
        case LK_RENDER_SERVER_CLOSE:
            Navigation::ShowToast("与渲染服务器 TCP 连接关闭");
            break;
        case LK_PROXY_SERVER_CLOSE:
            Navigation::ShowToast("与渲染服务器代理连接关闭");
            break;
    }
    ResetTracking(pvr::PvrTrackingOrigin_EyeLevel);
}

void PvrApplication::OnError(int errCode, const std::string &msg) {
    LOGE("on xr client error %d; msg %s;", errCode, msg.c_str());
    if (errCode == 1) {
        // enter applifailed.
        scene_local_->HomePage();
    } else {
        connected_ = false;
        scene_local_->HomePage();
    }
    Navigation::ShowToast(msg);
    ResetTracking(pvr::PvrTrackingOrigin_EyeLevel);
}

void PvrApplication::RequestTrackingInfo() {
#if 1
    pvr::PvrPose hmdPose = {};
    bool res = pvr::getHmdPose(pvr_sdk_object_, hmdPose);
    if (!res) {
        return;
    }
    larkxrDevicePair devicePair = {};
    devicePair.hmdPose = pvr::toLarkHMDTrakedPose(hmdPose);
    for (int i = 0; i < LARKVR_TOTAL_CONTROLLER_COUNT; i++) {
        devicePair.controllerState[i] = pvr::toLarkxrControllerState(controller_[i]);
        // rotate pico neo2 controller
        if (lark::XRConfig::headset_desc.type == larkHeadSetType_PICO_2) {
            devicePair.controllerState[i].rotateDeg = glm::half_pi<float>() / 3.0F;
            devicePair.controllerState[i].rotateAxis = glm::vec3(-1, 0, 0);
        }
    }
    larkxrTrackingDevicePairFrame devicePairFrame = {
            0, 0, 0,
            devicePair,
    };
    xr_client_->SendDevicePair(devicePairFrame);
#endif
}

void PvrApplication::OnMediaReady(int nativeTextrure) {
    LOGV("on media ready TLR%d", nativeTextrure);
    scene_cloud_->OnMediaReady(nativeTextrure);
    ResetTracking(pvr::PvrTrackingOrigin_FloorLevel);
}

void PvrApplication::OnMediaReady(int nativeTextureLeft, int nativeTextureRight) {
    LOGV("on media ready L%d R%d", nativeTextureLeft, nativeTextureRight);
    scene_cloud_->OnMediaReady(nativeTextureLeft, nativeTextureRight);
    ResetTracking(pvr::PvrTrackingOrigin_FloorLevel);
}

void PvrApplication::OnMediaReady() {
    LOGV("on media ready");
    scene_cloud_->OnMediaReady();
    ResetTracking(pvr::PvrTrackingOrigin_FloorLevel);
}

void
PvrApplication::OnHapticsFeedback(bool isLeft, uint64_t startTime, float amplitude, float duration,
                                  float frequency) {
    Application::OnHapticsFeedback(isLeft, startTime, amplitude, duration, frequency);
    LOGV("haptics %f %f", amplitude, duration);
    if (mainactivity_object_ == nullptr) {
        return;
    }
    EnvWrapper envWrapper = Context::instance()->GetEnv();
    JNIEnv* env = envWrapper.get();
    if (env == nullptr) {
        return;
    }
    jclass clazz = env->GetObjectClass(mainactivity_object_);
    jmethodID mid = env->GetMethodID(clazz, "onHapticsFeedback", "(ZFFF)V");
    env->CallVoidMethod(mainactivity_object_, mid, isLeft, amplitude, duration, frequency);
}

bool PvrApplication::GetRenderPose(pvr::PvrPose& pose) {
    if (!connected_ || cloud_tracking_.frameIndex == 0) {
        return false;
    }
    pose = pvr::fromLarkTrackedPose(cloud_tracking_.tracking);
    return true;
}

void PvrApplication::OnSyncPlayerSpace(larkxrPlaySpace *playSpace) {
}

void PvrApplication::ResetTracking(pvr::PvrTrackingOrigin origin) {
    bool res = pvr::setTrackingOriginType(pvr_sdk_object_, origin);
    LOGV("ResetTracking %d", res);
}