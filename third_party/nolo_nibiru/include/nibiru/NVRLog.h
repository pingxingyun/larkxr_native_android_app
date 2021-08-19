/*
 * Log.h
 *
 *  Created on: 2016��9��5��
 *      Author: zhang
 */

#ifndef LOG_H_
#define LOG_H_
#include <android/log.h>

#define NLOG_TAG "nvr-ndk"
#define NLOGI(...) __android_log_print(ANDROID_LOG_INFO, NLOG_TAG, __VA_ARGS__)
#define NLOGE(...) __android_log_print(ANDROID_LOG_ERROR, NLOG_TAG, __VA_ARGS__)
#define NLOGD(...) __android_log_print(ANDROID_LOG_DEBUG, NLOG_TAG, __VA_ARGS__)

#endif /* LOG_H_ */
