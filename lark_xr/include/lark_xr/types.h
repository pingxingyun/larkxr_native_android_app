//
// Created by fcx@pingxingyun.com on 2019/11/22.
//

#ifndef CLOUDLARKVRMODULE_TYPES_H
#define CLOUDLARKVRMODULE_TYPES_H

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//
// 用户配置
//
typedef struct larkxrUserSetting_ {
    // 比特率。单位 kbps. 默认值为 30 * 1000 Kbps;
    int bitrateKbps;
    // steam vr 设置的初始房间高度
    float roomHight;
} larkxrUserSetting;
//

//
// 系统类型。VR 一体机头盔，或者手机等。
//
typedef enum larkxrSystemType_ {
    Larkxr_System_VR_HEADSET = 0,
    Larkxr_System_Mobile,
    Larkxr_System_Win32,
    Larkxr_System_Other,
}  larkxrSystemType;

//
// 系统平台类型，主要为不同的 VR 一体机。
//
typedef enum larkxrPlatFromType_ {
    Larkxr_Platform_HTC_FOCUS      = 0,
    Larkxr_Platform_HTC_FOCUS_PLUS = 1,
    Larkxr_Platform_PICO_NEO       = 2,
    Larkxr_Platform_PICO_NEO_2     = 3,
    Larkxr_Platform_PICO_NEO_3     = 4,
    Larkxr_Platform_PICO_G2        = 5,
    Larkxr_Platform_PICO_G2_4k     = 6,
    Larkxr_Platform_PICO_GOBLIN    = 7,
    Larkxr_Platform_DPVR_P1        = 8,
    Larkxr_Platform_Oculus_Quest   = 9,
    Larkxr_Platform_Oculus_Quest_2 = 10,
    Larkxr_Platform_GENERAL_UNITY_WIN32 = 900,
    Larkxr_Platform_GENERAL_WAVE   = 1000,
}  larkxrPlatFromType;

//
// 系统当前使用的 vr 头盔 sdk 类型
//
typedef enum larkxrSDKVersion_ {
    Larkxr_HEADSET_SDK_HTC_WAVE = 0,
    Larkxr_HEADSET_SDK_OCULUS,
    Larkxr_HEADSET_SDK_PICO,
    Larkxr_HEADSET_SDK_GOOGLE_CARDBOARD = 1000,
}  larkxrHeadSetSDKVersion;

//
// 系统基本信息。将在 sdk 最开始初始化。
//
typedef struct larkxrSystemInfo_ {
    bool                      initd;
    larkxrSystemType          sysType;
    larkxrPlatFromType        platFromType;
    larkxrHeadSetSDKVersion   headSetSdkVersion;
    std::string               sdkVersionName;
    std::string               supportServerVersion;
    std::string               deviceName;
    std::string               macAddress;
    uint64_t                  diskTotal;
    uint64_t                  diskUsed;
    uint64_t                  sdTotal;
    uint64_t                  sdUsed;
    std::string               dataPath;
}  larkxrSystemInfo;

//
// 系统当前内存信息
//
typedef struct larkxrDeviceMemInfo_ {
    uint64_t memTotal;
    uint64_t memUsed;
} larkxrDeviceMemInfo;

//
// 系统当前电量信息
//
typedef struct larkxrDeviceBatteryInfo_ {
    int hmd;
    int controlerLeft;
    int controlerRight;
} larkxrDeviceBatteryInfo;
// end system typs.

//
// vr 渲染左右眼类型
//
typedef enum larkxrEye_ {
    LARKVR_EYE_LEFT   = 0,
    LARKVR_EYE_RIGHT  = 1,
    LARKVR_EYE_COUNT  = 2
} larkxrEye;

//
// 提交给云端 vr 渲染用的 fov。fov 应从 vr 头盔sdk中获取正确的值。以度数为单位。
//
typedef struct larkxrRenderFov_ {
    float left;
    float right;
    float top;
    float bottom;
} larkxrRenderFov;


//
// vr 渲染相关信息，包括帧率，瞳距，分辨率，左右眼fov
//
typedef struct larkxrRenderInfo_ {
    int fps;
    float ipd;
    // 左右眼加一起的宽度
    int renderWidth;
    int renderHeight;
    // 渲染的视场角，以度数为单位。
    larkxrRenderFov fov[LARKVR_EYE_COUNT];
} larkxrRenderInfo;
// end render info

//
// device types.
//
// 设备类型
typedef enum larkxrDeviceType_ {
    // 头盔
    Larkxr_Device_Type_HMD = 0,
    // 1：左手柄
    Larkxr_Device_Type_Controller_Left = 1,
    // 2：右手柄
    Larkxr_Device_Type_Controller_Right = 2,
} larkxrDeviceType;

typedef enum larkxrControllerType_ {
    Larkxr_Controller_Left = Larkxr_Device_Type_Controller_Left,
    Larkxr_Controller_Right = Larkxr_Device_Type_Controller_Right
} larkxrControllerType;

//
typedef enum larkxrPoseOriginModel_ {
    Larkxr_PoseOriginModel_OriginOnHead                  = 0, /**< The origin of 6 DoF pose is on head. */
    Larkxr_PoseOriginModel_OriginOnGround                = 1, /**< The origin of 6 DoF pose is on ground. */
    Larkxr_PoseOriginModel_OriginOnTrackingObserver      = 2, /**< The raw pose from tracking system. */
    Larkxr_PoseOriginModel_OriginOnHead_3DoF             = 3, /**< The origin of 3 DoF pose is on head. */
} larkxrPoseOriginModel;

typedef enum larkxrNumDoF_ {
    Larkxr_NumDoF_3DoF = 0,
    Larkxr_NumDoF_6DoF = 1,
} larkxrNumDoF;

typedef enum larkxrIntensity_ {
    Larkxr_Intensity_Weak       = 1,   /**< The Intensity of vibrate is Weak. */
    Larkxr_Intensity_Light      = 2,   /**< The Intensity of vibrate is Light. */
    Larkxr_Intensity_Normal     = 3,   /**< The Intensity of vibrate is Normal. */
    Larkxr_Intensity_Strong     = 4,   /**< The Intensity of vibrate is Strong. */
    Larkxr_Intensity_Severe     = 5,   /**< The Intensity of vibrate is Severe. */
} larkxrIntensity;
/**
 * 设备姿态数据。
 *  test data from wvr
 *  rotation:     -0.275494039 0.103380702 0.0448519662 0.95467472
 *  raw rotation: 0.275494069 -0.10338071 -0.0448519699 0.954674661
 *   is6dof: 1; rawX: 0.124201; rawY: -0.027059; rawZ: -0.041025;
 *                 X: -0.124201; Y: 0.027059; Z: 0.041025;
 */
typedef struct larkxrTrackedPose_ {
    larkxrDeviceType device;
    bool isConnected;
    bool is6Dof;
    bool isValidPose;
    int status;
    int64_t timestamp;                  /**< Absolute time (in nanosecond) of pose. */
    int64_t poseFetchTime;
    int64_t expectedDisplayTime;
    float predictedMilliSec;            /**< Number of milliseconds from now to predict poses. */
    larkxrPoseOriginModel poseOriginModel;
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 velocity;
    glm::vec3 angularVelocity;
    glm::vec3 acceleration;
    glm::vec3 angularAcceleration;
    // raw porstion from sene
    glm::vec3 rawPosition;
    glm::quat rawRotation;
    // pose matrix for htc wave
    glm::mat4 rawPoseMatrix;
    struct
    {
        glm::vec3       viewPosition; // view position for openxr stereo view pose
        glm::quat       viewRotation; // view rotation for openxr stereo view pose
        glm::mat4		projectionMatrix;
        glm::mat4		viewMatrix;
    } eye[ LARKVR_EYE_COUNT ];
#if defined(NOLO_6DOF)
    glm::vec3 baseLineOffset; // 头部基准线偏移。nolo 使用
        glm::vec3 noloPosition;   // nolo position.
        glm::quat noloRotation;
#endif
    int battery; // battery 0 - 100;
} larkxrTrackedPose;

typedef enum larkxrNoloInputId_ {
    Larkxr_InputId_0     = 0,
    Larkxr_InputId_1     = 1,
    Larkxr_InputId_2     = 2,
    Larkxr_InputId_3     = 3,
    Larkxr_InputId_4     = 4,
    Larkxr_InputId_5     = 5,

    //alias group mapping
    Larkxr_InputId_TouchPad_Press   = Larkxr_InputId_0,
    Larkxr_InputId_Trigger          = Larkxr_InputId_1,
    Larkxr_InputId_AppMenu          = Larkxr_InputId_2,
    Larkxr_InputId_Home             = Larkxr_InputId_3,
    Larkxr_InputId_Grip             = Larkxr_InputId_4,
    Larkxr_InputId_TouchPadTouch    = Larkxr_InputId_5,
} larkxrNoloInputId;

//
// 设备输入状态
//
typedef enum larkxrInput_ {
    larkxr_Input_System_Click,
    larkxr_Input_Application_Menu_Click,
    larkxr_Input_Grip_Click,
    larkxr_Input_Grip_Value,
    larkxr_Input_Grip_Touch,
    larkxr_Input_DPad_Left_Click,
    larkxr_Input_DPad_Up_click,
    larkxr_Input_DPad_Right_Click,
    larkxr_Input_DPad_Down_Click,
    larkxr_Input_A_Click,
    larkxr_Input_A_Touch,
    larkxr_Input_B_Click,
    larkxr_Input_B_Touch,
    larkxr_Input_X_Click,
    larkxr_Input_X_Touch,
    larkxr_Input_Y_Click,
    larkxr_Input_Y_Touch,
    larkxr_Input_Trigger_Left_Value,
    larkxr_Input_Trigger_Right_Value,
    larkxr_Input_Shoulder_Left_Click,
    larkxr_Input_Shoulder_Right_Click,
    larkxr_Input_Joystick_Left_Click,
    larkxr_Input_Joystick_Left_X,
    larkxr_Input_Joystick_Left_Y,
    larkxr_Input_Joystick_Right_Click,
    larkxr_Input_Joystick_Right_X,
    larkxr_Input_Joystick_Right_Y,
    larkxr_Input_Joystick_Click,
    larkxr_Input_Joystick_X,
    larkxr_Input_Joystick_Y,
    larkxr_Input_Joystick_Touch,
    larkxr_Input_Back_Click,
    larkxr_Input_Guide_Click,
    larkxr_Input_Start_Click,
    larkxr_Input_Trigger_Click,
    larkxr_Input_Trigger_Value,
    larkxr_Input_Trigger_Touch,
    larkxr_Input_Trackpad_X,
    larkxr_Input_Trackpad_Y,
    larkxr_Input_Trackpad_Click,
    larkxr_Input_Trackpad_Touch,

    larkxr_Input_MAX = larkxr_Input_Trackpad_Touch,
    larkxr_Input_COUNT = larkxr_Input_MAX + 1
} larkxrInput;
#define LARKXR_BUTTON_FLAG(input) (1ULL << input)

//
// 手柄的数据输入状态
//
typedef struct larkxrControllerInputState_ {
    larkxrDeviceType deviceType;
    bool isConnected;
    uint64_t buttons;
    /// d          ! y = 1
    /// d          |
    /// d          |
    /// d  --------|--------> x 1
    /// d          |
    /// d          | -1
    /// d        openvr
    glm::vec2 touchPadAxis;
    float triggerValue; // trigger axis 0 - 1.0f
    float gripValue;
    uint8_t batteryPercentRemaining;
} larkxrControllerInputState;

//
// 手柄设备状态
//
typedef struct larkxrControllerDeviceState_ {
    larkxrControllerType deviceType; // controller left or right
    larkxrTrackedPose pose;
    larkxrControllerInputState inputState;
    float rotateDeg; // rotate the controller.
    glm::vec3 rotateAxis;
} larkxrControllerDeviceState;

//
// 设备电量
//
typedef enum larkxrBatteryStatus_ {
    Larkxr_BatteryStatus_Unknown = 0,
    Larkxr_BatteryStatus_Normal = 1,
    Larkxr_BatteryStatus_Low = 2,
    Larkxr_BatteryStatus_UltraLow = 3,
} larkxrBatteryStatus;

const uint8_t LARKVR_BATTERY_STATUS_NUMBER_NORMAL = 100;
const uint8_t LARKVR_BATTERY_STATUS_NUMBER_LOW = 50;
const uint8_t LARKVR_BATTERY_STATUS_NUMBER_ULTRALLOW = 10;

typedef enum larkxrAsyncTaskType_ {
    Larkxr_Vibrator_Controller_Left = 0,
    Larkxr_Vibrator_Controller_Right,
} larkxrAsyncTaskType;

const int LARKVR_TOTAL_CONTROLLER_COUNT = 2;

//
// 包括头盔和手柄在内的完整设备信息.
//
typedef struct larkxrDevicePair_ {
    larkxrTrackedPose hmdPose;
    larkxrControllerDeviceState controllerState[LARKVR_TOTAL_CONTROLLER_COUNT];
} larkxrDevicePair;
// end deive types.

//
// 所有设备的姿态数据帧
//
typedef struct larkxrTrackingDevicePairFrame_ {
    uint64_t frameIndex;
    uint64_t fetchTime;
    double displayTime;
    larkxrDevicePair devicePair;
} larkxrTrackingDevicePairFrame;

//
// 姿态数帧
//
typedef struct larkxrTrackingFrame_ {
    larkxrTrackedPose tracking;
    uint64_t frameIndex;
    uint64_t fetchTime;
    double displayTime;
} larkxrTrackingFrame;

//
// Player space data
//
typedef struct larkxrPlaySpace_
{
    int             recenterCount;
    glm::vec3       position;
    glm::quat       rotation;
    float			area_width;
    float			area_height;
    uint32_t		perimeterPointsCount;						 //default  =  0
    float           (*perimeterPoints)[3];
} larkxrPlaySpace;

//
// 硬件视频解码纹理类型
//
typedef enum larkxrHwRenderTextureType_ {
    // 不包含纹理
    larkxrHwRenderTextureType_None                 = -1,
    // Android gltexture2d 纹理 左右眼在一起
    larkxrHwRenderTextureType_Android_Multiview    = 4,
    // android gltexture2d opengl 双眼分开，左眼一个纹理右眼一个纹理
    larkxrHwRenderTextureType_Android_Stereo       = 5,
    // windows d3d11 native texture ShaderResourceView 左右眼在一起
    larkxrHwRenderTextureType_D3D11_Multiview      = 6,
    // windows d3d11 native texture ShaderResourceView 双眼分开
    larkxrHwRenderTextureType_D3D11_Stereo         = 7,
    // windows d3d11 nv12 native texture 左右眼在一起
    larkxrHwRenderTextureType_D3D11_NV12           = 8,
    // windows d3d11 yuv texture, ShaderResourceView 类型
    // 分成 y 和 uv 两个纹理，从 nv12 纹理中取出。左右眼在一起
    larkxrHwRenderTextureType_D3D11_Y_UV_SRV       = 9,
} larkxrHwRenderTextureType;

//
// 硬件视频解码的纹理
// 
typedef struct larkxrHwRenderTexture_
{
    larkxrHwRenderTextureType type;
    void* textureSlot1;
    void* textureSlot2;
    int width;
    int height;
} larkxrHwRenderTexture;
//#pragma pack(pop) // end pack
#endif //CLOUDLARKVRMODULE_TYPES_H
