#include <jni.h>
#include <string>
#include <log.h>
#include <env_context.h>
#include <build_config.h>
#include "pvr_application.h"
#include "pvr_utils.h"



extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_cloudlarkxrpico_MainActivity_nativeInit(JNIEnv *env, jobject thiz, jobject am,
                                                     jstring internal_data_path_,
                                                     jstring external_data_path_) {
    const char *internalDataPath = env->GetStringUTFChars(internal_data_path_, 0);
    const char *externalDataPath = env->GetStringUTFChars(external_data_path_, 0);

    LOGI("MainActivity_init: call  Context::getInstance()->init");
    JavaVM* vm = nullptr;
    env->GetJavaVM(&vm);
    Context::Init(vm, env, am, thiz, internalDataPath, externalDataPath);
    LOGI("register WVR main when library loading");

    env->ReleaseStringUTFChars(internal_data_path_, internalDataPath);
    env->ReleaseStringUTFChars(external_data_path_, externalDataPath);

    BuildConfig::init(env, "com/pxy/cloudlarkxrpico/BuildConfig");
}

// application
extern "C"
JNIEXPORT jlong JNICALL
Java_com_pxy_cloudlarkxrpico_MainActivity_nativeApplicationInstance(JNIEnv *env, jobject thiz) {
    return (jlong) PvrApplication::instance();
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_pxy_cloudlarkxrpico_MainActivity_nativeInitApplication(JNIEnv *env, jobject thiz) {
    PvrApplication::InitInstance();
    return (jlong) PvrApplication::instance();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_cloudlarkxrpico_MainActivity_nativeInitJava(JNIEnv *env, jobject thiz, jlong ptr,
                                                         jobject picovr_sdk,
                                                         jobject main_activity,
                                                         jint type) {
    PvrApplication* application = (PvrApplication*)ptr;
    if (application == nullptr) {
        return;
    }
    // 初始化java
    JavaVM* vm;
    env->GetJavaVM(&vm);

    application->InitJava(vm, picovr_sdk, main_activity, type);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_cloudlarkxrpico_MainActivity_nativeReleaseApplication(JNIEnv *env, jobject thiz,
                                                                   jlong ptr) {
    PvrApplication::ReleaseInstance();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_cloudlarkxrpico_MainActivity_nativeInitGl(JNIEnv *env, jobject thiz, jlong ptr, jint eyeWidth, jint eyeHeight) {
    PvrApplication* application = (PvrApplication*)ptr;
    if (application == nullptr) {
        return;
    }
    // 初始化gl
    application->InitGL(eyeWidth, eyeHeight);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_cloudlarkxrpico_MainActivity_nativeDeInitGl(JNIEnv *env, jobject thiz, jlong ptr) {
    PvrApplication* application = (PvrApplication*)ptr;
    if (application == nullptr) {
        return;
    }
    // 初始化gl
    application->deInitGL();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_cloudlarkxrpico_MainActivity_nativeFrameBegin(JNIEnv *env, jobject thiz, jlong ptr,
                                                           jfloatArray orientation,
                                                           jfloatArray position) {
    PvrApplication* application = (PvrApplication*)ptr;
    if (application == nullptr) {
        return;
    }
    jfloat* array_o = env->GetFloatArrayElements(orientation,JNI_FALSE);
    jfloat* array_p = env->GetFloatArrayElements(position,JNI_FALSE);

    pvr::PvrPose pose = {
            {
                    array_o[3], // W
                    array_o[0], // X
                    array_o[1], // Y
                    array_o[2], // Z
            },
            {
                    array_p[0], // x
                    array_p[1], // y
                    array_p[2], // z
            }
    };
    application->FrameBegin(pose);

    env->ReleaseFloatArrayElements(orientation,array_o,JNI_COMMIT);
    env->ReleaseFloatArrayElements(position,array_p,JNI_COMMIT);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_cloudlarkxrpico_MainActivity_nativeFrameEnd(JNIEnv *env, jobject thiz, long ptr) {
    PvrApplication* application = (PvrApplication*)ptr;
    if (application == nullptr) {
        return;
    }
    application->FrameEnd();
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_pxy_cloudlarkxrpico_MainActivity_naitveGetRenderPose(JNIEnv *env, jobject thiz, jlong ptr,
                                                              jfloatArray pose,
                                                              jlongArray time_stamp) {
    PvrApplication* application = (PvrApplication*)ptr;
    if (application == nullptr) {
        return static_cast<jboolean>(false);
    }

    pvr::PvrPose nativePose = {};
    bool res = application->GetRenderPose(nativePose);
    if (!res) {
        return false;
    }

    jfloat* array_pose = env->GetFloatArrayElements(pose, JNI_FALSE);
    // pose, float array length >= 7, in order "x,y,z,w,px,py,pz".
    array_pose[0] = nativePose.rotation.x;
    array_pose[1] = nativePose.rotation.y;
    array_pose[2] = nativePose.rotation.z;
    array_pose[3] = nativePose.rotation.w;

    array_pose[4] = nativePose.position.x;
    array_pose[5] = nativePose.position.y;
    array_pose[6] = nativePose.position.z;

    jlong * array_timeStamp = env->GetLongArrayElements(time_stamp, JNI_FALSE);
    array_timeStamp[0] = nativePose.poseTimeStampNs;
    array_timeStamp[1] = nativePose.poseFetchTimeNs;
    array_timeStamp[2] = nativePose.expectedDisplayTimeNs;

    env->ReleaseFloatArrayElements(pose, array_pose, JNI_COMMIT);

    return static_cast<jboolean>(res);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_cloudlarkxrpico_MainActivity_nativeDraw(JNIEnv *env, jobject thiz, jlong ptr,
                                                     jint eye) {
    PvrApplication* application = (PvrApplication*)ptr;
    if (application == nullptr) {
        return;
    }
    application->Draw(eye);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_cloudlarkxrpico_MainActivity_naitveOnRenderResume(JNIEnv *env, jobject thiz,
                                                               jlong ptr) {
    PvrApplication* application = (PvrApplication*)ptr;
    if (application == nullptr) {
        return;
    }
    application->OnResume();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_cloudlarkxrpico_MainActivity_nativeOnRenderPause(JNIEnv *env, jobject thiz,
                                                              jlong ptr) {
    PvrApplication* application = (PvrApplication*)ptr;
    if (application == nullptr) {
        return;
    }
    application->OnPause();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_cloudlarkxrpico_MainActivity_nativeOnRenderDestory(JNIEnv *env, jobject thiz,
                                                                jlong ptr) {
    PvrApplication* application = (PvrApplication*)ptr;
    if (application == nullptr) {
        return;
    }
    application->OnDestory();
    application->ShutdownGL();
    application->ShutdownVR();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_cloudlarkxrpico_MainActivity_nativeUpdateController(JNIEnv *env, jobject thiz,
                                                                 jlong ptr, jint type,
                                                                 jobject controller) {
    if (controller == nullptr) {
        return;
    }
    PvrApplication* application = (PvrApplication*)ptr;
    if (application == nullptr) {
        return;
    }
    using namespace pvr;
    PvrControllerType controllerType = static_cast<PvrControllerType>(type);
    jclass clazz = env->GetObjectClass(controller);
    // controller methods
    jmethodID mid_serial = env->GetMethodID(clazz, "getSerialNum", "()I");
    jmethodID mid_connectState = env->GetMethodID(clazz, "getConnectState", "()I");
    jmethodID mid_is6dof = env->GetMethodID(clazz, "get6DofAbility", "()I");
    jmethodID mid_battery = env->GetMethodID(clazz, "getBatteryLevel", "()I");
    jmethodID mid_buttonState = env->GetMethodID(clazz, "getButtonState", "(Lcom/picovr/cvclient/ButtonNum;)Z");
    jmethodID mid_triggerNum = env->GetMethodID(clazz, "getTriggerNum", "()I");
    jmethodID mid_touchPad = env->GetMethodID(clazz, "getTouchPad", "()[I");
    jmethodID mid_orientation = env->GetMethodID(clazz, "getOrientation", "()[F");
    jmethodID mid_position = env->GetMethodID(clazz, "getPosition", "()[F");
    jmethodID mid_acc = env->GetMethodID(clazz, "getAcc", "()[F");
    jmethodID mid_agl = env->GetMethodID(clazz, "getAgl", "()[F");
    // cv button enum
    jclass cvControllerButton = env->FindClass("com/picovr/cvclient/ButtonNum");
    jfieldID f_home = env->GetStaticFieldID(cvControllerButton, "home",
                                            "Lcom/picovr/cvclient/ButtonNum;");
    jfieldID f_app = env->GetStaticFieldID(cvControllerButton, "app",
                                           "Lcom/picovr/cvclient/ButtonNum;");
    jfieldID f_click = env->GetStaticFieldID(cvControllerButton, "click",
                                             "Lcom/picovr/cvclient/ButtonNum;");
    jfieldID f_volumeUp = env->GetStaticFieldID(cvControllerButton, "volumeUp",
                                                "Lcom/picovr/cvclient/ButtonNum;");
    jfieldID f_volumeDown = env->GetStaticFieldID(cvControllerButton, "volumeDown",
                                                  "Lcom/picovr/cvclient/ButtonNum;");
    jfieldID f_buttonAX = env->GetStaticFieldID(cvControllerButton, "buttonAX",
                                                "Lcom/picovr/cvclient/ButtonNum;");
    jfieldID f_buttonBY = env->GetStaticFieldID(cvControllerButton, "buttonBY",
                                                "Lcom/picovr/cvclient/ButtonNum;");
    jfieldID f_buttonLG = env->GetStaticFieldID(cvControllerButton, "buttonLG",
                                                "Lcom/picovr/cvclient/ButtonNum;");
    jfieldID f_buttonRG = env->GetStaticFieldID(cvControllerButton, "buttonRG",
                                                "Lcom/picovr/cvclient/ButtonNum;");

    // data.
    jint serial = env->CallIntMethod(controller, mid_serial);
    jint connectState = env->CallIntMethod(controller, mid_connectState);
    jint is6dof = env->CallIntMethod(controller, mid_is6dof);
    jint battery = env->CallIntMethod(controller, mid_battery);
    jint triggerNum = env->CallIntMethod(controller, mid_triggerNum);
    //
    jboolean home = env->CallBooleanMethod(controller, mid_buttonState, env->GetStaticObjectField(cvControllerButton, f_home));
    jboolean app = env->CallBooleanMethod(controller, mid_buttonState, env->GetStaticObjectField(cvControllerButton, f_app));
    jboolean click = env->CallBooleanMethod(controller, mid_buttonState, env->GetStaticObjectField(cvControllerButton, f_click));
    jboolean volumeUp = env->CallBooleanMethod(controller, mid_buttonState, env->GetStaticObjectField(cvControllerButton, f_volumeUp));
    jboolean volumeDown = env->CallBooleanMethod(controller, mid_buttonState, env->GetStaticObjectField(cvControllerButton, f_volumeDown));
    jboolean buttonAX = env->CallBooleanMethod(controller, mid_buttonState, env->GetStaticObjectField(cvControllerButton, f_buttonAX));
    jboolean buttonBY = env->CallBooleanMethod(controller, mid_buttonState, env->GetStaticObjectField(cvControllerButton, f_buttonBY));
    // WARING
    // pico sdk 里 LG 和 RG 是左边的grip和右边的grip的意思。不是左手的grip和右手的grip的意思
    // 左手只有右边grip，右手只有左边grip。
    jboolean grip = controllerType == PvrConTroller_Left ?
                    env->CallBooleanMethod(controller, mid_buttonState, env->GetStaticObjectField(cvControllerButton, f_buttonRG))
                                                         : env->CallBooleanMethod(controller, mid_buttonState, env->GetStaticObjectField(cvControllerButton, f_buttonLG));
    //
    jfloatArray orientation = static_cast<jfloatArray>(env->CallObjectMethod(controller,
                                                                             mid_orientation));
    jfloatArray position = static_cast<jfloatArray>(env->CallObjectMethod(controller,
                                                                          mid_position));
    jintArray touchpad = static_cast<jintArray>(env->CallObjectMethod(controller, mid_touchPad));
    // controller states.
    jfloat* array_o = env->GetFloatArrayElements(orientation,JNI_FALSE);
    jfloat* array_p = env->GetFloatArrayElements(position,JNI_FALSE);
    glm::quat controllerRotation = {};
    controllerRotation.x = -array_o[0];
    controllerRotation.y = -array_o[1];
    controllerRotation.z = array_o[2];
    controllerRotation.w = array_o[3];

    glm::vec3 controllerPosition = {};
    controllerPosition.x = array_p[0];
    controllerPosition.y = array_p[1];
    controllerPosition.z = array_p[2];
    env->ReleaseFloatArrayElements(orientation, array_o, JNI_COMMIT);
    env->ReleaseFloatArrayElements(position, array_p, JNI_COMMIT);

#if 0
    /// WARNING
    /// get j_acc == null
    /// get j_agl == null
//    jfloatArray j_acc = static_cast<jfloatArray>(env->CallObjectMethod(controller, mid_acc));
//    jfloatArray j_agl = static_cast<jfloatArray>(env->CallObjectMethod(controller, mid_agl));
    // controller states.
//    jfloat* array_acc = env->GetFloatArrayElements(j_acc,JNI_FALSE);
//    jfloat* array_agl = env->GetFloatArrayElements(j_agl,JNI_FALSE);

//    glm::vec3 acc = {};
//    acc.x = array_acc[0];
//    acc.y = array_acc[1];
//    acc.z = array_acc[2];
//
//    glm::vec3 agl = {};
//    agl.x = array_agl[0];
//    agl.y = array_agl[1];
//    agl.z = array_agl[2];
//
//    env->ReleaseFloatArrayElements(j_acc, array_acc, JNI_COMMIT);
//    env->ReleaseFloatArrayElements(j_agl, array_agl, JNI_COMMIT);
#endif

    jint* array_touchpad = env->GetIntArrayElements(touchpad, JNI_FALSE);
    glm::vec2 touchpadAxis = {};
    /// d          ! y = 1                 ! x = 255
    /// d          |                       |
    /// d          |                0      |
    /// d  --------|--------> x 1   -------|------> y 255
    /// d          |                       |
    /// d          | -1                    | 0
    /// d        openvr
    touchpadAxis.y = array_touchpad[0] / 255.0F * 2.0F - 1;
    touchpadAxis.x = array_touchpad[1] / 255.0F * 2.0F - 1;
    env->ReleaseIntArrayElements(touchpad, array_touchpad, JNI_COMMIT);

    PvrControllerState controllerState = {
            controllerType,
            {
                    controllerRotation,
                    controllerPosition,
//                    acc,
//                    agl
            },
            {
                    controllerType,
                    battery,                                            // battery
                    static_cast<bool>(connectState),                    // isConnected,
                    static_cast<bool>(is6dof),                          // is6dof,
                    static_cast<bool>(home),                            // home,
                    static_cast<bool>(app),                             // app,
                    static_cast<bool>(click),                           // joyStick,
                    static_cast<bool>(triggerNum == 255),               // trigger,
                    static_cast<bool>(grip),                            // grip,
                    controllerType == PvrConTroller_Left  && buttonAX,  // buttonX,
                    controllerType == PvrConTroller_Right && buttonAX,  // buttonA,
                    controllerType == PvrConTroller_Right && buttonBY,  // buttonB,
                    controllerType == PvrConTroller_Left  && buttonBY,   // buttonY,
                    triggerNum,                                         // triggerNumber
                    touchpadAxis,                                       // joyStickAxis
            }
    };

//    LOGV("update controller triggerNumber %d ax %d by %d touchpad %f touchpad %f B %d X %d t %d grip %d app %d", triggerNum, buttonAX, buttonBY, touchpadAxis.x, touchpadAxis.y,
//         controllerState.input.buttonB, controllerState.input.buttonY, controllerState.input.trigger, grip, app);

    application->UpdateControler(controllerState);

    env->DeleteLocalRef(orientation);
    env->DeleteLocalRef(position);
    env->DeleteLocalRef(touchpad);
//    env->DeleteLocalRef(j_acc);
//    env->DeleteLocalRef(j_agl);
    env->DeleteLocalRef(clazz);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_cloudlarkxrpico_MainActivity_nativeSetControlerBatteryLevel(JNIEnv *env, jobject thiz,
                                                                         jint right, jint left) {
    lark::XRClient::SetControlerBatteryLevel(left, right);
}

/*extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_cloudlarkxrpico_MainActivity_intoApp(JNIEnv *env, jobject thiz, jlong ptr, jobject rtc_params) {
    // TODO: implement intoApp()
    LogE("intoapp","into");

    jclass cls = (*env).GetObjectClass(rtc_params);
    jfieldID jfieldId=env->GetFieldID(cls,"appilId", "Ljava/lang/String;");
    jstring appid= static_cast<jstring>(env->GetObjectField(rtc_params, jfieldId));
    const char *charappid = env->GetStringUTFChars(appid, 0);
    LogE("appid","%s", charappid);
    PvrApplication* application = (PvrApplication*)ptr;
    if (application == nullptr) {
        return;
    }

    application->EnterAppli(charappid);
    env->ReleaseStringUTFChars(appid, charappid);
    env->DeleteLocalRef(cls);
}*/

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_cloudlarkxrpico_MainActivity_nativeIntoApp__JLjava_lang_String_2(JNIEnv *env, jobject thiz,jlong ptr,jstring rtc_params) {
    // TODO: implement intoApp()
    PvrApplication* application = (PvrApplication*)ptr;
    if (application == nullptr) {
        return;
    }
    const char *charappid = env->GetStringUTFChars(rtc_params, 0);
    LogE("appid","%s", charappid);
    application->Set2DUIEnterAppliMode(charappid);
    env->ReleaseStringUTFChars(rtc_params, charappid);
}