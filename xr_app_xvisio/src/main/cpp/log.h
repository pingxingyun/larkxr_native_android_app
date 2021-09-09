//
// Created by fcx@pingxingyun.com on 2019/12/1.
//

#ifndef CLOUDLARKVRDEMO_LOG_H
#define CLOUDLARKVRDEMO_LOG_H

#include <android/log.h>

#ifndef LOG_TAG
#define LOG_TAG "lark_xr_app"
#endif

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, __VA_ARGS__)

#define LogD(tag, ...) __android_log_print(ANDROID_LOG_DEBUG, tag, __VA_ARGS__)
#define LogE(tag, ...) __android_log_print(ANDROID_LOG_ERROR, tag, __VA_ARGS__)

#define LOGENTRY(...) utils::LogEntry local_log_entry(LOG_TAG, __func__)
#define LOGLEAVE()

namespace utils {
    class LogEntry {
    private:
        const char * mLogTag;
        const char * mFuncName;
        LogEntry() {
        }
    public:
        LogEntry(const char * logtag, const char * func) : mLogTag(logtag), mFuncName(func) {
            __android_log_print(ANDROID_LOG_DEBUG, mLogTag, " +++%s", mFuncName);
        }
        LogEntry(const char * logtag, const char * func, const char * format, ...) : mLogTag(logtag), mFuncName(func) {
            __android_log_print(ANDROID_LOG_DEBUG, mLogTag, " +++%s", mFuncName);
            if (format != nullptr) {
                va_list args;
                va_start (args, format);
                __android_log_vprint(ANDROID_LOG_DEBUG, mLogTag, format, args);
                va_end(args);
            }
        }
        ~LogEntry() {
            __android_log_print(ANDROID_LOG_DEBUG, mLogTag, " ---%s", mFuncName);
        }
    };
}  // namespace log

#endif // CLOUDLARKVRDEMO_LOG_H