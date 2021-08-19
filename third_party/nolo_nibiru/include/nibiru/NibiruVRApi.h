/*
 * NibiruVRApi.h
 *
 *  Created on: 2016��9��5��
 *      Author: zhang
 */

#ifndef NIBIRUVRAPI_H_
#define NIBIRUVRAPI_H_
#include <jni.h>
#include "NVRLog.h"
#include "matrix.h"
#include "nvr_action.h"

#define EYE_NUM 2

namespace nvr {

typedef int (*__init_nvr_service)(void*, void*, void*);
typedef void (*__on_attach_window)();
typedef void (*__on_detach_window)();
typedef void (*__on_resume)();
typedef void (*__on_pause)();
typedef void (*__destory_nvr_service)();

typedef void (*__get_head_pose)(void*);
typedef int (*__prepare_draw)(void*, void*);
typedef int (*__prepare_draw2)(void*, void*, void*);
typedef int (*__prepare_draw_eye)(int);
typedef void (*__finish_draw_eye)(int);
typedef void (*__finish_draw)();

typedef void (*__set_sys_vr_mode)(int);

typedef void (*__tracker_lock)();
typedef void (*__tracker_unlock)();

typedef int (*__get_input_key)(int);

typedef void (*__reg_handle_nkey)();
typedef void (*__unreg_handle_nkey)();

typedef void (*__set_enable_distortion)(int);

typedef void (*__on_surface_created)();
typedef void (*__on_surface_changed)(int, int);

//ndk_setParamI
typedef void (*__set_param_i)(int, int);

//ndk_getParamI
typedef int (*__get_param_i)(int);

typedef void (*__set_param_fa)(int, float*);

typedef void (*__get_nvr_config)(float*);

// ndk_getVersionInfo : [249,20181130,14983]
typedef void (*__get_version_info)(int*);

// controller
typedef void (*__getControllerRotPos)(NACTION_HAND_TYPE, float *, int);
typedef bool (*__getControllerTouchDown)(NACTION_HAND_TYPE);
typedef bool (*__getControllerTouchUp)(NACTION_HAND_TYPE);
typedef bool (*__getControllerTouchPressed)(NACTION_HAND_TYPE);
typedef void (*__getControllerTouchPosition)(NACTION_HAND_TYPE, vec2 &);
typedef bool (*__getButtonDown)(NACTION_HAND_TYPE, KEY_CODE);
typedef bool (*__getButtonUp)(NACTION_HAND_TYPE, KEY_CODE);
typedef bool (*__getButtonPressed)(NACTION_HAND_TYPE, KEY_CODE);
// 0=left,1=right,2=left+right,-1=none
typedef int (*__getContollerConnectStatus)();
typedef void (*__resetController)();
// controller

// predictive head pose
typedef void (*__get_predictive_head_pose)(void*,float);

struct nvr_Eye{
	int pos;
	mat4 rotation;
	mat4 eyeView;
	mat4 eyeRotationView;
	mat4 projection;

	int viewPort[4];

};


#define NVR_MODE_NVR 		0
#define NVR_MODE_2D			1

#define NVR_KEY_ENTER		23
#define NVR_KEY_LEFT		21
#define NVR_KEY_RIGHT		22
#define NVR_KEY_UP			19
#define NVR_KEY_DOWN		20
#define NVR_KEY_BACK		4
#define NVR_KEY_NKEY		110
#define NVR_KEY_NONE		-1

#define  NVR_PROJECTION_NEARFAR 	1
#define  NVR_ENABLE_GAZE 			2
#define  NVR_RENDER_MODE 			6
#define  NVR_GAME_MODE				16
#define  NVR_DISPLAY_QUALITY		18
#define  NVR_DTR_SUPPORT			19
#define  NVR_ENABLE_FPS				20
#define  NVR_LOCK_SENSOR			21
#define  NVR_GAZE_COLOR				22
#define  NVR_GAZE_SIZE				23
#define  NVR_GAZE_DISTANCE			24
#define  NVR_GAZE_SUPPORT			25

#define  NVR_MULTIVIEW_ENABLE 		26
#define  NVR_MULTIVIEW_SUPPORT  	27

#define  NVR_VIDEO_MODE  			28
#define  NVR_REQUEST_RENDER			29

#define NVR_MULTIVIEW_2_SUPPORT   	30

#define NVR_DIRECT_RENDER			31
#define NVR_SUPPORT_DIRECT_RENDER	32

#define NVR_SUPPORT_6DOF                        48

#define NVR_GET_SDK_VERSION    17

#define NVR_FBO_HEAD_VIEW                        74
#define NVR_ENABLED_INTERACTION_SDK              80
#define NVR_LEFT_CONTROLLER_BATTERY_LEVEL  92
#define NVR_RIGHT_CONTROLLER_BATTERY_LEVEL  93
#define NVR_CONTROLLER_HAND_MODE    94

#define  NVR_LOCK_CUR				0
#define  NVR_LOCK_FRONT				1

#define NVR_DISPLAY_QUALITY_DEFAULT 0
#define NVR_DISPLAY_QUALITY_HIGH 	1
#define NVR_DISPLAY_QUALITY_LOW 	2

#define GAZE_SIZE_DEFAULT 	0
#define GAZE_SIZE_LARGE 	1
#define GAZE_SIZE_MEDIUM 	2
#define GAZE_SIZE_SMALL 	3

#define  RENDER_MODE_DEMAND 		0
#define  RENDER_MODE_CONTINUOUS		1

class NibiruVRApi {
public:
	NibiruVRApi();
	virtual ~NibiruVRApi();

	int nvr_Initialize(JNIEnv* env, jobject &surfaceView, jobject &activity);

	bool isDTRSupport();

	bool isSupport6Dof();

	// 1=rotation, 2=position
	void setTrackingMode(int mode);

	void nvr_AttachWindow();

	void nvr_DetachWindow();

	void nvr_OnPause();

	void nvr_OnResume();

	void nvr_GetHeadPose(mat4 *pose);

	int nvr_PrepareDraw(mat4 *pose, nvr_Eye* eyes);

	int nvr_PrepareDraw2(mat4 *pose, float * position, nvr_Eye* eyes);

	int nvr_PrepareDrawEye(int eye);

	void nvr_FinishDrawEye(int eye);

	void nvr_FinishDraw();

	void nvr_SetSystemVRMode(int mode);

	void nvr_lockTracker(int method);

	void nvr_unlockTracker();

	int nvr_GetInputKey(int keycode);

	void nvr_RegHandleNKey();

	void nvr_UnRegHandleNKey();

	void nvr_SetEnableDistortion(bool isEnable);


	void nvr_OnSurfaceCreated();

	void nvr_OnSurfaceChanged(int w, int h);

	void nvr_Destory();

	void nvr_setGameMode(bool mode);

	void nvr_setDisplayQuality(int level);

	void nvr_SetEnableFPS(bool isEnable);

	bool nvr_ShowGaze();

	void nvr_HideGaze();

	void nvr_SetGazeDistance(float distance);

	void nvr_SetGazeColor(float r, float g, float b);

	void nvr_SetGazeSize(int size);

	void nvr_StartVideoMode();

	void nvr_StopVideoMode();

	bool nvr_IsVideoMode();

	void nvr_NotifyVideoFrameUpdate();

	void nvr_SetRenderMode(int mode);

	int nvr_GetRenderMode();

	void nvr_RequestRender();

	bool nvr_IsSupportMultiView();

	bool nvr_IsSupportMultiView2();

	void nvr_SetEnableMultiView(bool isEnable);

	bool nvr_IsEnableMultiView();

	bool nvr_IsSupportDirectRender();

	void nvr_SetEnableDirectRender(bool isEnable);

	bool nvr_IsEnableDirectRender();

	void nvr_UpdateNearFar(float near,float far);

	void nvr_GetCurrentNearFar(float* values);

	void nvr_GetCurrentConfig(float* values);

	 // Controller
	 // hand代表左/右手，hand=0->左手，hand=1->右手，float数组为长度为4的四元数+长度为3的位移
	 // 四元数的顺序: x-y-z-w
	 void nvr_getControllerRotPos(NACTION_HAND_TYPE hand, float *dest, int length);

	 bool nvr_getControllerTouchDown(NACTION_HAND_TYPE handType);

	 bool nvr_getControllerTouchUp(NACTION_HAND_TYPE handType);

	 bool nvr_getControllerTouchPressed(NACTION_HAND_TYPE handType);

	 void nvr_getControllerTouchPosition(NACTION_HAND_TYPE handType, vec2 &position);

	 bool nvr_getControllerButtonDown(NACTION_HAND_TYPE handType, KEY_CODE keyCode);

	 bool nvr_getControllerButtonUp(NACTION_HAND_TYPE handType, KEY_CODE keyCode);

	 bool nvr_getControllerButtonPressed(NACTION_HAND_TYPE handType, KEY_CODE keyCode);

	 void nvr_resetController();

	 int nvr_getContollerConnectStatus();

	 void nvr_getSDKVersion(int* versionInfo);
	// Controller

	 void nvr_setFramePose(float* poseData);
	 void nvr_getPredictiveHeadPose(mat4 *pose, float predictiveTimeMS);
private:
	void* handle;

	char libName[32];

	bool isSupportGaze ;

	bool isLocalLib ;

	int initAPIs();

	__init_nvr_service init_nvr_service ;

	__on_attach_window	on_attach_window ;
	__on_detach_window	on_detach_window ;

	__on_pause 			on_pause ;
	__on_resume			on_resume ;

	__get_head_pose		get_head_pose ;

	__prepare_draw_eye	prepare_draw_eye ;
	__finish_draw_eye	finish_draw_eye ;

	__prepare_draw		prepare_draw ;
	__prepare_draw2		prepare_draw2 ;
	__finish_draw		finish_draw ;

	__set_sys_vr_mode	set_vr_mode ;

	__tracker_lock		tracker_lock ;
	__tracker_unlock	tracker_unlock ;

	__get_input_key		get_input_key ;

	__reg_handle_nkey	reg_handle_nkey ;
	__unreg_handle_nkey	unreg_handle_nkey ;

	__set_enable_distortion	set_enable_distortion ;

	__on_surface_created	on_surface_created ;
	__on_surface_changed	on_surface_changed ;

	__destory_nvr_service	destory_nvr_service ;

	__set_param_i			set_param_i ;

	__get_param_i			get_param_i ;

	__set_param_fa			set_param_fa ;

	__get_nvr_config   get_nvr_config ;

	__get_version_info getVersionInfo;
	// controller
	__getControllerRotPos getControllerRotPos;
	__getControllerTouchDown getControllerTouchDown;
	__getControllerTouchUp getControllerTouchUp;
	__getControllerTouchPressed getControllerTouchPressed;
	__getControllerTouchPosition getControllerTouchPosition;
	__getButtonDown getControllerButtonDown;
	__getButtonUp getControllerButtonUp;
	__getButtonPressed getControllerButtonPressed;
	// 0=left,1=right,2=left+right,-1=none
	__getContollerConnectStatus getContollerConnectStatus;
	__resetController resetController;
	// controller
	__get_predictive_head_pose getPredictiveHeadPose;
};

} /* namespace nvr */

#endif /* NIBIRUVRAPI_H_ */
