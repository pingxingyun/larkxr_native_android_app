#include <cstdlib>
#include <unistd.h>
#include <pthread.h>
#include <sys/prctl.h>					// for prctl( PR_SET_NAME )
#include <android/window.h>				// for AWINDOW_FLAG_KEEP_SCREEN_ON
#include <android/native_window_jni.h>	// for native window JNI

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <android/log.h>
//#include <android_native_app_glue.h>
#include <wvr/wvr.h>
#include <build_config.h>
#include "log.h"
#include "env_context.h"
#include "wave_application.h"

int main(int argc, char *argv[]) {
	LOGENTRY();
	LOGI("Wave sdk start!");

	WaveApplication* application = new WaveApplication();

	LOGI("HelloVR main, start call app->initVR()");
	if (!application->InitVR()) {
		LOGW("HelloVR main, initVR fail,start call app->shutdownVR()");
		application->ShutdownVR();
		return 1;
	}

	if (!application->InitGL()) {
		LOGW("HelloVR main, initGL failed, start call app->shutdownVR()");
		application->ShutdownGL();
		application->ShutdownVR();
		return 1;
	}

	// init java method
	application->InitJava();

	while (true) {
		if (application->OnUpdate()) {
			break;
		};
	}

	application->ShutdownGL();
	application->ShutdownVR();
	LOGI("--------------------shutdown vr");
	delete application;
	return 0;
}

// extern "C"
extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_cloudlarkxrhtc_MainActivity_init(JNIEnv *env, jobject thiz, jobject am,
											  jstring internal_data_path_,
											  jstring external_data_path_) {
	const char *internalDataPath = env->GetStringUTFChars(internal_data_path_, 0);
	const char *externalDataPath = env->GetStringUTFChars(external_data_path_, 0);

	LOGI("MainActivity_init: call  Context::getInstance()->init");
	JavaVM* vm = nullptr;
	env->GetJavaVM(&vm);
	Context::Init(vm, env, am, thiz, internalDataPath, externalDataPath);
	LOGI("register WVR main when library loading");
	WVR_RegisterMain(main);

	env->ReleaseStringUTFChars(internal_data_path_, internalDataPath);
	env->ReleaseStringUTFChars(external_data_path_, externalDataPath);

	BuildConfig::init(env, "com/pxy/cloudlarkxrhtc/BuildConfig");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_cloudlarkxrhtc_MainActivity_nativeCreated(JNIEnv *env, jobject thiz) {
	WaveApplication* application = (WaveApplication*)WaveApplication::instance();
	LOGV("native created %d", application == nullptr);
	if (application != nullptr) {
		application->OnCreate();
	}
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_cloudlarkxrhtc_MainActivity_nativeResume(JNIEnv *env, jobject thiz) {
	WaveApplication* application = (WaveApplication*)WaveApplication::instance();
	LOGV("native resume %d", application == nullptr);
	if (application != nullptr) {
		application->OnResume();
	}
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_cloudlarkxrhtc_MainActivity_nativePause(JNIEnv *env, jobject thiz) {
	WaveApplication* application = (WaveApplication*)WaveApplication::instance();
	if (application != nullptr) {
		application->OnPause();
	}
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_cloudlarkxrhtc_MainActivity_nativeDestory(JNIEnv *env, jobject thiz) {
	WaveApplication* application = (WaveApplication*)WaveApplication::instance();
	if (application != nullptr) {
		application->OnDestory();

	}
}