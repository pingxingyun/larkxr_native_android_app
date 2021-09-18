//
// Created by Admin on 2021/7/1.
//

#include <lark_xr/xr_config.h>
#include <env_context.h>
#include <asset_files.h>
#include <utils.h>
#include "xvisio_vr_utils.h"
#include "larkxr_client.h"

// extra room height add to position.y.
#define ROOM_HEIGHT 1.5f

LarkxrClient::LarkxrClient() {
    LOGV("");
    Application::RegiseredInstance(this);
}

LarkxrClient::~LarkxrClient() {
    LOGV("");
    Application::UnRegiseredInstance();
}

void LarkxrClient::InitJava(JavaVM *vm) {
    // NVR_GetNVRConfig
    // fov: 45.222481,46.188538,45.709682,45.709682, near=0.029500, far=1000.000000,
    // screenSize=0.120960,0.060480, eyeIpd=0.061500.[1600.000000X1600.000000].0xe0521900
    // NVR_GetViewParameters FBO [1600 * 1600]
    // NVR Config is [0.128, 0.036, 3840.0, 1080.0, 1920.0, 1080.0, 1920.0, 1080.0, 4.0, 0.064, 0.0806, 1.7777778,
    // 1.7777778, 0.0806, 100.0, 20.0, 20.0, -0.03, -0.02, 0.064
    // , 0.0, 0.0, -1.0, 0.01, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0]
    //
    xr_client_ = std::make_shared<XRClient>();
    // 只初始化 java 环境不初始化 gl 环境。
    // gl 环境可能随着休眠和启动重复创建。
    xr_client_->Init(vm, false);
    xr_client_->RegisterObserver(this);
//    app_list_task_ = std::make_shared<AppListTask>(this, false);

    //
    // 诠视二次开发授权码,注意保留
    //
//    if (!xr_client_->InitSdkAuthorization("e6501123e1204ece99fe28cd5938407e")) {
//        LOGE("init sdk auth faild %d %s", xr_client_->last_error_code(),
//             xr_client_->last_error_message().c_str());
//    }
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
    // 测试的服务器地址
//     xr_client_->SetServerAddr("192.168.0.50", 8181);
}

bool LarkxrClient::InitGL() {
    if (!xr_client_) {
        return false;
    }

    // gl 环境可能随着休眠和启动重复创建。
    // gl 环境释放时调用 ReleaseGLShareContext 释放环境。
    xr_client_->InitGLShareContext();
    lark::XRConfig::flip_draw = false;
    lark::XRConfig::use_multiview = true;

    // ipd
    if (nxr_sensor_client_) {
        NxrDisplayCalibration displayCalibration {};
        nxr_sensor_client_->readDisplayCalibration(&displayCalibration);
        float eye_distance = glm::distance(glm::vec3(displayCalibration.translation[0][0], displayCalibration.translation[0][1], displayCalibration.translation[0][2]),
                                           glm::vec3(displayCalibration.translation[1][0], displayCalibration.translation[1][1], displayCalibration.translation[1][2]));
        LOGV("eye distance %f", eye_distance);
        // 0.060883f
        lark::XRConfig::ipd = eye_distance;
    }

    // details in doc folder.
    // preject matrix from unity.
    // left
    // near 0.3 far 1000
    // cam.projectionMatrix = PerspectiveOffCenter(
    // 2049.967081982467f, 2059.332355445469f, 648.1342214361877f, 347.5380574331058f, 1280f, 720f,
    // cam.nearClipPlane, cam.farClipPlane);
    // right
    // near 0.3 far 1000
    // Internal reference setting of glasses (right camera)
    // cam.projectionMatrix = PerspectiveOffCenter(
    // 2052.973221775336f, 2062.194471182466f, 644.7759307379876f, 335.8633168270726f, 1280f, 720f,
    // cam.nearClipPlane, cam.farClipPlane);

    lark::XRConfig::fov[0] = xvisio::PerspectiveOffCenter(0, 2049.967081982467f, 2059.332355445469f, 648.1342214361877f, 347.5380574331058f,
                                 1280.0f, 720.0f, 0.3, 1000);
    lark::XRConfig::fov[1] = xvisio::PerspectiveOffCenter(1, 2052.973221775336f, 2062.194471182466f, 644.7759307379876f, 335.8633168270726f,
                                 1280.0f, 720.0f, 0.3, 1000);
    lark::XRConfig::render_width = 1280 * 2;
    lark::XRConfig::render_height = 720;

    lark::XRConfig::fps = 60;
#ifdef USE_RENDER_QUEUE
    lark::XRConfig::use_render_queue = true;
#else
    lark::XRConfig::use_render_queue = false;
#endif
    lark::XRConfig::QuickConfigWithDefaulSetup(lark::QuickConfigLevel::QuickConfigLevel_Fast);
//    lark::XRConfig::bitrate = 40 * 1000;

//    lark::XRConfig::headset_desc.type = larkHeadSetType_NOLO_Sonic_1;
    lark::XRConfig::headset_desc.type = larkHeadSetType_OCULUS;

//    xrClient->SetServerAddr("192.168.1.192", 8181);
//    xr_client_->SetServerAddr("192.168.31.10", 8484);
//    xrClient->SetServerAddr("192.168.31.10", 8181);
//    app_list_task_->Start();
//    textureRect.Init();

    // init gl
    //打开抗锯齿功能
    //1.开启混合功能
    glEnable(GL_BLEND);
    //配置混合方程式，默认为 GL_FUNC_ADD 方程
    glBlendEquation(GL_FUNC_ADD);
    //2.指定混合因子
    //注意:如果你修改了混合方程式,当你使用混合抗锯齿功能时,请一定要改为默认混合方程式
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//    glEnable(GL_DEPTH_TEST);
//    glDepthFunc(GL_LEQUAL);
//    glDepthMask(true);
//    glCullFace(GL_BACK);
//    glEnable(GL_CULL_FACE);
//    glFrontFace(GL_CCW);

    auto env = Context::instance()->GetEnv();
    lark::AndroidAssetContext context = {
            Context::instance()->native_activity(),
            env.get(),
            Context::instance()->bitmap_factory(),
    };
    lark::AssetLoader::instance()->Load(&context, Assetlist);

    // defautl use hmd for input
    Input::SetCurrentRayCastType(Input::RayCast_Hmd);

    scene_local_ = std::make_shared<SceneLocal>();
    scene_local_->InitGl(this, 1920, 1080);

    rect_render_ = std::make_shared<RectTexture>();

    inited = true;
    XRClient::SetHmdBatteryLevel(100);
    return true;
}

void LarkxrClient::ShutdownGL() {
    if (xr_client_) {
        xr_client_->ReleaseGLShareContext();
    }
}

void LarkxrClient::OnCreate() {
    if (!inited)return;
    if (xr_client_) {
        xr_client_->OnCreated();
    }
}

void LarkxrClient::OnPause() {
    if (!inited)return;
    if (xr_client_) {
        xr_client_->OnPause();
    }
}

void LarkxrClient::OnResume() {
    if (!inited)return;
    if (xr_client_) {
        xr_client_->OnResume();
    }
}

void LarkxrClient::OnDestory() {
    if (!inited) return;
    if (xr_client_) {
        xr_client_->OnDestory();
        xr_client_->UnRegisterObserver();
        xr_client_->Release();
        xr_client_.reset();
    }
}

void LarkxrClient::Connect() {

}

void LarkxrClient::Close() {
    LOGV("关闭云端连接");
    if (xr_client_) {
        xr_client_->Close();
    }
}

void LarkxrClient::EnterAppli(const std::string &appliId) {
    LOGV("EnterAppli appliId:%s", appliId.c_str());
    if (xr_client_) {
        xr_client_->EnterAppli(appliId);
    }
}


void LarkxrClient::Update(const glm::quat& rotation, const glm::vec3& position) {
    UpdateHmd(rotation, position);
#ifdef USE_RENDER_QUEUE
    if (!xr_client_->is_connected()) {
        scene_local_->UpdateHMDPose(device_pair_.hmdPose.rotation, device_pair_.hmdPose.position);
        scene_local_->HandleInput(device_pair_);
    } else {
        HandlInput();
        has_new_frame_ = false;
        lark::XRVideoFrame xrVideoFrame(0);
        if (xr_client_->Render(&cloud_tracking_, &xrVideoFrame)) {
            has_new_frame_ = true;
            if (xrVideoFrame.frame_type() == lark::XRVideoFrame::FrameType::kNative_Multiview) {
                rect_render_->SetMutiviewModeTexture(xrVideoFrame.texture());
            } else if (xrVideoFrame.frame_type() == lark::XRVideoFrame::FrameType::kNative_Stereo) {
                rect_render_->SetStereoTexture(xrVideoFrame.texture_left(), xrVideoFrame.texture_right());
            }
        } else {
            if (!xr_client_->media_ready()) {
                scene_local_->UpdateHMDPose(device_pair_.hmdPose.rotation, device_pair_.hmdPose.position);
                scene_local_->HandleInput(device_pair_);
            }
        }
    }
#else
    if (!xr_client_->media_ready()) {
        scene_local_->UpdateHMDPose(device_pair_.hmdPose.rotation, device_pair_.hmdPose.position);
        scene_local_->HandleInput(device_pair_);
    } else {
        handlInput();
        has_new_frame_ = false;
        if (xr_client_->HasNewFrame()) {
            has_new_frame_ = xr_client_->Render(&cloud_tracking_);
        }
    }
#endif

}

void LarkxrClient::UpdateHmd(const glm::quat& rotation, const glm::vec3& position) {
//    LOGV("update hmd %f %f %f %d",
//         gsxr_poseData.pose.position.x, gsxr_poseData.pose.position.y, gsxr_poseData.pose.position.z, gsxr_poseData.pose_valid);

    larkxrTrackedPose larkxrTrackedPoseHmd = {};

    larkxrTrackedPoseHmd.device = Larkxr_Device_Type_HMD;
    larkxrTrackedPoseHmd.isConnected = true;
    larkxrTrackedPoseHmd.is6Dof = true;
    larkxrTrackedPoseHmd.isValidPose = true;

    // larkxrTrackedPoseHmd.rotation = glm::inverse(rotation);

//    larkxrTrackedPoseHmd.rotation = rotation;

    larkxrTrackedPoseHmd.rotation.x = rotation.x;
    larkxrTrackedPoseHmd.rotation.y = rotation.y;
    larkxrTrackedPoseHmd.rotation.z = rotation.z;
    larkxrTrackedPoseHmd.rotation.w = rotation.w;

    larkxrTrackedPoseHmd.position.x = position.x;
    // extra height
    larkxrTrackedPoseHmd.position.y = position.y + ROOM_HEIGHT;
    larkxrTrackedPoseHmd.position.z = position.z;

    device_pair_ = {};
    device_pair_.hmdPose = larkxrTrackedPoseHmd;

}

void LarkxrClient::HandlInput() {
    // 处理事件，关闭应用等
    bool backButtonDownThisFrame[Input::RayCast_Count] = {false, false};
    bool triggerDownThisFrame[Input::RayCast_Count] = {false, false};
    bool enterButtonDownThisFrame[Input::RayCast_Count] = {false, false};
    for (int i = 0; i < 2; i++) {
        Input::RayCastType rayCastType = i == 0 ? Input::RayCast_left : Input::RayCast_Right;
        backButtonDownThisFrame[rayCastType] |=
                device_pair_.controllerState[i].inputState.buttons & LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_B_Click);
        backButtonDownThisFrame[rayCastType] |=
                device_pair_.controllerState[i].inputState.buttons & LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Y_Click);
        backButtonDownThisFrame[rayCastType] |=
                device_pair_.controllerState[i].inputState.buttons & LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Application_Menu_Click);

        enterButtonDownThisFrame[rayCastType] |=
                device_pair_.controllerState[i].inputState.buttons & LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_X_Click);
        enterButtonDownThisFrame[rayCastType] |=
                device_pair_.controllerState[i].inputState.buttons & LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_A_Click);

        triggerDownThisFrame[rayCastType] |=
                device_pair_.controllerState[i].inputState.buttons & LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Trigger_Click);

        bool backButtonDownLastFrame = back_button_down_last_frame_[rayCastType];
        back_button_down_last_frame_[rayCastType] = backButtonDownThisFrame[rayCastType];

        bool traggerButtonDownLastFrame = trigger_button_down_last_frame_[rayCastType];
        trigger_button_down_last_frame_[rayCastType] = triggerDownThisFrame[rayCastType];

        bool enterButtonDownLastFrame = enter_button_down_last_frame_[rayCastType];
        enter_button_down_last_frame_[rayCastType] = enterButtonDownThisFrame[rayCastType];

        // 关闭连接
        if ( triggerDownThisFrame[rayCastType] && backButtonDownLastFrame && !backButtonDownThisFrame[rayCastType] ) {
            LOGV("close app." );
            CloseAppli();
        }
    }
}

void LarkxrClient::Draw(const nvr::nvr_Eye& eye) {
    if (xr_client_ == NULL) {
        LOGV("xrClient == NULL");
        return;
    }

    if (!xr_client_->media_ready()) {
        scene_local_->Draw(eye.pos);
    } else {
        if (has_new_frame_) {
            if (eye.pos == 0) {
                rect_render_->Draw(static_cast<Object::Eye>(eye.pos), glm::mat4(), glm::mat4());
//             textureRect.drawSelf(this->nativeTextrureLeft);
            } else {
                rect_render_->Draw(static_cast<Object::Eye>(eye.pos), glm::mat4(), glm::mat4());
//             textureRect.drawSelf(this->nativeTextrureRight);
            }
        }
    }
}

void LarkxrClient::Finish() {
    if (has_new_frame_) {
#ifdef USE_RENDER_QUEUE
        xr_client_->ReleaseRenderTexture();
#endif
//            LOGV("has_new_rame_ true");
        glm::quat hmdRotation = {};

//        mat4 hmd_pose_ = {};
//        glm::quat hmdRotation = {};
//        nibiruVrApi->nvr_GetHeadPose(&hmd_pose_);
//        matrixToQuat(hmd_pose_, hmdRotation);

        lark::XRLatencyCollector::Instance().Rendered2(cloud_tracking_.frameIndex);
        glm::vec3 renderAng = glm::eulerAngles(cloud_tracking_.tracking.rotation);
        glm::vec3 trackingAng = glm::eulerAngles(hmdRotation);
        float degree = glm::degrees(renderAng.y - trackingAng.y);
        lark::XRLatencyCollector::Instance().Submit(cloud_tracking_.frameIndex, degree);
    }
}


void LarkxrClient::OnSDKAuthorizationFailed(int code, const std::string &msg) {
    LOGE("SDKAuthorizationFailed");
}

void LarkxrClient::OnConnected() {
    LOGV("连接成功");
    connected = true;
//    app_list_task_->start();
}

void LarkxrClient::OnClose(int code) {
    LOGV("连接关闭");
//    app_list_task_->Stop();
    LOGV("=========on close %d", code);
    connected_ = false;
//    scene_cloud_->OnClose();
    scene_local_->HomePage();
    rect_render_->ClearTexture();
    switch(code) {
        case LK_XR_MEDIA_TRANSPORT_CHANNEL_CLOSED:
            Navigation::ShowToast("与服务器媒体传输通道连接关闭");
            break;
        case LK_RENDER_SERVER_CLOSE:
            Navigation::ShowToast("与渲染服务器 TCP 连接关闭");
            break;
        case LK_PROXY_SERVER_CLOSE:
            Navigation::ShowToast("与渲染服务器代理连接关闭");
            break;
    }
}

void LarkxrClient::OnStreamingDisconnect() {
    LOGV("StreamingDisconnect");
}

void LarkxrClient::OnInfo(int infoCode, const std::string &msg) {
    LOGV("OnInfo infoCode:%d msg:%s", infoCode, msg.c_str());
}

void LarkxrClient::OnError(int errCode, const std::string &msg) {
    LOGE("OnError errCode:%d msg:%s", errCode, msg.c_str());
    if (errCode == LK_API_ENTERAPPLI_FAILED) {
        // enter applifailed.
        scene_local_->HomePage();
    } else {
        connected_ = false;
//        scene_cloud_->OnClose();
        scene_local_->HomePage();
        rect_render_->ClearTexture();
    }
    Navigation::ShowToast(msg);
}

void LarkxrClient::OnMediaReady(int nativeTextrure) {
    LOGV("MediaReady HW Decoder textureid:%d", nativeTextrure);
    textureId = nativeTextrure;
    rect_render_->SetMutiviewModeTexture(nativeTextrure);
}

void LarkxrClient::OnMediaReady(int nativeTextrureLeft, int nativeTextureRight) {
    LOGV("MediaReady textureid: left %d right %d", nativeTextrureLeft, nativeTextureRight);
//    this->nativeTextrureLeft = nativeTextrureLeft;
//    this->nativeTextrureRight = nativeTextureRight;
    rect_render_->SetStereoTexture(nativeTextrureLeft, nativeTextureRight);
}

// hw decoder callback without textrue.
void LarkxrClient::OnTrackingFrame(const larkxrTrackingFrame &trackingFrame) {
//    LOGV(" hw decoder callback without textrue");
}

void LarkxrClient::RequestTrackingInfo() {
    larkxrTrackingDevicePairFrame devicePairFrame = {0, 0, 0, device_pair_};
    xr_client_->SendDevicePair(devicePairFrame);

    lark::XRClient::SetControlerBatteryLevel(50, 50);
}

void LarkxrClient::OnClientId(const std::string &clientId) {
    LOGV("OnClientId %s", clientId.c_str());
}

void
LarkxrClient::OnHapticsFeedback(bool isLeft, uint64_t startTime, float amplitude, float duration,
                                float frequency) {
    LOGV("OnHapticsFeedback");
}

void LarkxrClient::OnAppListInfo(const std::vector<lark::AppliInfo> &appliInfo) {
    AppliInfo appliInfo1 = appliInfo.front();
    LOGV("OnAppListInfo appId:%s appName:%s", appliInfo1.appliId.c_str(),
         appliInfo1.appliName.c_str());
    if (!isStartedApp) {
//        EnterAppli("858062276437999616");
//        EnterAppli("858045408058802176");
//        EnterAppli("857953595042037760");
//        EnterAppli("861556520939159552");
//        EnterAppli("861572691763658752");
//        EnterAppli("861575962720993280");


//        EnterAppli("861600892363735040");
//        EnterAppli("861609640016740352");


//        EnterAppli("862777385521512448");

//        EnterAppli("872155170492907520");
//        EnterAppli("756846918545440768");
//        EnterAppli("874605264077062144");

        isStartedApp = true;
    }
}

void LarkxrClient::OnAppListPageInfo(const lark::AppliPageInfo &appliPageInfo) {
    LOGV("OnAppListPageInfo");
}

void LarkxrClient::OnFailed(const std::string &msg) {
    LOGV("============applist OnFailed %s", msg.c_str());
}

void LarkxrClient::OnRunMode(lark::GetVrClientRunMode::ClientRunMode runMode) {
    LOGV("============OnRunModeChange new %d", runMode);
}

void LarkxrClient::setAssets(JNIEnv *env, jobject &assets) {
    textureRect.setAssets(env, assets);
}

void LarkxrClient::setNibiruVRApi(nvr::NibiruVRApi *vrApi) {
    this->nibiruVrApi = vrApi;
}

void LarkxrClient::CloseAppli() {
    LOGV("on close appli");
    if (xr_client_) {
        xr_client_->Close();
    }
}
