//
// Created by fcx on 2020/10/14.
//

#ifndef CLOUDLARKXR_COMMON_TYPES_H
#define CLOUDLARKXR_COMMON_TYPES_H

// sdk version.
const int LARK_NATIVE_CLIENT_SDK_VERSION_MARJOR = 3;
const int LARK_NATIVE_CLIENT_SDK_VERSION_MINOR  = 2;
const int LARK_NATIVE_CLIENT_SDK_VERSION_REVISE = 0;
const int LARK_NATIVE_CLIENT_SDK_VERSION_BUILD  = 5;

/// vr setup
typedef enum larkHeadSetType_ {
    larkHeadSetType_HTC = 0,
    larkHeadSetType_OCULUS = 1,
    larkHeadSetType_PICO_2 = 2,
    larkHeadSetType_PICO_3 = 3,
    larkHeadSetType_NOLO_Sonic_1 = 4,
} larkHeadSetType;

typedef struct larkHeadSetControllerDesc_ {
    larkHeadSetType     type = larkHeadSetType_OCULUS;
    bool                forece3dof = false; //强制3dof 如oculus go
    float               controllerposeTimeOffset = 0.01;//正常:0.01 中速:-0.03 快速:-1 控制器追踪速度"像需要快速运动的游戏比如《光剑》，选择中速或快速。 运动比较慢的游戏比如《Skyrim》，使用正常即可。",
    float               hapticsIntensity = 3;//控制器触动反馈0-5
} larkHeadSetControllerDesc;

//集中渲染
/*以高分辨率渲染图像的中心,而以较低的分辨率渲染郊区。
导致需要通过网络传输的视频分辨率大大降低。在相同比特率的情况下,较小的视频可以保留更多细节,并同时降低延迟。
FFR会在视图的边缘造成一些视觉伪像,这些伪像或多或少会根据设置和游戏而变得可见。
*/
typedef struct larkFoveatedRendering_
{
    bool                    enableFoveateRendering = true;      //true的时候 其他参数启用(默认为true)
    float                   foveationStrength = 2;      //渲染强度          [0.5-10.0]          默认 2 值越高,意味着朝向帧边缘的细节越少,伪像越多
    float                   foveationShape = 1.5;      //渲染形状          [0.2-2.0]           默认 1.5  集中渲染的形状
    float                   foveationVerticalOffset = 0;      //渲染垂直偏移      [-0.05-0.05]        默认 0  较高的值表示高质量的帧区域进一步向下移动
} larkFoveatedRendering;

//色彩校正
typedef struct larkColorCorrention_
{
    bool                    enableColorCorrection = true;      //true的时候 其他参数启用(默认为false)
    float                   brightness = 0;      //亮度:范围[-1; 1],默认值为0。-1完全为黑色,1完全为白色
    float                   contrast = 0;      //对比度:范围[-1; 1],默认值为0。-1完全是灰色的
    float                   saturation = 0;      //饱和度:范围[-1; 1],默认值为0。-1为黑白
    float                   gamma = 1;      //伽玛:范围[0; 5],默认值为1。使用值2.2校正从sRGB到RGB空间的颜色
    float                   sharpening = 0;      //锐化:范围[-1; 5],默认为0。-1是最模糊的,5是最锐利的
} larkColorCorrention;

typedef enum larkStreamType_
{
    larkStreamType_UDP = 0,                        //普通UDP
    larkStreamType_TCP = 1,                        //普通TCP
    larkStreamType_THROTTLED_UDP = 2,              //调速(服务端控制)+普通UDP
    larkStreamType_KCP = 3,
} larkStreamType;

//视频相关
typedef struct larkVRVideoDesc_
{
    uint32_t                  eye_resolution_width = 1440; // half eye
    uint32_t                  eye_resolution_height = 1440;
    uint32_t                  fps = 72;      //60 72 80 90 120
    uint32_t                  encode_bitrate_mbs = 30;      //mbp/s
    uint32_t                  codec = 0;      //0 h264 1 h265
    uint32_t                  target_eye_resolution_width = 1440; // half eye
    uint32_t                  target_eye_resolution_height = 1440;
    float                   seconds_from_vsync_to_photons = 0.005;      //0.005
    bool                    use10bitEncoder = false;      //false
    //-------集中渲染相关
    larkFoveatedRendering       foveatedRendering;
    //-------色彩校正相关
    larkColorCorrention         colorCorrention;
    //StreamType
    larkStreamType              stream = larkStreamType_UDP;
} larkVRVideoDesc;


///
typedef struct RenderFov_ {
    float left;
    float right;
    float top;
    float bottom;
} RenderFov;

typedef enum PlayerModeType_ {
    // 普通模式
    PlayerModeType_Normal = 0,
    // 交互模式（可以一人操作多人看）
    PlayerModeType_Interactive = 1,
    // 多人互动模式 (多人可操作)
    PlayerModeType_MutiPlayer = 2,
} PlayerModeType;

typedef enum UserType_ {
    // 观看者
    UserType_Observer = 0,
    // 操作者
    UserType_Player = 1,
} UserType;

// config
typedef struct CommonConfig_ {
    bool debugTask;
    bool useProxy;
    std::string appServer;
    int appPort;
    std::string preferPubOutIp;
    // common config
    std::string taskId;
    std::string appliId;
    int noOperationTimeout;
    int fps;
    int bitrateKbps;
    int width;
    int height;
    // vr config.
    float ipd;
    float roomHeight;
    // oculus controller.
    bool hasTouchcontroller;
    RenderFov fovList[2];
    bool useKcp;
    bool useH265;
    PlayerModeType playerMode;
    UserType userType;
    std::string nickName;
    std::string roomCode;
    larkHeadSetControllerDesc headSetDesc;
    larkVRVideoDesc vrVideoDesc;
} CommonConfig;

typedef enum larkEventTypes_ {
    // 进入应用接口调用
    LK_API_ENTERAPPLI_SUCCESS                        = 10,
    LK_API_ENTERAPPLI_FAILED                         = 11,
    LK_API_ENTERAPPLI_FAILED_SDK_UN_AUTHORIZATIONED  = 13,
    // 同步应用中
    LK_TASK_SYNC_APP                                 = 20,
    // SDK 授权验证
    LK_SDK_AUTHORIZATION_SUCCESS                     = 30,
    LK_SDK_AUTHORIZATION_FAILED                      = 31,
    LK_SDK_AUTHORIZATION_FAILED_NETWORK_FAILED       = 32,
    LK_SDK_AUTHORIZATION_FAILED_SERVER_ERROR         = 33,
    LK_SDK_AUTHORIZATION_FAILED_LICENCE_NOT_FOUND    = 34,
    LK_SDK_AUTHORIZATION_FAILED_LICENCE_FORBIDDEN    = 35,
    LK_SDK_AUTHORIZATION_FAILED_SYS_NOT_INIT         = 36,

    //
    // 连接应用服务器事件。直连渲染服务器时抛出
    // 消息来源：websocket 连接事件
    //
    LK_RENDER_SERVER_CONNECTED                       = 100,
    LK_RENDER_SERVER_FAILED                          = 101,
    LK_RENDER_SERVER_CLOSE                           = 102,
    LK_RENDER_SERVER_ERROR                           = 103,

    //
    // 连接 websocket 代理服务器事件
    // 消息来源：websocekt proxy 连接事件
    //
    LK_PROXY_SERVER_CONNECTED                        = 200,
    LK_PROXY_SERVER_FAILED                           = 201,
    LK_PROXY_SERVER_CLOSE                            = 202,
    LK_PROXY_SERVER_ERROR                            = 203,

    //
    // 版本检测
    // 消息来源：服务端协议返回 ToClientMessage->VersionCheckResponse
    //
    LK_VERSION_CHECK_SUCCESS                         = 300,
    LK_VERSION_CHECK_FAILED                          = 301,

    //
    // task 检测
    // 消息来源： 服务端协议返回 ToClientMessage->TaskResponse
    //
    LK_TASK_SUCCESS                                  = 400,
    LK_TASK_NOTFOUND                                 = 401,
    LK_TASK_SERVER_ERROR                             = 402,
    LK_TASK_APP_WRONGPARAM                           = 403,
    LK_TASK_NO_GPU_RESOURCE                          = 404,

    //
    // 启动流媒体
    // 消息来源： 服务端协议返回 ToClientMessage->StartStreamResponse
    //
    LK_START_STREAM_SUCCESS                           = 500,
    LK_START_STREAM_PROCESS_START_FAILED              = 501,
    LK_START_STREAM_PROCESS_START_TIMEOUT             = 502,
    LK_START_STREAM_NOT_STREAMING                     = 503,
    LK_START_STREAM_ENCODER_ERROR                     = 504,

    //
    // RTC 事件
    // 消息来源：webrtc 连接事件和 ToClientMessage->WebrtcError
    //
    LK_RTC_EVENT_PEERCONNECTION_CONNECTED             = 600,
    LK_RTC_EVENT_PEERCONNECTION_CLOSED                = 601,
    LK_RTC_EVENT_PEERCONNECTION_ERROR                 = 602,

    //
    // 加载视频流
    // 消息来源：浏览器或原生组件
    //
    LK_VIDEO_LOADED                                   = 700,

    //
    // 服务端主动退出
    // 消息来源：后台协议 NotifyClientLogout
    //
    LK_NOTIFY_CLIENT_LOGOUT_PLAYER_LOGOUT             = 800,
    // 一人操作多人看房主退出
    LK_NOTIFY_CLIENT_LOGOUT_TASKOWNER_LOGOUT          = 801,

    //
    // 服务端推送云端应用事件
    // 消息来源：后台协议 AppProcessNotification
    //
    LK_APP_PROCESS_NOTIFI_APP_QUIT                    = 900,
    // 无操作超时
    LK_NO_OPERATION_TIMEOUT                           = 910,

    //
    // XR 相关事件
    //
    LK_STARTVRSTREAM_SUCCESS                            = 1000,
    LK_STARTVRSTREAM_START_PROCESS_FAILED               = 1001,
    LK_STARTVRSTREAM_START_DRIVER_RUNTIME_TIMEOUT       = 1002,
    LK_STARTVRSTREAM_START_DRIVER_RUNTIME_UDPPORT_ERROR = 1003,
    LK_STARTVRSTREAM_START_DRIVER_RUNTIME_ENCODER_ERROR = 1004,
    LK_XR_MEDIA_TRANSPORT_CHANNEL_CONNECT               = 1010,
    LK_XR_MEDIA_TRANSPORT_CHANNEL_CLOSED                = 1011,
    LK_XR_MEDIA_TRANSPORT_CHANNEL_ERROR                 = 1012,
    LK_XR_MEDIA_TRANSPORT_CHANNEL_USER_CANCEL           = 1013,

    //
    // 外部请求
    //
    // 操作
    LK_IFRAME_POSTER_OPERATE_MOUSE_MOVE               = 10000,
    LK_IFRAME_POSTER_OPERATE_MOUSE_DOWN               = 10001,
    LK_IFRAME_POSTER_OPERATE_MOUSE_UP                 = 10002,
    LK_IFRAME_POSTER_OPERATE_MOUSE_WHEEL              = 10003,
    LK_IFRAME_POSTER_OPERATE_KEY_DOWN                 = 10010,
    LK_IFRAME_POSTER_OPERATE_KEY_UP                   = 10011,
    // 功能
    LK_IFRAME_POSTER_FUNC_MOUSE_MODE                  = 10100,
    LK_IFRAME_POSTER_FUNC_SCALE_MODE                  = 10101,
    // ui
    LK_IFRAME_POSTER_UI_CONTROLLER_BAR                = 10200,

    //
    // dcs 服务器接收事件
    //
    LK_DATA_CHANNEL_ESTABLISHED                        = 20000,
    LK_DATA_CHANNEL_RETYING                            = 20001,
    LK_DATA_CHANNEL_CLOSE                              = 20002,
    LK_DATA_CHANNEL_ERROR                              = 20003,
    LK_DATA_CHANNEL_BINARYMESSAGE                      = 20004,
    LK_DATA_CHANNEL_TEXT_MESSAGE                       = 20005,
    //
    // dcs 服务器发送事件
    //
    LK_RE_CONNECT_DATA_CHANNEL                         = 20100,
    LK_CLOSE_DATA_CHANNEL                              = 20101,
    LK_DATA_CHANNEL_SEND_TEXT                          = 20102,
    LK_DATA_CHANNEL_SEND_BINARY                        = 20103,
    LK_NOT_DEFAINE                                     = 99999,
} larkEventTypes;
#endif //CLOUDLARKXR_COMMON_TYPES_H
