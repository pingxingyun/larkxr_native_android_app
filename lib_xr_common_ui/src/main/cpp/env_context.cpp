//
// Created by fcx@pingxingyun.com on 2019/12/1.
//

#include <linux/prctl.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <asm/unistd.h>
#include "env_context.h"
#include "log.h"

using namespace lark;
namespace {
// Return the current thread's name.
    static std::string GetThreadName() {
        char name[17] = {0};
        if (prctl(PR_GET_NAME, name) != 0)
            return std::string("<noname>");
        return std::string(name);
    }
    // Return thread ID as a string.
    static std::string GetThreadId() {
        char buf[21];  // Big enough to hold a kuint64max plus terminating NULL.
        snprintf(buf, sizeof(buf), "%ld", static_cast<long>(syscall(__NR_gettid)));
        return std::string(buf);
    }
}
///
/// EnvWrapper
///
EnvWrapper::EnvWrapper(JavaVM * vm, JNIEnv * env, bool needAttach) :
        vm_(vm), env_(env), need_attach_(needAttach) {
    if (need_attach_) {
        std::string name(GetThreadName() + " - " + GetThreadId());
        JavaVMAttachArgs args;
        args.version = JNI_VERSION_1_6;
        args.name = &name[0];
        args.group = nullptr;
        int ret = vm_->AttachCurrentThread(&env_, &args);
        if (ret != 0) {
            LOGV("AttachCurrentThread failed.");
            env_ = nullptr;
            need_attach_ = false;
        }
    }
}

EnvWrapper::~EnvWrapper() {
    if (need_attach_ && env_ != nullptr) {
        vm_->DetachCurrentThread();
    }
}

///
/// context
///
std::unique_ptr<Context>  Context::s_instance_ = nullptr;
void Context::Init(JavaVM* vm, JNIEnv *env, jobject assetManagerInstance, jobject mainActivityInstance,
                   const char *internalDataPath, const char *externalDataPath) {
    LOGV("Init native activity with jobject");
    s_instance_.reset(new Context(vm, env, assetManagerInstance, mainActivityInstance,
                                  internalDataPath, externalDataPath));
}

void Context::Init(JavaVM* vm, JNIEnv * env, ANativeActivity * activityNative, AAssetManager * assetManager,
        const char * internalDataPath, const char * externalDataPath) {
    LOGV("Init native activity ANativeActivity internalDataPath externalDataPath");
    s_instance_.reset(new Context(vm, env, activityNative, assetManager, internalDataPath, externalDataPath));
}

void Context::Init(ANativeActivity * activityNative) {
    LOGV("Init Context with native activity");
    s_instance_.reset(new Context(activityNative));
}

void Context::Reset() {
    s_instance_.reset();
}

Context::Context(JavaVM *vm, JNIEnv *env, jobject assetManagerInstance,
                 jobject mainActivityInstance, const char *internalDataPath,
                 const char *externalDataPath): vm_(vm)
{
    asset_manager_ = AAssetManager_fromJava(env, assetManagerInstance);
    if (asset_manager_ == nullptr) {
        LOGE("Fail to get AssetManager from Java");
        abort();
    }
    strcpy(internal_data_path_, internalDataPath);
    strcpy(external_data_path_, externalDataPath);

    bitmap_factory_ = std::make_shared<BitmapFactory>(env);

    // Init native activity manual
    native_activity_ = new ANativeActivity();
    native_activity_->sdkVersion = 28;
    native_activity_->instance = mainActivityInstance;
    native_activity_->assetManager = asset_manager_;
    native_activity_->callbacks = nullptr;
    native_activity_->clazz = nullptr;
    native_activity_->env = env;
    native_activity_->vm = vm_;
    native_activity_->internalDataPath = internal_data_path_;
    native_activity_->externalDataPath = external_data_path_;
}

Context::Context(JavaVM* vm, JNIEnv * env, ANativeActivity * activityNative,
                 AAssetManager * assetManager, const char * internalDataPath, const char * externalDataPath)
{
    vm_ = vm;
    native_activity_ = activityNative;
    asset_manager_ = assetManager;
    strcpy(internal_data_path_, internalDataPath);
    strcpy(external_data_path_, externalDataPath);
}

Context::Context(ANativeActivity * activityNative) {
    vm_ = activityNative->vm;
    native_activity_ = activityNative;
    asset_manager_ = activityNative->assetManager;
    strcpy(internal_data_path_, activityNative->internalDataPath);
    strcpy(external_data_path_, activityNative->externalDataPath);

    JNIEnv * env = nullptr;
    vm_->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
    bitmap_factory_ = std::make_shared<BitmapFactory>(env);
}

Context::~Context() {
    EnvWrapper envwrapper = GetEnv();
    JNIEnv * env = envwrapper.get();

    // important. delet global ref manal
    if (bitmap_factory_) {
        bitmap_factory_->Clean(env);
    }
}

EnvWrapper Context::GetEnv() {
    JNIEnv * env = nullptr;
    int stat;
    stat = vm_->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
    if (stat == JNI_EDETACHED) {
        return EnvWrapper(vm_, env, true);
    } else if (stat == JNI_OK) {
        return EnvWrapper(vm_, env, false);
    } else if (stat == JNI_EVERSION) {
        LOGE("GetEnv: version not supported");
    } else {
        LOGE("Could not get java env!");
    }
    return EnvWrapper(vm_, nullptr, false);
}