//
// Created by fcx on 2020/5/17.
//

#ifndef CLOUDLARKXR_XR_CLIENT_4C_H
#define CLOUDLARKXR_XR_CLIENT_4C_H

#include "lark_xr/types.h"
#include "lark_xr/lk_common_types.h"
#include "lark_xr/lark_xr.h"
#ifdef WIN32
#include <d3d11.h>
#elif __ANDROID__
#include "jni.h"
#endif

#if defined( __cplusplus )
extern "C" {
#endif

/***
* 云雀 VR SDK 主要调用如下.
* larkxr_InitSdkAuthorization------------初始化SDK ID
*  larkxr_InitContext--------------------初始化系统上下文
*  larkxr_InitRenderInfo-----------------初始化渲染信息
*          |
*          |
*  larkxr_IsConnected--------------------是否连接成功
*          |
*          |
*    larkxr_IsFrameInited----------------渲染纹理是否初始
*    larkxr_GetRenderTexurte-------------获取纹理并初始化渲染
*          |
*  larkxr_UpdateDevicePose---------------更新姿态
*  larkxr_UpdateControllerInputState-----更新手柄状态
*          |
*          |-------| loop
*          |       |
*    larkxr_Render-| --------------------渲染。如果没有新的帧应跳过本次渲染
*          |       |
*          |-------|
*          |
*   larkxr_IsConnected------------------连接关闭结束渲染更新循环
*          |
*          |
*   larkxr_ReleaseContext------------------清理系统上下文
*/
//--------------------------------------------------------------------------------------------------
// main apis
// 初始化系统上下文
// 需要先调用 larkxr_InitContext

#ifdef __ANDROID__
void LARK_XR_API larkxr_InitContext(JavaVM *vm);

// should call InitGLShareContext/ReleaseGLShareContext
void LARK_XR_API larkxr_InitContextWithoutGl(JavaVM *vm);
void LARK_XR_API larkxr_InitGLShareContext();
void LARK_XR_API larkxr_ReleaseGLShareContext();
#else
void LARK_XR_API larkxr_InitContext();
void LARK_XR_API larkxr_InitContextDebug(bool is_debug, const char* log_path);
void LARK_XR_API larkxr_InitD3D11Device(ID3D11Device* d3d11_device);
void LARK_XR_API larkxr_ReleaseD3D11Device();
#endif

// 是否启用 debug 模式
void LARK_XR_API larkxr_EnableDebugMode(bool is_debug);

// 清理系统上下文
void LARK_XR_API larkxr_ReleaseContext();

// 注册 sdk
bool LARK_XR_API larkxr_InitSdkAuthorization(const char* id);

// not use.
bool LARK_XR_API larkxr_InitSdkAuthorizationWithSecret(const char* id, const char* secret);

// 查看错误码
int LARK_XR_API larkxr_GetLastError();

// 清理错误码
void LARK_XR_API larkxr_ClearError();

// 检查是否连接成功
bool LARK_XR_API larkxr_IsConnected();

// 检查是否暂停
bool LARK_XR_API larkxr_IsPause();

// 检查是否有帧初始化成功
bool LARK_XR_API larkxr_IsFrameInited();

// 获取渲染纹理。安卓中返回的是 oes texture。
// win32 返回 DXGI_FORMAT_NV12 类型
int LARK_XR_API larkxr_GetRenderTexture();

// 获取渲染

// 解码帧的类型 
// 具体为 XRVideoFrame::FrameType 中的类型
// win32 返回 DXGI_FORMAT_NV12 类型左右眼在一起的纹理
int LARK_XR_API larkxr_GetRenerTextureType();

// 
// 获取硬件解码纹理
larkxrHwRenderTexture LARK_XR_API larkxr_GetHwRenerTexture();

// 清理渲染纹理
void LARK_XR_API larkxr_ClearTexture();

// 检查是否有收到新的帧。也可以调用 larkxr_Render，省略该检查。
bool LARK_XR_API larkxr_HasNewFrame();

// 渲染云端返回的画面。通过参数 trakcingFrame 带回渲染所需的姿态信息。
// 该姿态信息是云端渲染该帧所用的姿态信息。一般用在最终的渲染提交中。
// 如果返回失败，请跳过该次渲染。
bool LARK_XR_API larkxr_Render(larkxrTrackingFrame *trackingFrame);

// unity 调用
bool LARK_XR_API larkxr_Render2(uint64_t *frameIndex, uint64_t *fetchTime, double *displayTime,
                    float *px, float *py, float *pz,
                    float *rx, float *ry, float *rz, float *rw);

// 设置服务器地址
void LARK_XR_API larkxr_SetServerAddr(const char* ip, int port);

// 连接云端应用
void LARK_XR_API larkxr_EnterAppli(const char* appliId);

// 关闭云端应用
void LARK_XR_API larkxr_Close();

// 系统生命周期
// android 创建成功
void LARK_XR_API larkxr_OnCreated();
// android 恢复
void LARK_XR_API larkxr_OnResume();
// android 暂停
void LARK_XR_API larkxr_OnPause();
// android 销毁
void LARK_XR_API larkxr_OnDestory();
// deivces
// 更新设置姿态数据
void LARK_XR_API larkxr_UpdateDevicePose(larkxrDeviceType deviceType, larkxrTrackedPose *pose);

// 更新设置姿态数据
// unity 调用
void LARK_XR_API larkxr_UpdateDevicePose2(larkxrDeviceType deviceType, float px, float py, float pz,
                              float rx, float ry, float rz, float rw);

// 更新手柄状态。包括按钮状态等
void LARK_XR_API larkxr_UpdateControllerInputState(larkxrControllerType controllerType,
                                       larkxrControllerInputState *controllerInputState);

// 更新手柄状态。包括按钮状态等
// unity 调用
void LARK_XR_API larkxr_UpdateControllerInput2(larkxrControllerType controllerType, bool isConnected,
                                   bool touchPadPressed, bool triggerPressed,
                                   bool digitTriggerPressed, bool appMenuPressed,
                                   bool homePressed, bool gripPressed, bool volumUpPressed,
                                   bool volumDownPressed, bool touchPadTouched);

// 更新手柄整体状态，包括姿态和按钮状态。
void LARK_XR_API larkxr_UpdateControllerState(larkxrControllerType controllerType,
                                  larkxrControllerDeviceState *controllerDeviceState);

// 更新设备整体设备状态。
void LARK_XR_API larkxr_UpdateDevicePair(larkxrDevicePair *devicePair);

// 获取当前设备姿态。一般供云雀 sdk 内部调用.
bool LARK_XR_API larkxr_GetDevicePose(larkxrDeviceType deviceType,
                          larkxrTrackedPose *pose, uint32_t ipredictMillsec);

// 获取手柄状态。一般供云雀 sdk 内部调用.
bool LARK_XR_API larkxr_GetControllerInputState(larkxrControllerType deviceType,
                                    larkxrControllerInputState *inputState);

// 发送姿态数据.
void LARK_XR_API larkxr_SendDevicePair(uint64_t frameIndex, uint64_t fetchTime, double displayTime);

// 获取是否为 3dof 或 6dof。**待完善**
bool LARK_XR_API larkxr_DeviceDegreeOfDof(larkxrDeviceType deviceType, larkxrNumDoF *dof);

// 检擦设备是否连接成功
bool LARK_XR_API larkxr_DeviceIsConnected(larkxrDeviceType deviceType);

// 重定位设备，**待完善**
void LARK_XR_API larkxr_Rencenter(larkxrDeviceType deviceType);

// 触发震动, **待完善**
void LARK_XR_API larkxr_TriggerVibrator(larkxrControllerType controllerType, unsigned short duration,
                            larkxrIntensity intensity);

// 同步触发震动, **待完善**
void LARK_XR_API larkxr_SyncTriggerVibrator(larkxrControllerType controllerType, unsigned short duration,
                                larkxrIntensity intensity);

// 获取电池电量,状态信息。**待完善**
void LARK_XR_API larkxr_GetBatteryStatus(larkxrDeviceType deviceType, larkxrBatteryStatus *status);

// 获取电池电量百分比。
uint8_t LARK_XR_API larkxr_GetBatteryStatusNumber(larkxrDeviceType deviceType);

// 设置是否启用震动。**待完善**
void LARK_XR_API larkxr_SetEnableVibrator(bool enable);

// 异步循环.**待完善**
void LARK_XR_API larkxr_AsyncTaskLoop();
//--------------------------------------------------------------------------------------------------
// 在最终渲染时调用，用于收集延时信息。
// 在最终渲染时调用，用于收集延时信息。
void LARK_XR_API larkxr_LatencyCollectorrSubmit(uint64_t frameIndex, float blackDegree);
//--------------------------------------------------------------------------------------------------
// 用户设置
void LARK_XR_API larkxr_SetupUserSetting(larkxrUserSetting *userSeting);

// 读取所有用户设置
void LARK_XR_API larkxr_GetUserSetting(larkxrUserSetting *userSeting);

// 设置比特率
void LARK_XR_API larkxr_SetupBitrateKbps(int bitrate);

// 获取比特率
int LARK_XR_API larkxr_GetBitrateKbps();

// 获取默认比特率
int LARK_XR_API larkxr_GetDefaultBitrateKbps();

// 设置房间高度
void LARK_XR_API larkxr_SetupRoomHeight(float roomHeight);

// 获取房间高度
float LARK_XR_API larkxr_GetRoomHeight();

// 获取默认房间高度
float LARK_XR_API larkxr_GetDefaultRoomHeight();
//--------------------------------------------------------------------------------------------------
// system apis
// 检擦系统信息是否初始化完成
bool LARK_XR_API larkxr_SystemInited();

// 返回系统信息
void LARK_XR_API larkxr_GetSystemInfo(larkxrSystemInfo *info);

// 返回系统信息
// unity 调用
void LARK_XR_API larkxr_GetSystemInfo2(bool *isInited, larkxrSystemType *systemType,
                           larkxrPlatFromType *platFromType, larkxrHeadSetSDKVersion *headSetSdkVersion);

//--------------------------------------------------------------------------------------------------
// render info
// 获取默认配置
void LARK_XR_API larkxr_GetDefaultRenderInfo(larkxrRenderInfo *defaultInfo);

// 获取渲染信息。
void LARK_XR_API larkxr_RenderInfo(larkxrRenderInfo *info);

// 设置渲染信息。
// 请注意渲染的大小和视场角，如果不正确的话会导致画面的畸变。
void LARK_XR_API larkxr_InitRenderInfo(larkxrRenderInfo *renderInfo);

// 设置分辨率
void LARK_XR_API larkxr_SetRenderResolution(int renderWidth, int renderHeight);

// 获取分辨率
void LARK_XR_API larkxr_GetRenderResolution(int *renderWidth, int *renderHeight);

// 获取默认分辨率
void LARK_XR_API larkxr_GetDefaultResolution(int *renderWidth, int *renderHeight);

// 设置瞳距
void LARK_XR_API larkxr_SetIpd(float ipd);

// 获取瞳距
float LARK_XR_API larkxr_GetIpd();

// 获取默认瞳距
float LARK_XR_API larkxr_GetDefaultIpd();

// 设置渲染帧率
void LARK_XR_API larkxr_SetRenderFps(int fps);
// 获取当前渲染帧率
int LARK_XR_API larkxr_GetRenderFps();
// 默认 FPS
int LARK_XR_API larkxr_GetDefaultFps();

// 设置 fov
void LARK_XR_API larkxr_SetFov(larkxrRenderFov *fov, int cout);

// 设置 fov
// unity 用
void
LARK_XR_API larkxr_SetFov2(float eyeLeft_Left, float eyeLeft_Right, float eyeLeft_Top, float eyeLeft_Bottom,
               float eyeRight_Left, float eyeRight_Right, float eyeRight_Top,
               float eyeRight_Bottom);

void
LARK_XR_API larkxr_GetFov2(float *eyeLeft_Left, float *eyeLeft_Right, float *eyeLeft_Top, float *eyeLeft_Bottom,
               float *eyeRight_Left, float *eyeRight_Right, float *eyeRight_Top,
               float *eyeRight_Bottom);

void LARK_XR_API larkxr_GetDefaultFov2(float *eyeLeft_Left, float *eyeLeft_Right, float *eyeLeft_Top,
                           float *eyeLeft_Bottom,
                           float *eyeRight_Left, float *eyeRight_Right, float *eyeRight_Top,
                           float *eyeRight_Bottom);

// set larkHeadSetControllerDesc
void LARK_XR_API lakrxr_SetHeadSetControllerDesc(larkHeadSetControllerDesc* headset_desc);
// get larkHeadSetControllerDesc
larkHeadSetControllerDesc LARK_XR_API lakrxr_GetHeadSetControllerDesc();
// get default head set control
larkHeadSetControllerDesc LARK_XR_API lakrxr_GetDefaultHeadSetControllerDesc();

// set use mutiview. stereo mode only support in android.
void LARK_XR_API larkxr_SetUseMultiview(bool useMulti);
// set flip drawing. only suuport in android.
void LARK_XR_API larkxr_SetFlipDraw(bool flipDraw);

// setup fov rendeing
void LARK_XR_API larkxr_SetFoveatedRendering(larkFoveatedRendering* fovRending);
// get fov rendeing setup
larkFoveatedRendering LARK_XR_API larkxr_GetFoveatedRendering();
larkFoveatedRendering LARK_XR_API larkxr_GetDefaultFoveatedRendering();

// setup 
void LARK_XR_API larkxr_SetColorCorrention(larkColorCorrention* colorCorrection);
larkColorCorrention LARK_XR_API larkxr_GetColorCorrention();
larkColorCorrention LARK_XR_API larkxr_GetDefaultColorCorrention();

// quick config with level
void LARK_XR_API larkxr_QuickConfigWithDefaulSetup(int level);
int LARK_XR_API larkxr_GetDefaultQuickConfigLevel();

// ResolutionScale
void LARK_XR_API larkxr_SetResolutionScale(float scale);
float LARK_XR_API larkxr_GetResolutionScale();

// use h265
void LARK_XR_API larkxr_SetUseH265(bool use);
bool LARK_XR_API larkxr_GetUseH265();

// 
//--------------------------------------------------------------------------------------------------
#if defined( __cplusplus )
}    // extern "C"
#endif

#endif //CLOUDLARKXR_XR_CLIENT_4C_H
