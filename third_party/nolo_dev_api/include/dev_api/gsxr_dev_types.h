#ifndef GSXR_DEV_TYPES_H_
#define GSXR_DEV_TYPES_H_

/**
 *  Declare GSXR Copyright here.
 */
 
#include <inttypes.h>
#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------------------
// GSXR_DEV_API_类型定义类
// ------------------------------------

#define GSXR_MAKE_VERSION(major, minor, patch) \
    ((((major) & 0xffffULL) << 48) | (((minor) & 0xffffULL) << 32) | ((patch) & 0xffffffffULL))

#define GSXR_VERSION_MAJOR(version) (uint16_t)(((uint64_t)(version) >> 48) & 0xffffULL)
#define GSXR_VERSION_MINOR(version) (uint16_t)(((uint64_t)(version) >> 32) & 0xffffULL)
#define GSXR_VERSION_PATCH(version) (uint32_t)((uint64_t)(version) & 0xffffffffULL)

#define GSXR_DEVICE_CURRENT_API_VERSION GSXR_MAKE_VERSION(0, 90, 0)

#define GSXR_COMMON_STRING_MAX_SIZE 128

// bits 63-48: 主版本号
// bits 47-32: 次版本号
// bits 31-0:  修订号
typedef uint64_t gsxr_version;

// 设备句柄
typedef void* gsxr_dev_handle_t;

// 设备识别号
typedef uint32_t gsxr_dev_id_t;

// 二维矢量结构体
typedef struct gsxr_vector2f {
    float x;
    float y;
} gsxr_vector2f_t;

// 三维矢量结构体
typedef struct gsxr_vector3f {
    float x;
    float y;
    float z;
} gsxr_vector3f_t;

// 四元数结构体
typedef struct gsxr_quaternionf {
    float x;
    float y;
    float z;
    float w;
} gsxr_quaternionf_t;

// 位姿结构体
typedef struct gsxr_posef {
    gsxr_vector3f_t         position;       // 位置
    gsxr_quaternionf_t      orientation;    // 方向
} gsxr_posef_t;

// 速度结构体
typedef struct gsxr_velocityf {
    gsxr_vector3f_t     linear_velocity;    // 线速度
    gsxr_vector3f_t     angular_velocity;   // 角速度
} gsxr_velocityf_t;


// 枚举设备事件回调类型
typedef enum gsxr_dev_event_callback_type {
    /** 设备状态事件 */
    /** @note param1 为设备识别号(gsxr_dev_id_t) */
    GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_CONNECTED           = 2000, // 设备连接
    GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_DISCONNECTED        = 2001, // 设备断开

    /** @note param1 为设备识别号(gsxr_dev_id_t), param2 为新模式(gsxr_tracking_mode_t), param3 为前模式(gsxr_tracking_mode_t) */
    GSXR_DEV_EVENT_CALLBACK_TYPE_TRACKING_MODE_CHANGED      = 2003, // 设备跟踪模式变换

    /** @note param1 为设备识别号(gsxr_dev_id_t), param2 为新状态(gsxr_tracking_state_t), param3 为前状态(gsxr_tracking_state_t) */
    GSXR_DEV_EVENT_CALLBACK_TYPE_TRACKING_STATE_CHANGED     = 2499, // 跟踪系统状态改变 


    /** 设备输入事件 */
    /** @note param1 为设备识别号(gsxr_dev_id_t), param2 为输入部件识别号(gsxr_input_id_t) */
    GSXR_DEV_EVENT_CALLBACK_TYPE_INPUT_CLICKED              = 3000, // 起始点击
    GSXR_DEV_EVENT_CALLBACK_TYPE_INPUT_UNCLICKED            = 3001, // 结束点击
    GSXR_DEV_EVENT_CALLBACK_TYPE_INPUT_TOUCHED              = 3002, // 起始触控
    GSXR_DEV_EVENT_CALLBACK_TYPE_INPUT_UNTOUCHED            = 3003, // 结束触控
    GSXR_DEV_EVENT_CALLBACK_TYPE_INPUT_LEFT_TO_RIGHT_SWIPED = 3004, // 左至右滑动
    GSXR_DEV_EVENT_CALLBACK_TYPE_INPUT_RIGHT_TO_LEFT_SWIPED = 3005, // 右至左滑动
    GSXR_DEV_EVENT_CALLBACK_TYPE_INPUT_TOP_TO_BOTTOM_SWIPED = 3006, // 上至下滑动
    GSXR_DEV_EVENT_CALLBACK_TYPE_INPUT_BOTTOM_TO_TOP_SWIPED = 3007, // 下至上滑动

} gsxr_dev_event_callback_type_t;

/**
 *  说明：
 *      设备事件回调函数, 当设备触发新事件时回调
 *  参数：
 *       [in] event 设备回调事件类型
 *       [in] param1 回调参数1
 *       [in] param2 回调参数2
 *       [in] param3 回调参数3
 *       [in] cookie 函数回调时带回的预备数值
 *  返回值：
 *      无
 *  备注：
 *      各回调参数按回调事件类型具有不同定义, 请参阅各事件说明进行相应的类型转换, 赋予实值
 *  所属：
 *      GSXR_API_通用函数类
 */
typedef void (*gsxr_dev_event_callback_fn)(gsxr_dev_event_callback_type_t event,
                                           void* param1,
                                           void* param2,
                                           void* param3,
                                           void* cookie);

// GSXR 设备通用函数
typedef struct gsxr_dev_common_ops {
    /**
     *  说明：
     *      开启设备实例, 获取设备句柄
     *  参数：
     *      无
     *  返回值：
     *      设备句柄 gsxr_dev_handle_t
     *  备注：
     *      无
     *  所属：
     *      GSXR_API_通用函数类
     */
    gsxr_dev_handle_t (*open)(void);

    /**
     *  说明：
     *      关闭设备实例
     *  参数：
     *       [in] dev_handle 设备句柄, 由 open 函数获取
     *  返回值：
     *      无
     *  备注：
     *      无
     *  所属：
     *      GSXR_API_通用函数类
     */
    void (*close)(gsxr_dev_handle_t dev_handle);

    /**
     *  说明：
     *      设置事件回调函数, 各设备于指定事件发生时回调
     *  参数：
     *       [in] dev_handle 设备句柄, 由 open 函数获取
     *       [in] callback 事件回调函数指针
     *       [in] cookie 函数回调时带回的预备数值
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的回调函数
     *  备注：
     *      无
     *  所属：
     *      GSXR_API_通用函数类
     */
    int32_t (*set_event_callback)(gsxr_dev_handle_t dev_handle,
                                  gsxr_dev_event_callback_fn callback,
                                  void* cookie);

    /**
     *  说明：
     *      获取设备扩展功能参数值, 设备有特殊扩展功能时使用
     *  参数：
     *       [in] dev_handle 设备句柄, 由 open 函数获取
     *       [in] name 扩展参数名称, 以 '\0' 结尾, 不可为 nullptr
     *   [in,out] value value 不为 nullptr 时, 输出对应 name 的扩展参数值, 以 '\0' 结尾
     *   [in,out] length 当 value 为 nullptr 时, length 输出 value 的字符串长度(包含 '\0')
     *                   当 value 不为 nullptr 时, lenth 输入 value 的字符串长度(包含 '\0')
     *                   lenth 不可为 nullptr
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *  备注：
     *      无
     *  所属：
     *      GSXR_API_通用函数类
     */
    int32_t (*get_param)(gsxr_dev_handle_t dev_handle,
                         const char* name,
                         char* value,
                         uint32_t* length);

    /**
     *  说明：
     *      设置设备扩展功能参数值, 设备有特殊扩展功能时使用
     *  参数：
     *       [in] dev_handle 设备句柄, 由 open 函数获取
     *       [in] name 扩展参数名称, 以 '\0' 结尾, 不可为 nullptr
     *       [in] value 对应 name 的扩展参数值字符串, 以 '\0' 结尾, 不可为 nullptr
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *  备注：
     *      无
     *  所属：
     *      GSXR_API_通用函数类
     */
    int32_t (*set_param)(gsxr_dev_handle_t dev_handle,
                         const char* name,
                         const char* value);

    /**
     *  说明：
     *      设置应用的 JNI JavaVM
     *  参数：
     *       [in] dev_handle 设备句柄, 由 open 函数获取
     *       [in] andros_app_vm 指向应用的 JNI JavaVM
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *  备注：
     *      无
     *  所属：
     *      GSXR_API_通用函数类
     */
    int32_t (*set_app_vm)(gsxr_dev_handle_t dev_handle,
                          void* andros_app_vm);

    void* reserved[32-6];
} gsxr_dev_common_ops_t;

// 设备属性结构体
typedef struct gsxr_dev_properties {
    char    serial_number[GSXR_COMMON_STRING_MAX_SIZE];         // 设备 SN 串号
    char    firmware_version[GSXR_COMMON_STRING_MAX_SIZE];      // 设备 firmware 版本号
    char    vendor_name[GSXR_COMMON_STRING_MAX_SIZE];           // 设备厂家名称
    char    algorithm_version[GSXR_COMMON_STRING_MAX_SIZE];     // 设备算法版本号
    char    algorithm_name[GSXR_COMMON_STRING_MAX_SIZE];        // 设备算法名称
} gsxr_dev_properties_t;

// 设备电池状态结构体
typedef struct gsxr_battery_status {
    float       percentange;        // 电池剩余电量百分比
    bool        charging;           // true 表示电池处于充电状态, false 表示电池未处于充电状态
} gsxr_battery_status_t;

// GSXR 设备信息函数
typedef struct gsxr_dev_info_ops {
    /**
     *  说明：
     *      获取设备属性
     *  参数：
     *       [in] dev_handle 设备句柄, 由 open 函数获取
     *       [in] dev_id 设备识别号, 当设备连接时由回调函数 gsxr_dev_event_callback_fn 获取
     *      [out] properties 指向 gsxr_dev_properties_t 结构体
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *      -4 无效的设备识别号
     *  备注：
     *      gsxr_dev_event_callback_fn 回调 event 为 GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_CONNECTED, param1 即为设备识别号
     *  所属：
     *      GSXR_API_设备信息类
     */
    int32_t (*get_dev_properties)(gsxr_dev_handle_t dev_handle,
                                  gsxr_dev_id_t dev_id,
                                  gsxr_dev_properties_t* properties);

    /**
     *  说明：
     *      获取设备电量状态
     *  参数：
     *       [in] dev_handle 设备句柄, 由 open 函数获取
     *       [in] dev_id 设备识别号, 当设备连接时由回调函数 gsxr_dev_event_callback_fn 获取
     *      [out] battery_status 指向 gsxr_battery_status_t 结构体
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *      -4 无效的设备识别号
     *  备注：
     *      gsxr_dev_event_callback_fn 回调 event 为 GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_CONNECTED, param1 即为设备识别号
     *  所属：
     *      GSXR_API_设备信息类
     */
    int32_t (*get_battery_status)(gsxr_dev_handle_t dev_handle,
                                  gsxr_dev_id_t dev_id,
                                  gsxr_battery_status_t* battery_status);

    void* reserved[16-2];
} gsxr_dev_info_ops_t;


// 枚举设备输入部件
typedef enum gsxr_input_id : uint64_t {
    GSXR_INPUT_ID_0                      = 0x00000001,
    GSXR_INPUT_ID_1                      = 0x00000002,
    GSXR_INPUT_ID_2                      = 0x00000004,
    GSXR_INPUT_ID_3                      = 0x00000008,
    GSXR_INPUT_ID_4                      = 0x00000010,
    GSXR_INPUT_ID_5                      = 0x00000020,
    GSXR_INPUT_ID_6                      = 0x00000040,
    GSXR_INPUT_ID_7                      = 0x00000080,
    GSXR_INPUT_ID_8                      = 0x00000100,
    GSXR_INPUT_ID_9                      = 0x00000200,
    GSXR_INPUT_ID_10                     = 0x00000400,
    GSXR_INPUT_ID_11                     = 0x00000800,
    GSXR_INPUT_ID_12                     = 0x00001000,
    GSXR_INPUT_ID_13                     = 0x00002000,
    GSXR_INPUT_ID_14                     = 0x00004000,
    GSXR_INPUT_ID_15                     = 0x00008000,
    GSXR_INPUT_ID_16                     = 0x00010000,
    GSXR_INPUT_ID_17                     = 0x00020000,
    GSXR_INPUT_ID_18                     = 0x00040000,

    GSXR_INPUT_ID_ALIAS1_SYSTEM          = GSXR_INPUT_ID_0,   // 系统键
    GSXR_INPUT_ID_ALIAS1_MENU            = GSXR_INPUT_ID_1,   // 选单键
    GSXR_INPUT_ID_ALIAS1_GRIP            = GSXR_INPUT_ID_2,   // 抓握键
    GSXR_INPUT_ID_ALIAS1_DPAD_LEFT       = GSXR_INPUT_ID_3,   // 十字键-左
    GSXR_INPUT_ID_ALIAS1_DPAD_UP         = GSXR_INPUT_ID_4,   // 十字键-上
    GSXR_INPUT_ID_ALIAS1_DPAD_RIGHT      = GSXR_INPUT_ID_5,   // 十字键-右
    GSXR_INPUT_ID_ALIAS1_DPAD_DOWN       = GSXR_INPUT_ID_6,   // 十字键-下
    GSXR_INPUT_ID_ALIAS1_VOLUME_UP       = GSXR_INPUT_ID_7,   // 音量键-上
    GSXR_INPUT_ID_ALIAS1_VOLUME_DOWN     = GSXR_INPUT_ID_8,   // 音量键-下
    GSXR_INPUT_ID_ALIAS1_BUMPER          = GSXR_INPUT_ID_9,   // 保险杠键
    GSXR_INPUT_ID_ALIAS1_A               = GSXR_INPUT_ID_10,  // A键
    GSXR_INPUT_ID_ALIAS1_B               = GSXR_INPUT_ID_11,  // B键
    GSXR_INPUT_ID_ALIAS1_X               = GSXR_INPUT_ID_12,  // X键
    GSXR_INPUT_ID_ALIAS1_Y               = GSXR_INPUT_ID_13,  // Y键
    GSXR_INPUT_ID_ALIAS1_BACK            = GSXR_INPUT_ID_14,  // 返回键
    GSXR_INPUT_ID_ALIAS1_ENTER           = GSXR_INPUT_ID_15,  // 输入键
    GSXR_INPUT_ID_ALIAS1_TOUCHPAD        = GSXR_INPUT_ID_16,  // 触摸板
    GSXR_INPUT_ID_ALIAS1_TRIGGER         = GSXR_INPUT_ID_17,  // 扳机
    GSXR_INPUT_ID_ALIAS1_THUMBSTICK      = GSXR_INPUT_ID_18,  // 手摇杆
}
gsxr_input_id_t;

// 枚举设备输入类型
typedef enum gsxr_input_type {
    GSXR_INPUT_TYPE_CLICK       = 0x01,     // 点击类
    GSXR_INPUT_TYPE_TOUCH       = 0x02,     // 触摸类
    GSXR_INPUT_TYPE_ANALOG_1D   = 0x04,     // 类比类 - 一维数据
    GSXR_INPUT_TYPE_ANALOG_2D   = 0x08,     // 类比类 - 二维数据
} gsxr_input_type_t;

// 输入部件信息结构体
typedef struct gsxr_dev_input_info {
    gsxr_input_id_t     input_id;           // 输入部件
    uint8_t             input_type;         // 输入类型的位掩码
} gsxr_dev_input_info_t;

// 输入部件配置结构体
typedef struct gsxr_dev_input_configurations {
    uint32_t                        input_len;      // 输入部件数目
    const gsxr_dev_input_info_t*    input_lists;    // 输入部件信息
} gsxr_dev_input_configurations_t;

// GSXR 设备输入函数
typedef struct gsxr_dev_input_ops {
    /**
     *  说明：
     *      获取设备输入部件之配置
     *  参数：
     *       [in] dev_handle 设备句柄, 由 open 函数获取
     *       [in] dev_id 设备识别号, 当设备连接时由回调函数 gsxr_dev_event_callback_fn 获取
     *      [out] input_configs 指向 gsxr_dev_input_configurations_t 结构体
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *      -4 无效的设备识别号
     *  备注：
     *      gsxr_dev_event_callback_fn 回调 event 为 GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_CONNECTED, param1 即为设备识别号
     *  所属：
     *      GSXR_API_设备输入类
     */
    int32_t (*get_input_configurations)(gsxr_dev_handle_t dev_handle,
                                        gsxr_dev_id_t dev_id,
                                        gsxr_dev_input_configurations_t* input_configs);

    /**
     *  说明：
     *      获取设备输入部件之点击状态
     *  参数：
     *       [in] dev_handle 设备句柄, 由 open 函数获取
     *       [in] dev_id 设备识别号, 当设备连接时由回调函数 gsxr_dev_event_callback_fn 获取
     *       [in] input_id 指定输入部件
     *      [out] click 输出 true 表示已点击, false 表示未点击
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *      -4 无效的设备识别号
     *      -5 无效的设备输入部件
     *  备注：
     *      gsxr_dev_event_callback_fn 回调 event 为 GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_CONNECTED, param1 即为设备识别号
     *  所属：
     *      GSXR_API_设备输入类
     */
    int32_t (*get_input_click_state)(gsxr_dev_handle_t dev_handle,
                                     gsxr_dev_id_t dev_id,
                                     gsxr_input_id_t input_id,
                                     bool* click);

    /**
     *  说明：
     *      获取设备输入部件之触摸状态
     *  参数：
     *       [in] dev_handle 设备句柄, 由 open 函数获取
     *       [in] dev_id 设备识别号, 当设备连接时由回调函数 gsxr_dev_event_callback_fn 获取
     *       [in] input_id 指定输入部件
     *      [out] touch 输出 true 表示已触摸, false 表示未触摸
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *      -4 无效的设备识别号
     *      -5 无效的设备输入部件
     *  备注：
     *      gsxr_dev_event_callback_fn 回调 event 为 GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_CONNECTED, param1 即为设备识别号
     *  所属：
     *      GSXR_API_设备输入类
     */
    int32_t (*get_input_touch_state)(gsxr_dev_handle_t dev_handle,
                                     gsxr_dev_id_t dev_id,
                                     gsxr_input_id_t input_id,
                                     bool* touch);

    /**
     *  说明：
     *      获取设备输入部件之类比数据
     *  参数：
     *       [in] dev_handle 设备句柄, 由 open 函数获取
     *       [in] dev_id 设备识别号, 当设备连接时由回调函数 gsxr_dev_event_callback_fn 获取
     *       [in] input_id 指定输入部件
     *      [out] analog 指向二维矢量 gsxr_vector2f_t 结构体, 数据范围见备注说明
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *      -4 无效的设备识别号
     *      -5 无效的设备输入部件
     *  备注：
     *      gsxr_dev_event_callback_fn 回调 event 为 GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_CONNECTED, param1 即为设备识别号
     *
     *      [analog 数据范围]
     *          a. 若输入部件之输入类型为 GSXR_INPUT_TYPE_ANALOG_1D, analog 数据范围为 0<=x<=1, y=0
     *          b. 若输入部件之输入类型为 GSXR_INPUT_TYPE_ANALOG_2D, analog 数据范围为 -1<=x<=1, -1<=y<=1
     *  所属：
     *      GSXR_API_设备输入类
     */
    int32_t (*get_input_analog_state)(gsxr_dev_handle_t dev_handle,
                                      gsxr_dev_id_t dev_id,
                                      gsxr_input_id_t input_id,
                                      gsxr_vector2f_t* analog);

    void* reserved[32-3];
} gsxr_dev_input_ops_t;


// 枚举跟踪系统状态
typedef enum gsxr_tracking_state {
    GSXR_TRACKING_STATE_UNAVAILABLE     = 0,    // 跟踪系统不可用
    GSXR_TRACKING_STATE_AVAILABLE       = 1,    // 跟踪系统可用
    GSXR_TRACKING_STATE_STARTING        = 2,    // 跟踪系统起始中
    GSXR_TRACKING_STATE_TRACKING        = 3,    // 跟踪系统工作中
    GSXR_TRACKING_STATE_STOPPING        = 4,    // 跟踪系统停止中
    GSXR_TRACKING_STATE_RELOCALIZED     = 5,    /* 跟踪系统重置, 只能在 GSXR_TRACKING_STATE_TRACKING 状态时进行重置,
                                                   重置完毕重新进入 GSXR_TRACKING_STATE_TRACKING 状态 */ 
} gsxr_tracking_state_t;

// 枚举跟踪算法定位模式
typedef enum gsxr_tracking_mode {
    GSXR_TRACKING_MODE_NONE         = 0,        // 无跟踪能力
    GSXR_TRACKING_MODE_ROTATIONAL   = 0x1,      // 具备旋转跟踪能力
    GSXR_TRACKING_MODE_POSITIONAL   = 0x2,      // 具备位置跟踪能力
} gsxr_tracking_mode_t;

// 位姿及速度数据结构体
typedef struct gsxr_pose_data {
    int64_t             timestamp;          // 姿态时间戳
    bool                pose_valid;         // true 表示位姿有效, false 表示位姿无效
    gsxr_posef_t        pose;               // 位姿结构体
    bool                velocity_valid;     // true 表示速度有效, false 表示速度无效
    gsxr_velocityf_t    velocity;           // 速度结构体
} gsxr_pose_data_t;

// 传感器数据结构体
typedef struct gsxr_sensor_data {
    bool                gyro_valid;     // true 表示陀螺仪数据有效, false 表示陀螺仪数据无效
    int64_t             gyro_ts;        // 陀螺仪数据时间戳
    gsxr_vector3f_t     gyro;           // 旋转弧度, 单位 rad/s

    bool                acc_valid;      // true 表示加速度计数据有效, false 表示加速度计数据无效
    int64_t             acc_ts;         // 加速度计数据时间戳
    gsxr_vector3f_t     acc;            // 加速度, 单位 m/s^2

    bool                mag_valid;      // true 表示磁力计数据有效, false 表示磁力计数据无效
    int64_t             mag_ts;         // 磁力计数据时间戳
    gsxr_vector3f_t     mag;            // 磁力强度, 单位 uT
} gsxr_sensor_data_t;

// GSXR 设备跟踪函数
typedef struct gsxr_dev_tracking_ops {
    /**
     *  说明：
     *      获取设备跟踪状态
     *  参数：
     *       [in] dev_handle 设备句柄, 由 open 函数获取
     *       [in] dev_id 设备识别号, 当设备连接时由回调函数 gsxr_dev_event_callback_fn 获取
     *      [out] tracking_state 输出当前跟踪状态
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *      -4 无效的设备识别号
     *  备注：
     *      gsxr_dev_event_callback_fn 回调 event 为 GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_CONNECTED, param1 即为设备识别号
     *  所属：
     *      GSXR_API_设备跟踪类
     */
    int32_t (*get_tracking_state)(gsxr_dev_handle_t dev_handle,
                                  gsxr_dev_id_t dev_id,
                                  gsxr_tracking_state_t* tracking_state);

    /**
     *  说明：
     *      获取设备跟踪模式
     *  参数：
     *       [in] dev_handle 设备句柄, 由 open 函数获取
     *       [in] dev_id 设备识别号, 当设备连接时由回调函数 gsxr_dev_event_callback_fn 获取
     *      [out] current_mode 当前跟踪模式, 为 gsxr_tracking_mode_t 的位掩码
     *      [out] supported_mode 设备支持的所有跟踪模式, 为 gsxr_tracking_mode_t 的位掩码
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *      -4 无效的设备识别号
     *  备注：
     *      gsxr_dev_event_callback_fn 回调 event 为 GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_CONNECTED, param1 即为设备识别号
     *      3DoF 跟踪模式为 GSXR_TRACKING_MODE_ROTATIONAL, 6DoF 跟踪模式为 (GSXR_TRACKING_MODE_ROTATIONAL | GSXR_TRACKING_MODE_POSITIONAL)
     *  所属：
     *      GSXR_API_设备跟踪类
     */
    int32_t (*get_tracking_mode)(gsxr_dev_handle_t dev_handle,
                                 gsxr_dev_id_t dev_id,
                                 uint32_t* current_mode,
                                 uint32_t* supported_mode);

    /**
     *  说明：
     *      开启设备跟踪系统
     *  参数：
     *       [in] dev_handle 设备句柄, 由 open 函数获取
     *       [in] dev_id 设备识别号, 当设备连接时由回调函数 gsxr_dev_event_callback_fn 获取
     *       [in] tracking_mode 设置开启的跟踪模式, 为 gsxr_tracking_mode_t 的位掩码
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *      -4 无效的设备识别号
     *  备注：
     *      gsxr_dev_event_callback_fn 回调 event 为 GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_CONNECTED, param1 即为设备识别号
     *  所属：
     *      GSXR_API_设备跟踪类
     */
    int32_t (*start_tracking)(gsxr_dev_handle_t dev_handle,
                              gsxr_dev_id_t dev_id,
                              uint32_t tracking_mode);

    /**
     *  说明：
     *      停止设备跟踪系统
     *  参数：
     *       [in] dev_handle 设备句柄, 由 open 函数获取
     *       [in] dev_id 设备识别号, 当设备连接时由回调函数 gsxr_dev_event_callback_fn 获取
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -4 无效的设备识别号
     *  备注：
     *      gsxr_dev_event_callback_fn 回调 event 为 GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_CONNECTED, param1 即为设备识别号
     *  所属：
     *      GSXR_API_设备跟踪类
     */
    int32_t (*stop_tracking)(gsxr_dev_handle_t dev_handle,
                             gsxr_dev_id_t dev_id);

    /**
     *  说明：
     *      获取设备姿态数据
     *  参数：
     *       [in] dev_handle 设备句柄, 由 open 函数获取
     *       [in] dev_id 设备识别号, 当设备连接时由回调函数 gsxr_dev_event_callback_fn 获取
     *      [out] pose_data 指向 gsxr_pose_data_t 结构体
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *      -4 无效的设备识别号
     *  备注：
     *      gsxr_dev_event_callback_fn 回调 event 为 GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_CONNECTED, param1 即为设备识别号
     *  所属：
     *      GSXR_API_设备跟踪类
     */
    int32_t (*get_pose_data)(gsxr_dev_handle_t dev_handle,
                             gsxr_dev_id_t dev_id,
                             gsxr_pose_data_t* pose_data);

    /**
     *  说明：
     *      获取设备传感器数据
     *  参数：
     *       [in] dev_handle 设备句柄, 由 open 函数获取
     *       [in] dev_id 设备识别号, 当设备连接时由回调函数 gsxr_dev_event_callback_fn 获取
     *      [out] sensor_data 指向 gsxr_sensor_data_t 结构体
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *      -4 无效的设备识别号
     *  备注：
     *      gsxr_dev_event_callback_fn 回调 event 为 GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_CONNECTED, param1 即为设备识别号
     *  所属：
     *      GSXR_API_设备跟踪类
     */
    int32_t (*get_sensor_data)(gsxr_dev_handle_t dev_handle,
                               gsxr_dev_id_t dev_id,
                               gsxr_sensor_data_t* sensor_data);

    /**
     *  说明：
     *      重置设备跟踪系统原点
     *  参数：
     *       [in] dev_handle 设备句柄, 由 open 函数获取
     *       [in] dev_id 设备识别号, 当设备连接时由回调函数 gsxr_dev_event_callback_fn 获取
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -4 无效的设备识别号
     *  备注：
     *      gsxr_dev_event_callback_fn 回调 event 为 GSXR_DEV_EVENT_CALLBACK_TYPE_DEVICE_CONNECTED, param1 即为设备识别号
     *  所属：
     *      GSXR_API_设备跟踪类
     */
    int32_t (*relocalize_origin)(gsxr_dev_handle_t dev_handle,
                                 gsxr_dev_id_t dev_id);

    void* reserved[32-7];
} gsxr_dev_tracking_ops_t;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* GSXR_DEV_TYPES_H_ */
