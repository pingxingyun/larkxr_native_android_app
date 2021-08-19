#ifndef GSXR_DEV_CONTROLLER_H_
#define GSXR_DEV_CONTROLLER_H_

/**
 *  Declare GSXR Copyright here.
 */

#include "gsxr_dev_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// 枚举手柄控制器类型
typedef enum gsxr_controller_type {
    GSXR_CONTROLLER_TYPE_RIGHT      = 1,    // 右手手柄控制器
    GSXR_CONTROLLER_TYPE_LEFT       = 2,    // 左手手柄控制器
} gsxr_controller_type_t;

// 手柄控制器配置结构体
typedef struct gsxr_controller_configurations {
    gsxr_controller_type_t  controller_type;    // 手柄控制器类型
} gsxr_controller_configurations_t;

// 手柄控制器信息
typedef struct gsxr_controller_device {
    uint64_t        CON_ID;                     // 连线识别号
    const char*     name;                       // 名称
    bool            paired;                     // 是否曾配对过
    bool            connected;                  // 是否已连线
} gsxr_controller_device_t;

// 手柄控制器列表
typedef struct gsxr_controller_list {
    uint32_t                            devices_len;    // 数组个数
    const gsxr_controller_device_t*     devices;        // 指向手柄信息数组
} gsxr_controller_list_t;

// 触控板操作模式
typedef enum gsxr_touchpad_mode {
    GSXR_TOUCHPAD_MODE_SWIPE    = 0x1,          // 滑动模式
    GSXR_TOUCHPAD_MODE_CLICK    = 0x2,          // 按键模式
    GSXR_TOUCHPAD_MODE_ANALOG   = 0x4,          // 鼠标模式
} gsxr_touchpad_mode_t;

// 振动反馈结构体
typedef struct gsxr_haptic_vibration {
    int64_t         duration;                   // 振动时长, 单位为 nanoseconds 
    uint32_t        frequency;                  // 振动次数
    float           intensity;                  // 振动强度, 数据介于 0 - 1
} gsxr_haptic_vibration_t;

/**
 *  说明：
 *      手柄固件回调函数, 当手柄升级固件版本时回调
 *  参数：
 *       [in] controller_id 手柄设备识别号
 *       [in] status 更新状态, 0 为无可被更新之固件, 1 为更新中, 2 为更新成功, 3 为更新失败
 *       [in] progress 更新进度, 0 - 100
 *       [in] error 错误代码
 *       [in] cookie 函数回调时带回的预备数值
 *  返回值：
 *      无
 *  备注：
 *      无
 */
typedef void (*gsxr_controller_firmware_callback_fn)(gsxr_dev_id_t controller_id,
    int32_t status, int32_t progress, int32_t error, void* cookie);


// GSXR 手柄设备函数
typedef struct gsxr_dev_controller_ops {
    // 设备通用函数
    gsxr_dev_common_ops_t common;

    // 设备信息函数
    gsxr_dev_info_ops_t info;

    // 设备输入函数
    gsxr_dev_input_ops_t input;

    // 设备跟踪函数
    gsxr_dev_tracking_ops_t tracking;

    // ------------------------------------
    // GSXR_API_手柄连线类
    // ------------------------------------

    /**
     *  说明：
     *      开始搜索手柄
     *  参数：
     *       [in] controller_handle 手柄设备句柄, 由 open 函数获取
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *  备注：
     *      无
     *  所属：
     *      GSXR_API_手柄连线类
     */
    int32_t (*start_scan_controllers)(gsxr_dev_handle_t controller_handle);

    /**
     *  说明：
     *      获取搜索到的手柄列表
     *  参数：
     *       [in] controller_handle 手柄设备句柄, 由 open 函数获取
     *      [out] controller_list 搜索到的手柄列表
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *  备注：
     *      此函数需介于 start_scan_controllers 及 stop_scan_controllers 之间调用
     *  所属：
     *      GSXR_API_手柄连线类
     */
    int32_t (*get_available_controllers)(gsxr_dev_handle_t controller_handle,
                                         gsxr_controller_list_t* controller_list);

    /**
     *  说明：
     *      停止搜索手柄
     *  参数：
     *       [in] controller_handle 手柄设备句柄, 由 open 函数获取
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *  备注：
     *      无
     *  所属：
     *      GSXR_API_手柄连线类
     */
    int32_t (*stop_scan_controllers)(gsxr_dev_handle_t controller_handle);

    /**
     *  说明：
     *      连接手柄
     *  参数：
     *       [in] controller_handle 手柄设备句柄, 由 open 函数获取
     *       [in] CON_ID 手柄连线识别号, 由搜索到的手柄信息 gsxr_controller_device_t 中获取
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *  备注：
     *      无
     *  所属：
     *      GSXR_API_手柄连线类
     */
    int32_t (*connect)(gsxr_dev_handle_t controller_handle,
                       uint64_t CON_ID);

    /**
     *  说明：
     *      断开手柄
     *  参数：
     *       [in] controller_handle 手柄设备句柄, 由 open 函数获取
     *       [in] CON_ID 手柄连线识别号, 由搜索到的手柄信息 gsxr_controller_device_t 中获取
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *  备注：
     *      无
     *  所属：
     *      GSXR_API_手柄连线类
     */
    int32_t (*disconnect)(gsxr_dev_handle_t controller_handle,
                          uint64_t CON_ID);


    /**
     *  说明：
     *      获取曾配对过的手柄列表
     *  参数：
     *       [in] controller_handle 手柄设备句柄, 由 open 函数获取
     *      [out] controller_list 曾配对过的手柄列表
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *  备注：
     *      无
     *  所属：
     *      GSXR_API_手柄连线类
     */
    int32_t (*get_paired_controllers)(gsxr_dev_handle_t controller_handle,
                                      gsxr_controller_list_t* controller_list);

    // ------------------------------------
    // GSXR_API_手柄信息类
    // ------------------------------------

    /**
     *  说明：
     *      获取手柄设备配置
     *  参数：
     *       [in] controller_handle 手柄设备句柄, 由 open 函数获取
     *       [in] controller_id 手柄设备识别号, 当设备连接时由回调函数 gsxr_dev_event_callback_fn 获取
     *      [out] configs 指向 gsxr_controller_configurations_t 结构体
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *      -4 无效的设备识别号
     *  备注：
     *      gsxr_dev_event_callback_fn 回调 event 为 GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_CONNECTED, param1 即为设备识别号
     *  所属：
     *      GSXR_API_手柄信息类
     */
    int32_t (*get_controller_configurations)(gsxr_dev_handle_t controller_handle,
                                             gsxr_dev_id_t controller_id,
                                             gsxr_controller_configurations_t* configs);

    // ------------------------------------
    // GSXR_API_手柄输入类
    // ------------------------------------

    /**
     *  说明：
     *      获取手柄设备触控模式
     *  参数：
     *       [in] controller_handle 手柄设备句柄, 由 open 函数获取
     *       [in] controller_id 手柄设备识别号, 当设备连接时由回调函数 gsxr_dev_event_callback_fn 获取
     *      [out] enabled touchpad 是否已使能
     *      [out] current_mode 当前触控模式, 为 gsxr_touchpad_mode_t 的位掩码
     *      [out] supported_mode 设备支持的所有触控模式, 为 gsxr_touchpad_mode_t 的位掩码
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *      -4 无效的设备识别号
     *  备注：
     *      gsxr_dev_event_callback_fn 回调 event 为 GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_CONNECTED, param1 即为设备识别号
     *  所属：
     *      GSXR_API_手柄输入类
     */
    int32_t (*get_touchpad_mode)(gsxr_dev_handle_t controller_handle,
                                 gsxr_dev_id_t controller_id,
                                 bool* enabled,
                                 uint32_t* current_mode,
                                 uint32_t* supported_mode);
    /**
     *  说明：
     *      使能手柄设备触控板
     *  参数：
     *       [in] controller_handle 手柄设备句柄, 由 open 函数获取
     *       [in] controller_id 手柄设备识别号, 当设备连接时由回调函数 gsxr_dev_event_callback_fn 获取
     *       [in] touchpad_mode 设置使能的触控模式, 为 gsxr_touchpad_mode_t 的位掩码
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *      -4 无效的设备识别号
     *  备注：
     *      gsxr_dev_event_callback_fn 回调 event 为 GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_CONNECTED, param1 即为设备识别号
     *  所属：
     *      GSXR_API_手柄输入类
     */
    int32_t (*enable_touchpad)(gsxr_dev_handle_t controller_handle,
                               gsxr_dev_id_t controller_id,
                               uint32_t touchpad_mode);

    /**
     *  说明：
     *      设置手柄设备触控模式
     *  参数：
     *       [in] controller_handle 手柄设备句柄, 由 open 函数获取
     *       [in] controller_id 手柄设备识别号, 当设备连接时由回调函数 gsxr_dev_event_callback_fn 获取
     *       [in] touchpad_mode 设置转换的触控模式, 为 gsxr_touchpad_mode_t 的位掩码
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *      -4 无效的设备识别号
     *  备注：
     *      gsxr_dev_event_callback_fn 回调 event 为 GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_CONNECTED, param1 即为设备识别号
     *  所属：
     *      GSXR_API_手柄输入类
     */
    int32_t (*set_touchpad_mode)(gsxr_dev_handle_t controller_handle,
                                 gsxr_dev_id_t controller_id,
                                 uint32_t touchpad_mode);

    /**
     *  说明：
     *      禁用手柄设备触控板
     *  参数：
     *       [in] controller_handle 手柄设备句柄, 由 open 函数获取
     *       [in] controller_id 手柄设备识别号, 当设备连接时由回调函数 gsxr_dev_event_callback_fn 获取
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -4 无效的设备识别号
     *  备注：
     *      gsxr_dev_event_callback_fn 回调 event 为 GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_CONNECTED, param1 即为设备识别号
     *  所属：
     *      GSXR_API_手柄输入类
     */
    int32_t (*disable_touchpad)(gsxr_dev_handle_t controller_handle,
                                gsxr_dev_id_t controller_id);

    // ------------------------------------
    // GSXR_API_手柄输出类
    // ------------------------------------

    /**
     *  说明：
     *      触发手柄振动反馈
     *  参数：
     *       [in] controller_handle 手柄设备句柄, 由 open 函数获取
     *       [in] controller_id 手柄设备识别号, 当设备连接时由回调函数 gsxr_dev_event_callback_fn 获取
     *       [in] haptics 指向 gsxr_haptic_vibration_t 结构体
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *      -4 无效的设备识别号
     *  备注：
     *      gsxr_dev_event_callback_fn 回调 event 为 GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_CONNECTED, param1 即为设备识别号
     *  所属：
     *      GSXR_API_手柄输出类
     */
	int32_t (*trigger_vibration)(gsxr_dev_handle_t controller_handle,
                                 gsxr_dev_id_t controller_id,
                                 const gsxr_haptic_vibration_t* haptics);

    // ------------------------------------
    // GSXR_API_手柄升级类
    // ------------------------------------

    /**
     *  说明：
     *      通知手柄进行固件升级
     *  参数：
     *       [in] controller_handle 手柄设备句柄, 由 open 函数获取
     *       [in] controller_id 手柄设备识别号, 当设备连接时由回调函数 gsxr_dev_event_callback_fn 获取
     *       [in] callback 手柄固件回调函数, 不可为 nullptr
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
     *      GSXR_API_手柄连线类
     */
    int32_t (*upgrade_firmware)(gsxr_dev_handle_t controller_handle,
                                gsxr_dev_id_t controller_id,
                                gsxr_controller_firmware_callback_fn callback,
                                void* cookie);
} gsxr_dev_controller_ops_t;

// 手柄设备实例结构体
typedef struct gsxr_dev_controller {
    gsxr_version                api_version;        // 手柄实例使用的 API 版本号 GSXR_DEVICE_CURRENT_API_VERSION
    gsxr_dev_controller_ops_t*  controller_ops;     // 手柄实例实现的 GSXR 手柄设备函数指针
} gsxr_dev_controller_t;

/**
 *  说明：
 *      获取手柄设备实例
 *  参数：
 *      无
 *  返回值：
 *      手柄设备实例结构体 gsxr_dev_controller_t
 *  备注：
 *      无
 *  所属：
 *      GSXR_API_手柄初始类
 */
gsxr_dev_controller_t* gsxr_dev_get_controller_instance();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* GSXR_DEV_CONTROLLER_H_ */
