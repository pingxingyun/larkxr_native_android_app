//
// Created by fcx@pingxingyun.com on 2019/10/24.
//

#ifndef MY_APPLICATION_UTILS_H
#define MY_APPLICATION_UTILS_H

#include <pthread.h>
#include <vector>
#include <string>
#include <clocale>
#include <jni.h>
#include <stdlib.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "application.h"

// Return micro second.  Should always positive because now is bigger.
#define timeval_subtract(now, last) \
    ((now.tv_sec - last.tv_sec) * 1000000LL + now.tv_usec - last.tv_usec)

#define timeval_subtract_millsecond(now, last) \
    ((now.tv_sec - last.tv_sec) * 1000 + (now.tv_usec - last.tv_usec) / 1000)

namespace utils {

    static inline std::wstring StringToWstring(const std::string & str)
    {
        if (str.empty()) {
            return L"";
        }
        unsigned len = str.size() * 2;
        setlocale(LC_CTYPE, "");
        wchar_t *p = new wchar_t[len];
        mbstowcs(p,str.c_str(),len);
        std::wstring str1(p);
        delete[] p;
        return str1;
    }

    static inline std::string WStringToString(const std::wstring & str)
    {
        if (str.empty()) {
            return "";
        }
        unsigned len = str.size() * 4;
        setlocale(LC_CTYPE, "");
        char *p = new char[len];
        wcstombs(p,str.c_str(),len);
        std::string str1(p);
        delete[] p;
        return str1;
    }

    static inline std::string GetStringFromJNIString(JNIEnv *env, jstring string){
        const char *buf = env->GetStringUTFChars(string, 0);
        std::string ret = buf;
        env->ReleaseStringUTFChars(string, buf);

        return ret;
    }

    static inline uint64_t GetTimestampUs() {
        timeval tv;
        gettimeofday(&tv, nullptr);

        uint64_t Current = (uint64_t)tv.tv_sec * 1000 * 1000 + tv.tv_usec;
        return Current;
    }

    static inline double GetTimeInSeconds()
    {
        struct timespec now;
        clock_gettime( CLOCK_MONOTONIC, &now );
        return ( now.tv_sec * 1e9 + now.tv_nsec ) * 0.000000001;
    }

    static inline int ReadFile(const char* path, char* buffer, size_t size) {
        FILE* file = fopen(path, "r");
        if (file == nullptr) {
            return -1;
        }
        int count = fread(buffer, sizeof(char), size, file);
        fclose(file);
        return count;
    }


    inline std::string GetTimestampMillStr() {
        uint64_t ms = GetTimestampUs() * 1000;
        return std::to_string(ms);
    }

#ifdef LARK_SDK_SECRET
    inline static std::string GetSignature(const std::string& appkey, const std::string& secret, const std::string& utc_millseconds)
    {
        // digestToHex
        return lark::GetSignature(appkey, secret, utc_millseconds);
    }
#else
    inline static std::string GetSignature(const std::string& appkey, const std::string& secret, const std::string& utc_millseconds)
    {
        // digestToHex
        return "";
    }
#endif
}

#endif //MY_APPLICATION_UTILS_H
