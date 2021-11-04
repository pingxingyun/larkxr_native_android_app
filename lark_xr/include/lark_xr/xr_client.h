//
// Created by fcx@pingxingyun.com on 2020/2/26.
//

#ifndef CLOUDLARKXR_XR_CLIENT_H
#define CLOUDLARKXR_XR_CLIENT_H

#include "lark_xr/lark_xr.h"
#include "lark_xr/types.h"
#include "lark_xr/xr_tracking_frame.h"
#include "lark_xr/lk_common_types.h"
#include "request/vr_client.h"
#include "request/report_resource.h"
#include "request/vrclient_heartbeat.h"

#ifdef __ANDROID__
#include <jni.h>
#elif WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <d3d11.h>
#endif


namespace lark {
/**
 * larkxr client 回调函数。
 * 需要注意所有回调函数都不保证在 opengl 渲染线程下调用。需要注意需要在 opengl 渲染线程的操作。
 */
class XRClientObserver {
public:
	/**
	 * lark xr sdk 授权检测失败时回调。
	 * @param code sdk 授权错误码。@see larkEventTypes
	 * @param msg 授权失败具体错误信息
	 */
	virtual void OnSDKAuthorizationFailed(int code, const std::string& msg) = 0;
	/**
	 * 连接服务器成功时回调
	 */
	virtual void OnConnected() = 0;
	/**
	 *  与服务器连接关闭时回调
	 * @param code 关闭原因 枚举原因@see larkEventTypes。一般有：
	 *             LK_XR_UDP_CHANNEL_USER_CANCEL 用户主动关闭
	 *             LK_PROXY_SERVER_CLOSE 与代理服务器的连接关闭
	 *             LK_RENDER_SERVER_CLOSE 与渲染服务器的连接关闭
	 *             LK_XR_MEDIA_TRANSPORT_CHANNEL_CLOSED 媒体socket通道关闭
	 */
	virtual void OnClose(int code) = 0;
	/**
	 * 媒体 udp 通道关闭时回调。当vr头盔待机时，xrclient 将主动关闭媒体连接减少耗电和流量。
	 */
	virtual void OnStreamingDisconnect() = 0;
	/**
	 * 返回一般事件信息
	 * @param infoCode  事件码在@see larkEventTypes 枚举中
	 * @param msg 信息说明,可能为空
	 */
	virtual void OnInfo(int infoCode, const std::string& msg) = 0;
	/**
	 * 出现错误时回调。当出现错误时当前连接已不可用。连接将断开。
	 * @param errCode 事件码在@see larkEventTypes 枚举中
	 * @param msg 错误信息。
	 */
	virtual void OnError(int errCode, const std::string& msg) = 0;
	/**
	 * 当使用软解码视频解码成功时回调。
	 * Android 平台固定使用硬件解码。不会回调该函数。
	 */
	virtual void OnMediaReady() = 0;
	/**
	 * multiview 渲染模式下硬件解码成功时回调。
	 * 当 XRConfig::use_multiview=true; 时启用该模式,该模式为默认模式
	 * 返回 opengl GL_TEXTURE_2D 类型纹理。云端渲染好的左右眼图像在同一张纹理上。
	 * --------------
	 * |  L   |  R  |
	 * --------------
	 * 图像如上图所示。该纹理在 larkxr sdk 更新。
	 * 该回调每次连接成功之后回调一次。
	 * 之后收到OnTrackingFrame回调时，将 tracking frame 用于 atw 渲染该纹理。
	 * @param nativeTextrure opengl 纹理 id
	 */
	virtual void OnMediaReady(int nativeTextrure) = 0;
	/**
	 * stereo 渲染模式下硬件解码成功时回调。
	 * 当 XRConfig::use_multiview=false; 时启用该模式
	 * 返回 opengl GL_TEXTURE_2D 类型纹理。分别为云端渲染好的左右眼图像。
	 * ------------------------     -----------------------
	 * |  nativeTextrureLeft  |    |  nativeTextureRight  |
	 * -----------------------     ------------------------
	 * 图像如上图所示。该纹理在 larkxr sdk 更新。
	 * 该回调每次连接成功之后回调一次。
	 * 之后收到OnTrackingFrame回调时，将 tracking frame 用于 atw 渲染该纹理。
	 * @param nativeTextrure opengl 纹理 id
	 */
	virtual void OnMediaReady(int nativeTextrureLeft, int nativeTextureRight) = 0;
	/**
	 * 收到软件解码好的一帧。
	 * 包括 tracking 数据帧和视频帧。
	 * 只会在使用软解码的系统回调。
	 * @param frame tracking 数据帧和视频帧
	 */
	virtual void OnTrackingFrame(std::unique_ptr<XRTrackingFrame>&& frame) = 0;
	/**
	 * 收到的云端渲染好的 tracking 数据帧，只在使用硬件解码器的系统回调。
	 * 该 tracking 帧用于 vr 头盔进行 atw 渲染。在未收到 tracking 帧的情况下应跳过渲染循环。
	 * 具体 tracking 数据的使用方法根据不同的 vr 头盔sdk 不同，应参考附带的demo中的使用方法。
	 * 由于云渲染是完全异步的过程，该 tracking 数据是之前发送给云端的 tracking 数据而不是当前最新的，
	 * 请不要直接使用从 vr 头盔 sdk 中获取的最新的 tracking 数据渲染云端纹理。
	 * @param trackingFrame tracking 数据帧。
	 */
	virtual void OnTrackingFrame(const larkxrTrackingFrame& trackingFrame) = 0;
	/**
	 * xrclient 请求获取 tracking 数据。
	 * xrclient 连接成功之后将已固定的频率请求 tracking 数据。
	 * 在该回调中将从 vr 头盔中获取到的 tracking 数据发送给 xrclient。
	 */
	virtual void RequestTrackingInfo() = 0;
	/**
	 * 收到云雀后台分配的客户端 id。用于后台区分客户端和后台启动应用管理等操作。
	 * @param clientId 客户端 id。
	 */
	virtual void OnClientId(const std::string& clientId) = 0;
	/**
	 * 收到手柄震动请求
	 * 需要注意不同 vr 头盔手柄震动调用方式不同，需要区别对待。具体用法参考相关 demo
	 * @param isLeft 是否是左手柄
	 * @param startTime 开始时间
	 * @param amplitude 震动强度
	 * @param duration 震动持续时间
	 * @param frequency 震动频率
	 */
	virtual void OnHapticsFeedback(bool isLeft, uint64_t startTime, float amplitude, float duration, float frequency) = 0;
	/**
	 * 请求同步玩家区域数据
	 */
	 virtual void OnSyncPlayerSpace(larkxrPlaySpace* playSpace) = 0;
	 /**
	  * 数据通道开启
	  */
	  virtual void OnDataChannelOpen() = 0;
    /**
    * 数据通道关闭
    */
    virtual void OnDataChannelClose() = 0;
	/**
	* 收到二进制数据
	*/
	virtual void OnDataChannelData(const char* buffer, int length) = 0;
	/**
	* 收到字符数据
	*/
	virtual void OnDataChannelData(const std::string& data) = 0;
};

class XRClientImp;
class LARK_XR_API XRClient
{
public:
	/**
	 * 获取系统信息。系统信息在初始化成功后可用。
	 * 包括系统类型，平台类型（vr头盔的类型），vrsdk类型（应使用的 vr 头盔sdk），sdk 版本号等信息
	 * @return 系统信息
	 */
	static const larkxrSystemInfo& system_info();
	/**
	 * 设置系统信息。内部初始化时调用
	 * @param systemInfo 系统信息
	 */
	static void set_system_info(const larkxrSystemInfo& systemInfo);
	/**
	 * 设备内存信息。
	 * @return 设备内存信息
	 */
	static const larkxrDeviceMemInfo& device_mem_info();
	/**
	 * 设置设备内存信息吗，larkxr Sdk 内部调用
	 * @param deviceMemInfo
	 */
	static void set_device_mem_info(const larkxrDeviceMemInfo& deviceMemInfo);
	/**
	 * 当前服务器地址
	 * @return 服务器地址
	 */
    static std::string GetServerHost();
    /**
     * 当前服务器端口号
     * @return 服务器端口号
     */
    static uint16_t GetServerPort();
    /*
     * http 请求超时时间
     */
    static void SetHttpRequestTimeoutInSeconds(int timeoutInSeconds);
    /**
     * 设置客户端管理接入凭证; 云雀后台系统设定接入管理中设置
     * @param appKey admin_key
     * @param appSecret admin_secret
     */
    static void SetCertificate(const std::string& appKey, const std::string& appSecret);
    /**
     * 头盔电量信息 百分比
     * @param level
     */
	static void SetHmdBatteryLevel(int level);
	/**
	 * 手柄电量信息 百分比
	 * @param left
	 * @param right
	 */
	static void SetControlerBatteryLevel(int left, int right);
	/**
	 * 手柄电量
	 * @param isLeft
	 * @param level
	 */
	static void SetControlerBatteryLevel(bool isLeft, int level);
	/**
	 * 设置设备的电量信息
	 * @param batteryInfo
	 */
	static void SetBatteryInfo(larkxrDeviceBatteryInfo batteryInfo);
	/**
	 * 获取设备电量信息
	 * @return
	 */
	static larkxrDeviceBatteryInfo battery_info();

	XRClient();
	~XRClient();

	/**
	 * 初始化 larkxr SDK，应在larkxr 系统初始化完成之后调用。如果初始化失败将返回false。
	 * 可通过 last_error_code 和 last_error_message 获取错误信息。
	 * 授权 id 应从商务处获取正确的 id。
	 * APP 首次启动将连接网络进行授权验证。授权成功之后在授权有效期内可不连接外网。
	 * @param id 授权id
	 * @return 成功 true 失败返回 false
	 */
	bool InitSdkAuthorization(const std::string& id);

	/**
	 * @deprecated
	 */
	bool InitSdkAuthorization(const std::string& id, const std::string& sig, const std::string& timestamp);
#ifdef __ANDROID__
	/**
	 * 安卓平台初始化 sdk。
	 * @param vm java 虚拟机
	 * @param init_share_context 是否立即初始化 opengl 上下文。如果调用 init 方法不在渲染线程或渲染环境
	 * 可能会变动的情况下，init_share_context 传入 false，然后手动调用 InitGLShareContext。如果渲染线程
	 * 不会变化可以直接在 init 方法初始化。
	 */
	void Init(JavaVM* vm, bool init_share_context = true);
	/**
	 * 初始化opengl共享上下文
	 * 必须在 opengl渲染线程中调用
	 */
	void InitGLShareContext();
	/**
	 *
	 */
	void ReleaseGLShareContext();
#elif WIN32
	/**
	 * 初始化sdk
	 */
	void Init(bool debug_mode = false, const std::string& debug_path = "");
	/*
	* 初始化 d3d11 device，用于解码输出共享纹理
	*/
	void InitD3D11Device(ID3D11Device* device);
	/*
	* 释放 d3d11 devices
	*/
	void ReleaseD3D11Device();
#else
#error "SDK 目前只支持 win平台和 android 平台"
#endif
	// 是否打印 debug 日志。
	// 目前只在 win 平台上起作用
	void EnableDebugMode(bool enable_debug_mode);

	/**
	 * 释放资源
	 */
	void Release();

	/**
	 * 系统生命周期相关函数。
	 */
	 // 系统创建时调用。如果此时配置了云雀服务器地址，将在后台上线。
	void OnCreated();
	// 系统从休眠中恢复时调用，如果此时配置了云雀服务器地址，将在后台下线。
	void OnResume();
	// 系统休眠时调用，如果此时配置了云雀服务器地址，将在后台下线。
	void OnPause();
	// 系统销毁时调用。如果此时配置了云雀服务器地址，将在后台下线。
	void OnDestory();

	/**
	 * 设置云雀服务器地址
	 * @param ip 云雀服务器 ip
	 * @param port 云雀服务器端口号
	 */
	void SetServerAddr(const std::string& ip, int port);

	/**
	 * 注册回调函数
	 * @param observer 回调函数指针
	 */
	void RegisterObserver(XRClientObserver* observer);
	/**
	 * 取消注册回调函数
	 */
	void UnRegisterObserver();

	/**
	 * 使用应用 id 进入云端应用。
	 * 云端应用 id 从应用列表接口回调处获取。
	 * @param appliId 云端应用id
	 */
	void EnterAppli(const std::string& appliId);
	/**
	 * 使用手动配置进入云端应用。一般情况下不需要手动调用该接口。
	 * @param config 完整配置。
	 */
	void Connect(const CommonConfig& config);
	/**
	 * 主动关闭与云端的连接
	 */
	void Close();
	/**
	 * 立即发送设备的姿态数据信息并缓存下来。
	 * @param devicePair 设备的姿态数据信息。
	 */
	void SendDevicePair(const larkxrTrackingDevicePairFrame& devicePair);
	/**
	 * 发送上一次缓存的姿态数据
	 */
	void SendDevicePair();
	/**
	 * 发送音频数据给云端
	 * @param buffer
	 * @param length
	 */
	void SendAudioData(const char* buffer, int length);
	/**
	 * 发送自定义数据给云端应用
	 * @param buffer
	 * @param length
	 */
	void SendData(const char* buffer, int length);
	/**
	 * 发送自定义数据给云端应用
	 * @param buffer
	 * @param length
	 */
	void SendData(const std::string& data);
	/**
	 * 检测是否收到新的帧
	 * @return
	 */
	bool HasNewFrame();
	/**
	 * 渲染一个 tracking 帧，只有当当前有未渲染的新的 tracking 帧时才会返回成功。
	 * @param trackingFrame trakcing 数据
	 * @return 是否成功
	 */
	bool Render(larkxrTrackingFrame* trackingFrame);

	/**
	 * 使用渲染队列渲染，要在 XRConfig::use_render_queue=true 开启，开启后通过 videoFrame 参数获取视频纹理 id
	 * 视频的纹理 id 每次渲染可能不同
	 * 渲染完成之后要手动调用 ReleaseRenderTexture 归还当前纹理，否则渲染队列将耗尽。
	 * @param trackingFrame trakcing 数据
	 * @param videoFrame 视频纹理数据，在渲染队列中取出，要注意每次从队列中取出的纹理id可能不同
	 * @return 是否有新的视频帧
	 */
	bool Render(larkxrTrackingFrame* trackingFrame, lark::XRVideoFrame* videoFrame);

	/**
	 * 在开启渲染队列的情况下，使用完纹理后要手动释放。
	 */
	void ReleaseRenderTexture();

	/**
	 * 检测当前是否是暂停状态
	 * @return 是否暂停
	 */
	bool IsPause() const;
	/**
	 * 检测当前是否是连接成功的状态
	 * @return 是否是连接状态
	 */
	bool is_connected() const;

	/**
	 * 检测当前是否开始流媒体
	 * @return 是否开始流媒体
	 */
    bool streaming_started() const;
    /**
     * 检测视频流是否初始化成功
     * @return 视频是否初始化成功
     */
	bool media_ready() const;
	/**
	 * mutiview 模式下返回硬件解码的纹理 id
	 * XRConfig::use_multiview=true
	 * @return 返回 opengl 纹理id
	 */
	int native_textrure() const;

	/**
	 * stereo 模式下返回硬件解码的左眼纹理 id
	 * XRConfig::use_multiview=false
	 * @return 返回 opengl 纹理id
	 */
	int native_textrure_left() const;
	/**
	 * stereo 模式下返回硬件解码的右眼纹理 id
	 * XRConfig::use_multiview=false
	 * @return 返回 opengl 纹理id
	 */
	int native_textrure_right() const;
	/**
	 * 解码帧的类型 
	 * @see XRVideoFrame::FrameType
	*/
	int native_frame_type() const;
	/**
	* 硬件解码
	*/
	larkxrHwRenderTexture hw_video_frame() const;
	/**
	 * 当前缓存下来的设备姿态数据
	 * @return 设备姿态数据
	 */
	larkxrTrackingDevicePairFrame &device_pair();

	/**
	 * 单独关闭udp媒体通道
	 */
	void CloseUdp();

	/**
	 * 事件码。完整的事件码参照枚举 larkEventTypes
	 * 当有新事件发生时该值会被更新，否则将保留。
	 * @return 事件码。
	 */
	int last_error_code();
	/**
	 * 错误信息。跟 last_error_code 同步更新。
	 * @return 错误信息。
	 */
	std::string last_error_message();
	/**
	 * 清理事件信息。
	 */
	void ClearError();
private:
	XRClientImp* xr_client_imp_;
};
}



#endif //CLOUDLARKXR_XR_CLIENT_H
