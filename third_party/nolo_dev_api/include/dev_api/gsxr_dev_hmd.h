#ifndef GSXR_DEV_HMD_H_
#define GSXR_DEV_HMD_H_

/**
 *  Declare GSXR Copyright here.
 */

#include "gsxr_dev_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// 枚举头显类型
typedef enum gsxr_hmd_type {
    GSXR_HMD_TYPE_VR        = 1,    // VR头显
    GSXR_HMD_TYPE_AR        = 2,    // AR头显
} gsxr_hmd_type_t;

// 枚举视图类型
typedef enum gsxr_view_type {
    GSXR_VIEW_TYPE_MONO     = 1,    // 单视图
    GSXR_VIEW_TYPE_STEREO   = 2,    // 双视图
} gsxr_view_type_t;

// 枚举屏幕扫描输出顺序
typedef enum gsxr_scanout_order {
    GSXR_SCANOUT_ORDER_LEFT_TO_RIGHT    = 1,     // 左至右
    GSXR_SCANOUT_ORDER_RIGHT_TO_LEFT    = 2,     // 右至左
    GSXR_SCANOUT_ORDER_TOP_TO_BOTTOM    = 3,     // 上至下
    GSXR_SCANOUT_ORDER_BOTTOM_TO_TOP    = 4,     // 下至上
} gsxr_scanout_order_t;

// 显示屏配置结构体
typedef struct gsxr_display_config {
    uint32_t                    width;                  // 屏幕宽度(pixel) 刘哥获取
    uint32_t                    height;                 // 屏幕高度(pixel) 刘哥获取
    uint32_t                    refresh_rate;           // 刷新频率(Hz)  刘哥
    float                       vsync_fixed_offset;     // V-Sync 的固定偏移时间值(ms) 1000/刷新率
    gsxr_scanout_order_t        scanout_order;          // 扫描输出顺序 找晓阳 侯哥
} gsxr_display_config_t;

// 视锥体结构体
typedef struct gsxr_frustum {
    float       left;
    float       right;
    float       bottom;
    float       top;
} gsxr_frustum_t;

// 畸变校正参数结构体
typedef struct gsxr_chromatic_poly {
    float       red[8];
    float       green[8];
    float       blue[8];
} gsxr_chromatic_poly_t;

// 镜片视图配置结构体  晓阳
typedef struct gsxr_lens_view_config {
    gsxr_frustum_t          left_frustum;           // 左眼视锥体
    gsxr_frustum_t          right_frustum;          // 右眼视锥体
    float                   left_eye_center[2];     // 左眼畸变校正中心(UVs)
    float                   right_eye_center[2];    // 右眼畸变校正中心(UVs)
    gsxr_chromatic_poly_t   left_chromatic_poly;    // 左眼畸变校正参数
    gsxr_chromatic_poly_t   right_chromatic_poly;   // 右眼畸变校正参数
    uint32_t                warp_mesh_columns;      // 畸变网格 column 数目
    uint32_t                warp_mesh_rows;         // 畸变网格 row 数目
} gsxr_lens_view_config_t;

// 头显跟踪传感器偏移量配置结构体
typedef struct gsxr_sensor_to_head_config {
    gsxr_vector3f_t         position_offest;            // 位置 000
    gsxr_quaternionf_t      orientation_offset;         // 方向 w1 000
} gsxr_sensor_to_head_config_t;

// 头显配置结构体
typedef struct gsxr_hmd_configurations {
    gsxr_hmd_type_t                 hmd_type;               // 头显类型
    gsxr_view_type_t                view_type;              // 视图类型
    gsxr_display_config_t           display_config;         // 显示屏配置
    gsxr_lens_view_config_t         lens_view_config;       // 镜片视图配置
    gsxr_sensor_to_head_config_t    sensor_to_head_config;  // 跟踪传感器偏移量配置 (相对于视图中心)
} gsxr_hmd_configurations_t;

/**
 *  说明：
 *      头显屏幕垂直同步回调函数, 当屏幕发生V-Sync信号时回调, 回调频率与屏幕帧率同步
 *  参数：
 *       [in] timestamp 垂直同步信号时间戳
 *       [in] cookie 函数回调时带回的预备数值
 *  返回值：
 *      无
 *  备注：
 *      无
 *  所属：
 *      GSXR_API_头显信息类
 */
typedef void (*gsxr_hmd_vsync_callback_fn)(int64_t timestamp,
                                           void* cookie);

/**
 *  说明：
 *      头部距离传感器回调函数, 当头部与传感器距离发生变动时回调
 *  参数：
 *       [in] distance 头部与距离传感器距离, 单位为公分
 *       [in] cookie 函数回调时带回的预备数值
 *  返回值：
 *      无
 *  备注：
 *      无
 *  所属：
 *      GSXR_API_头显信息类
 */

typedef void (*gsxr_hmd_proximity_callback_fn)(float distance,
                                               void* cookie);

/**
 *  说明：
 *      头显 IPD 回调函数, 当头显 IPD 变动时回调
 *  参数：
 *       [in] ipd 头显双眼 IPD 距离, 单位为米
 *       [in] cookie 函数回调时带回的预备数值
 *  返回值：
 *      无
 *  备注：
 *      无
 *  所属：
 *      GSXR_API_头显信息类
 */
typedef void (*gsxr_hmd_ipd_callback_fn)(float ipd,
                                         void* cookie);

// GSXR 头显设备函数
typedef struct gsxr_dev_hmd_ops {
    // 设备通用函数
    gsxr_dev_common_ops_t common;

    // 设备信息函数
    gsxr_dev_info_ops_t info;

    // 设备输入函数
    gsxr_dev_input_ops_t input;

    // 设备跟踪函数
    gsxr_dev_tracking_ops_t tracking;

    // ------------------------------------
    // GSXR_API_头显信息类
    // ------------------------------------

    /**
     *  说明：
     *      获取头显设备配置
     *  参数：
     *       [in] hmd_handle 头显设备句柄, 由 open 函数获取
     *       [in] hmd_id 头显设备识别号, 当设备连接时由回调函数 gsxr_dev_event_callback_fn 获取
     *      [out] configs 指向 gsxr_hmd_configurations_t 结构体
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *      -4 无效的设备识别号
     *  备注：
     *      gsxr_dev_event_callback_fn 回调 event 为 GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_CONNECTED, param1 即为设备识别号
     *  所属：
     *      GSXR_API_头显信息类
     */
    int32_t (*get_hmd_configurations)(gsxr_dev_handle_t hmd_handle,
                                      gsxr_dev_id_t hmd_id,
                                      gsxr_hmd_configurations_t* configs);

    /**
     *  说明：
     *      获取头显双眼 IPD 距离
     *  参数：
     *       [in] hmd_handle 头显设备句柄, 由 open 函数获取
     *       [in] hmd_id 头显设备识别号, 当设备连接时由回调函数 gsxr_dev_event_callback_fn 获取
     *      [out] ipd 双眼 IPD 距离, 单位为米
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *      -4 无效的设备识别号
     *  备注：
     *      gsxr_dev_event_callback_fn 回调 event 为 GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_CONNECTED, param1 即为设备识别号
     *  所属：
     *      GSXR_API_头显信息类
     */
    int32_t (*get_hmd_ipd)(gsxr_dev_handle_t hmd_handle,
                           gsxr_dev_id_t hmd_id,
                           float* ipd);

    /**
     *  说明：
     *      设置头显屏幕垂直同步信号回调函数
     *  参数：
     *       [in] hmd_handle 头显设备句柄, 由 open 函数获取
     *       [in] hmd_id 头显设备识别号, 当设备连接时由回调函数 gsxr_dev_event_callback_fn 获取
     *       [in] callback 头显屏幕垂直同步信号回调函数
     *       [in] cookie 函数回调时带回的预备数值
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的回调函数
     *      -4 无效的设备识别号
     *  备注：
     *      gsxr_dev_event_callback_fn 回调 event 为 GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_CONNECTED, param1 即为设备识别号
     *  所属：
     *      GSXR_API_头显信息类
     */
    int32_t (*set_vsync_callback)(gsxr_dev_handle_t hmd_handle,
                                  gsxr_dev_id_t hmd_id,
                                  gsxr_hmd_vsync_callback_fn callback,
                                  void* cookie);

    /**
     *  说明：
     *      设置头显距离传感器信号回调函数
     *  参数：
     *       [in] hmd_handle 头显设备句柄, 由 open 函数获取
     *       [in] hmd_id 头显设备识别号, 当设备连接时由回调函数 gsxr_dev_event_callback_fn 获取
     *       [in] callback 头显距离传感器信号回调函数
     *       [in] cookie 函数回调时带回的预备数值
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的回调函数
     *      -4 无效的设备识别号
     *  备注：
     *      gsxr_dev_event_callback_fn 回调 event 为 GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_CONNECTED, param1 即为设备识别号
     *  所属：
     *      GSXR_API_头显信息类
     */
    int32_t (*set_proximity_callback)(gsxr_dev_handle_t hmd_handle,
                                      gsxr_dev_id_t hmd_id,
                                      gsxr_hmd_proximity_callback_fn callback,
                                      void* cookie);

    /**
     *  说明：
     *      设置头显 IPD 变动回调函数
     *  参数：
     *       [in] hmd_handle 头显设备句柄, 由 open 函数获取
     *       [in] hmd_id 头显设备识别号, 当设备连接时由回调函数 gsxr_dev_event_callback_fn 获取
     *       [in] callback 头显 IPD 变动回调函数
     *       [in] cookie 函数回调时带回的预备数值
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的回调函数
     *      -4 无效的设备识别号
     *  备注：
     *      gsxr_dev_event_callback_fn 回调 event 为 GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_CONNECTED, param1 即为设备识别号
     *  所属：
     *      GSXR_API_头显信息类
     */
    int32_t (*set_ipd_callback)(gsxr_dev_handle_t hmd_handle,
                                gsxr_dev_id_t hmd_id,
                                gsxr_hmd_ipd_callback_fn callback,
                                void* cookie);

    /**
     *  说明：
     *      获取头显设备音量状态
     *  参数：
     *       [in] hmd_handle 头显设备句柄, 由 open 函数获取
     *       [in] hmd_id 头显设备识别号, 当设备连接时由回调函数 gsxr_dev_event_callback_fn 获取
     *      [out] current_volume 当前音量值, 0 <= current_volume <= max_volume
     *      [out] max_volume 最大音量值
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *      -4 无效的设备识别号
     *  备注：
     *      gsxr_dev_event_callback_fn 回调 event 为 GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_CONNECTED, param1 即为设备识别号
     *  所属：
     *      GSXR_API_头显信息类
     */
    int32_t (*get_stream_volume)(gsxr_dev_handle_t hmd_handle,
                                 gsxr_dev_id_t hmd_id,
                                 uint32_t* current_volume,
                                 uint32_t* max_volume);

    /**
     *  说明：
     *      设置头显设备音量状态
     *  参数：
     *       [in] hmd_handle 头显设备句柄, 由 open 函数获取
     *       [in] hmd_id 头显设备识别号, 当设备连接时由回调函数 gsxr_dev_event_callback_fn 获取
     *       [in] volume 设置音量值, 音量值需介于 [0 - 最大音量值] 区间
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *      -4 无效的设备识别号
     *  备注：
     *      gsxr_dev_event_callback_fn 回调 event 为 GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_CONNECTED, param1 即为设备识别号
     *  所属：
     *      GSXR_API_头显信息类
     */
    int32_t (*set_stream_volume)(gsxr_dev_handle_t hmd_handle,
                                 gsxr_dev_id_t hmd_id,
                                 uint32_t volume);

    /**
     *  说明：
     *      获取头显设备屏幕亮度
     *  参数：
     *       [in] hmd_handle 头显设备句柄, 由 open 函数获取
     *       [in] hmd_id 头显设备识别号, 当设备连接时由回调函数 gsxr_dev_event_callback_fn 获取
     *      [out] current_brightness 当前屏幕亮度值, 0 <= current_brightness <= max_brightness
     *      [out] max_brightness 最大屏幕亮度
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *      -4 无效的设备识别号
     *  备注：
     *      gsxr_dev_event_callback_fn 回调 event 为 GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_CONNECTED, param1 即为设备识别号
     *  所属：
     *      GSXR_API_头显信息类
     */
    int32_t (*get_screen_brightness)(gsxr_dev_handle_t hmd_handle,
                                     gsxr_dev_id_t hmd_id,
                                     uint32_t* current_brightness,
                                     uint32_t* max_brightness);

    /**
     *  说明：
     *      设置头显设备屏幕亮度
     *  参数：
     *       [in] hmd_handle 头显设备句柄, 由 open 函数获取
     *       [in] hmd_id 头显设备识别号, 当设备连接时由回调函数 gsxr_dev_event_callback_fn 获取
     *       [in] brightness 设置屏幕亮度值, 屏幕亮度值需介于 [0 - 最大屏幕亮度值] 区间
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *      -4 无效的设备识别号
     *  备注：
     *      gsxr_dev_event_callback_fn 回调 event 为 GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_CONNECTED, param1 即为设备识别号
     *  所属：
     *      GSXR_API_头显信息类
     */
    int32_t (*set_screen_brightness)(gsxr_dev_handle_t hmd_handle,
                                     gsxr_dev_id_t hmd_id,
                                     uint32_t brightness);
} gsxr_dev_hmd_ops_t;


// 头显设备实例结构体
typedef struct gsxr_dev_hmd {
    gsxr_version                api_version;    // 头显实例使用的 API 版本号 GSXR_DEVICE_CURRENT_API_VERSION
    gsxr_dev_hmd_ops_t*         hmd_ops;        // 头显实例实现的 GSXR 头显设备函数指针
} gsxr_dev_hmd_t;

/**
 *  说明：
 *      获取头显设备实例
 *  参数：
 *      无
 *  返回值：
 *      头显设备实例结构体 gsxr_dev_hmd_t
 *  备注：
 *      无
 *  所属：
 *      GSXR_API_头显初始类
 */
gsxr_dev_hmd_t* gsxr_dev_get_hmd_instance();


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* GSXR_DEV_HMD_H_ */
