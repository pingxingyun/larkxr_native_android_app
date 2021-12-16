/*
 * Copyright 2017 Google Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Copyright (c) 2020, NVIDIA CORPORATION. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <jni.h>

#include "Ar_Demo.h"

extern "C" {

namespace {
// maintain a reference to the JVM so we can use it later.
    static JavaVM *g_vm = nullptr;

    inline jlong jptr(Ar_Demo *native_ar_demo_application) {
        return reinterpret_cast<intptr_t>(native_ar_demo_application);
    }

    inline Ar_Demo *Native(jlong ptr) {
        return reinterpret_cast<Ar_Demo *>(ptr);
    }

}  // namespace

jint JNI_OnLoad(JavaVM *vm, void *) {
    g_vm = vm;
    return JNI_VERSION_1_6;
}

JNIEnv *GetJniEnv() {
    JNIEnv *env;
    jint result = g_vm->AttachCurrentThread(&env, nullptr);
    return result == JNI_OK ? env : nullptr;
}

jclass FindClass(const char *classname) {
    JNIEnv *env = GetJniEnv();
    return env->FindClass(classname);
}

}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_pxy_ar_1huawei_1app_JniInterface_createNativeApplication(JNIEnv *env, jclass clazz,
                                                                  jobject context) {
    // TODO: implement createNativeApplication()
    JavaVM *_vm = nullptr;
    env->GetJavaVM(&_vm);
    jobject g_act = env->NewGlobalRef(context);
    auto *app = new Ar_Demo(_vm, g_act, env);
    if (app == nullptr)
        return 0; // can't do anything more if failed construction.
    app->Init(); // do we need a return value?

    return jptr(app);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_ar_1huawei_1app_JniInterface_onResume(JNIEnv *env, jclass clazz,
                                                   jlong native_application, jobject context,
                                                   jobject activity) {
    // TODO: implement onResume()
    Native(native_application)->OnResume(env, context, activity);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_ar_1huawei_1app_JniInterface_onDisplayGeometryChanged(JNIEnv *env, jclass clazz,
                                                                   jlong native_application,
                                                                   jint display_rotation,
                                                                   jint width, jint height) {
    // TODO: implement onDisplayGeometryChanged()
    Native(native_application)->OnDisplayGeometryChanged(display_rotation, width, height);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_ar_1huawei_1app_JniInterface_onGlSurfaceDrawFrame(JNIEnv *env, jclass clazz,
                                                               jlong native_application) {
    // TODO: implement onGlSurfaceDrawFrame()
    Native(native_application)->OnDrawFrame();

}
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_pxy_ar_1huawei_1app_JniInterface_isArEngineApkInstalled(JNIEnv *env, jclass clazz,
                                                                 jlong native_application,
                                                                 jobject context) {
    // TODO: implement isArEngineApkInstalled()
    return jboolean(Native(native_application)->IsArEngineApkInstalled(env, context));

}
extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_ar_1huawei_1app_JniInterface_onGlSurfaceCreated(JNIEnv *env, jclass clazz,
                                                             jlong native_application) {
    // TODO: implement onGlSurfaceCreated()
    Native(native_application)->OnSurfaceCreated();
}