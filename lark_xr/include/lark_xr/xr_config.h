//
// Created by fcx on 2020/4/17.
//

#ifndef CLOUDLARKXR_XR_CONFIG_H
#define CLOUDLARKXR_XR_CONFIG_H

#include "types.h"
#include "lark_xr.h"
#include "lk_common_types.h"

namespace lark {
//
// 配置的默认值
//
const int LK_CONFIG_DEFAULT_RENDER_WIDTH = 2880;
const int LK_CONFIG_DEFAULT_RENDER_HEIGHT = 1440;
const float LK_CONFIG_DEFAULT_RESOLUTION_SCALE = 1.0f;
const int LK_CONFIG_DEFAULT_BITRATE = 40 * 1000;
const int LK_CONFIG_DEFAULT_FPS = 72;
const float LK_CONFIG_DEFAULT_ROOM_HEIGHT = 0.0f;
const float LK_CONFIG_DEFAULT_IPD = 0.064f;
const larkxrRenderFov LK_CONFIG_DEFAULT_FOV[2] = {
    {45.0f, 45.0f, 45.0f, 45.0f},
    {45.0f, 45.0f, 45.0f, 45.0f},
};
const bool LK_CONFIG_DEFAULT_USE_KCP = true;
const bool LK_CONFIG_DEFAULT_USE_H265 = false;
const bool LK_CONFIG_DEFAULT_USE_HAPTICS_FEEDBACK = true;
const bool LK_CONFIG_DEFAULT_USE_MULTIVIEW = true;
const bool LK_CONFIG_DEFAULT_FLIP_DRAWING = false;
const float LK_CONFIG_DEFAULT_SECONDS_FROM_VSYNC_TO_PHOTONS = 0.005;
const bool LK_CONFIG_DEFAULT_USE_10BIT_ENCODER = false;
const larkFoveatedRendering LK_CONFIG_DEFAULT_FOVEATED_RENDERING = {};
const larkColorCorrention LK_CONFIG_DEFAULT_COLOR_CORRENTION = {};
const larkStreamType LK_CONFIG_DEFAULT_STREAM_TYPE = larkStreamType_KCP;
const bool LK_CONFIG_DEFAULT_USE_RENDER_QUEUE = false;
const bool LK_CONFIG_DEFAULT_REPORT_FEC_FAILED = false;
#ifdef WIN32
const bool LK_CONFIG_DEFAULT_D3D11_TEXTURE_OUTPUT = true;
#endif

typedef enum QuickConfigLevel_ {
    QuickConfigLevel_Manual    = 0,
    QuickConfigLevel_Auto      = 1,
    QuickConfigLevel_Fast      = 2,
    QuickConfigLevel_Normal    = 3,
    QuickConfigLevel_Extreme   = 4,
} QuickConfigLevel;

typedef struct QuickConfigGroup_ {
    QuickConfigLevel level;
    float resolution_scale;
    int bitrate;
    larkStreamType stream_type;
    bool use_foveated_rendering;
    bool use_h265;
    bool use_10bit_encoder;
    bool report_fec_failed;
} QuickConfigGroup;

const QuickConfigLevel DEFAULT_QUICK_CONFIG_LEVEL = QuickConfigLevel_Normal;
const QuickConfigGroup DEFALUT_QUICK_CONFIG_GROUP_FAST = {
        QuickConfigLevel_Fast,                   // 流畅
        0.9,                             // 分辨率缩放 0.9
        20 * 1000,                              // 码率 20M
        larkStreamType::larkStreamType_UDP, // 普通 udp
        true,                       // 启用 ffr
        true,                                  // 启用 h265
        false,                          // 关闭 10 bit 编码
        true,                            // 发送 fec 失败
};
const QuickConfigGroup DEFALUT_QUICK_CONFIG_GROUP_NORMAL = {
        QuickConfigLevel_Normal,                 // 标准
        LK_CONFIG_DEFAULT_RESOLUTION_SCALE,            // 分辨率缩放 1
        LK_CONFIG_DEFAULT_BITRATE,                     // 码率 40M
        LK_CONFIG_DEFAULT_STREAM_TYPE,                 // 增强 udp
        LK_CONFIG_DEFAULT_FOVEATED_RENDERING.enableFoveateRendering, // 启用 ffr
        LK_CONFIG_DEFAULT_USE_H265,                    // 关闭 h265
        LK_CONFIG_DEFAULT_USE_10BIT_ENCODER,           // 关闭 10 bit 编码
        LK_CONFIG_DEFAULT_REPORT_FEC_FAILED,            // 发送 fec 失败
};
const QuickConfigGroup DEFALUT_QUICK_CONFIG_GROUP_EXTREME = {
        QuickConfigLevel_Extreme,                  // 超清
        1.0,                               // 分辨率缩放 1
        60 * 1000,                                // 码率 60M
        larkStreamType::larkStreamType_KCP,   // 增强 udp
        true,                         // 启用 ffr
        false,                                  // 关闭 h265
        true,                            // 关闭 10 bit 编码
        false,                            // 发送 fec 失败
};

class LARK_XR_API XRConfig {
public:
    // 渲染的宽，为左右眼加在一起的宽度
    static int render_width;
    // 渲染的高度。
    static int render_height;
    // 分辨率缩放 0 - 2
    // 最终分辨率为 align32(render_width * resolution_scale); align32(render_height * resolution_scale)
    static float resolution_scale;
    // 比特率。单位 kbps.
    static int bitrate;
    // 帧率
    static int fps;
    // @Deprecated 已弃用.使用头盔姿态的 tracking origin 等功能和安全区域配合使用
    // steam vr 设置的初始房间高度
    static float room_height;
    // 瞳距
    static float ipd;
    // 提交给云端 vr 渲染用的 fov。fov 应从 vr 头盔sdk中获取正确的值。以度数为单位。
    // 部分 vr 头盔的左右眼 fov 是非对称的。
    static larkxrRenderFov fov[2];
    // @Deprecated 已弃用，使用 stream_type 替代
    // 是否使用增强协议连接媒体
    static bool use_kcp;
    // 是否使用 h265
    static bool use_h265;
    // 是否启用手柄的震动反馈
    static bool use_haptics_feedback;
    // 是否将视频画面左右眼渲染到同一纹理上
    static bool use_multiview;
    // 是否将视频画面翻转渲染
    // 上下翻转
    static bool flip_draw;
    // 0.005 (3.1.8.0新增)
    static float seconds_from_vsync_to_photons;
    // 是否启用 10 bit 编码 (3.1.8.0新增)
    static bool use_10bit_encoder;
    // 是否上报 fec 失败，当当前 fec 实效时通知服务端加大冗余包 (3.1.8.0新增)
    static bool report_fec_failed;
    // 头盔类型设置，将根据具体头盔的类型选择steamvr中用的手柄和绑定文件 (3.1.8.0新增)
    static larkHeadSetControllerDesc headset_desc;
    // 集中渲染设置 (3.1.8.0新增)
    static larkFoveatedRendering foveated_rendering;
    // 颜色校正设置 (3.1.8.0新增)
    static larkColorCorrention color_corrention;
    // 设置使用的传输通道类型，目前有 udp，tcp，增强udp可选 (3.1.8.0新增)
    static larkStreamType stream_type;
    // 是否使用渲染队列 (3.1.8.0新增)
    static bool use_render_queue;
    static QuickConfigLevel quick_config_level;

#ifdef WIN32
    static bool d3d11_texture_output;
#endif // WIN32


    static void set_render_width(int width);
    static void set_render_height(int height);
    static void set_bitrate(int bitrate);
    static void set_fps(int fps);
    static void set_room_height(float roomHeight);
    static void set_ipd(float ipd);
    static void set_use_kcp(bool useKcp);
    static void set_use_H265(bool useH265);
    static void set_use_haptics_feedback(bool useHapticsFeedback);
    static void set_fov(larkxrRenderFov* fov, int len);
    static void set_use_multiview(bool useMulti);
    static void set_flip_draw(bool flipDraw);
    static void set_resolution_scale(float scale);
    static void set_seconds_from_vsync_to_photons(float vsync_to_photons);
    static void set_use_10bit_encoder(bool use);
    static void set_headset_desc(larkHeadSetControllerDesc desc);
    static void set_foveated_rendering(const larkFoveatedRendering& foveatedRendering);
    static void set_color_corrention(const larkColorCorrention& colorCorrention);
    static void set_use_render_queue(bool use);

    static uint32_t align32(float value);
    static uint32_t align32ed_scaled_render_width();
    static uint32_t align32ed_scaled_halfeye_render_width();
    static uint32_t align32ed_scaled_render_height();

    static void QuickConfigWithDefaulSetup(QuickConfigLevel level);
    static void QuickConfig(const QuickConfigGroup& config);
    static larkVRVideoDesc GetVideoDesc();
};
}
#endif //CLOUDLARKXR_XR_CONFIG_H
