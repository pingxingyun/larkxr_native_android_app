#ifndef GSXR_DEV_SYSTEM_H_
#define GSXR_DEV_SYSTEM_H_

/**
 *  Declare GSXR Copyright here.
 */

#include "gsxr_dev_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// 枚举系统性能等级
typedef enum gsxr_perf_level {
    GSXR_PERF_LEVEL_SYSTEM  = 0,    // 系统预设等级
    GSXR_PERF_LEVEL_HIGH    = 1,    // 高性能等级
    GSXR_PERF_LEVEL_MEDIUM  = 2,    // 中性能等级
    GSXR_PERF_LEVEL_LOW     = 3,    // 低性能等级
} gsxr_perf_level_t;

// 枚举系统日志输出等级
typedef enum gsxr_log_level {
    GSXR_LOG_LEVEL_VERBOSE  = 0,
    GSXR_LOG_LEVEL_DEBUG    = 1,
    GSXR_LOG_LEVEL_INFO     = 2,
    GSXR_LOG_LEVEL_WARN     = 3,
    GSXR_LOG_LEVEL_ERROR    = 4,
} gsxr_log_level_t;


// GSXR 系统平台函数
typedef struct gsxr_dev_sys_ops {
    // 设备通用函数
    gsxr_dev_common_ops_t common;

    // ------------------------------------
    // GSXR_API_系统调控类
    // ------------------------------------
    /**
     *  说明：
     *      设置线程优先级
     *  参数：
     *       [in] sys_handle 系统平台句柄, 由 open 函数获取
     *       [in] tid 线程 thread id
     *       [in] policy 调度策略级. 设置为 SCHED_FIFO 或 SCHED_RR 可获取 real-time 级效能
     *       [in] priority 线程优先级. 设置于 1-99 区间可获取 real-time 级效能 (数字越高, 优先级越高)
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *  备注：
     *      无
     *  所属：
     *      GSXR_API_系统调控类
     */
    int32_t (*set_thread_priority)(gsxr_dev_handle_t sys_handle,
                                   uint32_t tid,
                                   int policy,
                                   int priority);

    /**
     *  说明：
     *      设置 CPU, GPU 效能等级
     *  参数：
     *       [in] sys_handle 系统平台句柄, 由 open 函数获取
     *       [in] cpu_perf_level CPU 系统性能等级, 通常设置为 GSXR_PERF_LEVEL_SYSTEM 采取系统预设等级
     *       [in] gpu_perf_level GPU 系统性能等级, 通常设置为 GSXR_PERF_LEVEL_SYSTEM 采取系统预设等级
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *  备注：
     *      无
     *  所属：
     *      GSXR_API_系统调控类
     */
    int32_t (*set_performance_level)(gsxr_dev_handle_t sys_handle,
                                     gsxr_perf_level_t cpu_perf_level,
                                     gsxr_perf_level_t gpu_perf_level);

    /**
     *  说明：
     *      设置日志信息输出等级
     *  参数：
     *       [in] sys_handle 系统平台句柄, 由 open 函数获取
     *       [in] log_level 系统日志输出等级
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *      -3 无效的输入参数
     *  备注：
     *      无
     *  所属：
     *      GSXR_API_系统调控类
     */
    int32_t (*set_log_level)(gsxr_dev_handle_t sys_handle,
                             gsxr_log_level_t log_level);

    /**
     *  说明：
     *      设置开发者模式
     *  参数：
     *       [in] sys_handle 系统平台句柄, 由 open 函数获取
     *       [in] enable 开启或关闭开发者模式, true 为开启, false 为关闭
     *  返回值：
     *      0  成功
     *      -1 失败
     *      -2 无效的设备句柄
     *  备注：
     *      无
     *  所属：
     *      GSXR_API_系统调控类
     */
    int32_t (*set_developer_mode)(gsxr_dev_handle_t sys_handle,
                                  bool enable);
} gsxr_dev_sys_ops_t;

// 系统平台实例结构体
typedef struct gsxr_dev_sys {
    gsxr_version                api_version;    // 系统实例使用的 API 版本号 GSXR_DEVICE_CURRENT_API_VERSION
    gsxr_dev_sys_ops_t*         sys_ops;        // 系统实例实现的 GSXR 系统平台函数指针
} gsxr_dev_sys_t;

/**
 *  说明：
 *      获取系统平台实例
 *  参数：
 *      无
 *  返回值：
 *      系统平台实例结构体 gsxr_dev_sys_t
 *  备注：
 *      无
 *  所属：
 *      GSXR_API_系统初始类
 */
gsxr_dev_sys_t* gsxr_dev_get_sys_instance();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* GSXR_DEV_SYSTEM_H_ */
