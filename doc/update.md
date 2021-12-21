# 云雀安卓客户端SDK更新说明

## V3.1.0.0

初始发布.

## V3.1.3.0

1. 更新 Demo，提高稳定性，修复问题。
2. 添加设置电量（头盔和手柄）接口。
3. 设置客户端凭证接口。
4. 应用列表添加翻页相关接口。

## V3.1.8.0

3.1.8.0 新增较多内容，建议参照 SDK 中附带的 Demo 进行更新。

### Android 平台渲染初始化配置

XRClient 类新增渲染初始化参数，可以单独重复调用opengl上下文的初始化和销毁。

> 主要应对 Pico Neo3 环境中多次创建 opengl 环境的情况。在 Pico Neo 中的适配方法参考 demo

```c++
// XRClient
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
```

### 新增控制是否将视频渲染到同一张纹理上

通过 `XRConfig::use_multiview` 设置是否将视频渲染到同一张纹理上进行回调

XRClientObserver 新增如下方法:

```c++
// XRClientObserver
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
```

### 控制渲染画面的上下翻转行为

XRConfig 新增如下静态属性，如果 flip_draw 设置为 true 将视频画面上下翻转进行渲染回调。

```c++
// XRConfig
// 是否将视频画面翻转渲染
// 上下翻转
static bool flip_draw;
```

### 队列渲染

新增渲染队列方法，当开启渲染队列时，收到新的视频帧并渲染完成之后要手动调用释放该帧，否则渲染队列讲耗尽。

如果关闭渲染队列则渲染流程与之前的版本相同。

XRConfig 新增开关

```c++
// XRConfig
// 是否使用渲染队列 (3.1.8.0新增)
static bool use_render_queue;
```

XRClient 中新增渲染队列相关方法

```c++
// XRClient
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
```

### 设置请求超时时间

XRClient 新增静态方法，设置全局的sdk中接口请求的超时时间。

```c++
/*
* http 请求超时时间
*/
static void SetHttpRequestTimeoutInSeconds(int timeoutInSeconds);
```

### 设置头盔类型

现在可以在 SDK 外部手动指定头盔类型，通过 XRConfig::headset_desc 进行设置。

```c++
// XRConfig
// 头盔类型设置，将根据具体头盔的类型选择steamvr中用的手柄和绑定文件 (3.1.8.0新增)
static larkHeadSetControllerDesc headset_desc;
```

新增 nolo 和 pico neo3 类型的头盔。要注意不同的头盔类型绑定的 steamvr 的配置文件不同，导致手柄模型和行为的不同。

```c++
typedef enum larkHeadSetType_ {
    larkHeadSetType_HTC = 0,
    larkHeadSetType_OCULUS = 1,
    larkHeadSetType_PICO_2 = 2,
    larkHeadSetType_PICO_3 = 3,
    larkHeadSetType_NOLO_Sonic_1 = 4,
} larkHeadSetType;
```

### 同步玩家区域

XRClientObserver 新增同步玩家区域设置回调，设置玩家区域将同步设置到 steamvr 的配置当中。

```c++
// XRClientObserver
/**
* 请求同步玩家区域数据
*/
virtual void OnSyncPlayerSpace(larkxrPlaySpace* playSpace) = 0;
```

### 视频流相关配置更新

XRConfig 新增视频流相关控制参数

1. 配置分辨率的缩放 `XRConfig::resolution_scale`

```c++
// XRConfig
// 分辨率缩放 0 - 2
// 最终分辨率为 align32(render_width * resolution_scale); align32(render_height * resolution_scale)
static float resolution_scale;
```

2. 去掉房间高度设置,主要依靠头戴设备本身的 tracking origin 等功能动态的实现云渲染的身高设置。传给 XRClient 的姿态就已经是处理好身高的tracking，不再云雀 SDK 内部处理。

```c++
// XRConfig
// @Deprecated 已弃用.使用头盔姿态的 tracking origin 等功能和安全区域配合使用
// steam vr 设置的初始房间高度
static float room_height;
```

3. 新增 TCP 传输通道，原有的 use_kcp 配置项启用，使用 stream_type 替代。

```c++
// XRConfig
// @Deprecated 已弃用，使用 stream_type 替代
// 是否使用增强协议连接媒体
static bool use_kcp;
// 设置使用的传输通道类型，目前有 udp，tcp，增强udp可选 (3.1.8.0新增)
static larkStreamType stream_type;
```

4. 集中渲染

```c++
/*以高分辨率渲染图像的中心,而以较低的分辨率渲染郊区。
导致需要通过网络传输的视频分辨率大大降低。在相同比特率的情况下,较小的视频可以保留更多细节,并同时降低延迟。
FFR会在视图的边缘造成一些视觉伪像,这些伪像或多或少会根据设置和游戏而变得可见。
*/
typedef struct larkFoveatedRendering_
{
    bool                    enableFoveateRendering = true;      //true的时候 其他参数启用(默认为true)
    float                   foveationStrength = 2;      //渲染强度          [0.5-10.0]          默认 2 值越高,意味着朝向帧边缘的细节越少,伪像越多
    float                   foveationShape = 1.5;      //渲染形状          [0.2-2.0]           默认 1.5  集中渲染的形状
    float                   foveationVerticalOffset = 0;      //渲染垂直偏移      [-0.05-0.05]        默认 0  较高的值表示高质量的帧区域进一步向下移动
} larkFoveatedRendering;
```

```c++
// XRConfig
// 集中渲染设置 (3.1.8.0新增)
static larkFoveatedRendering foveated_rendering;
```

5. 色彩校正

```c++
typedef struct larkColorCorrention_
{
    bool                    enableColorCorrection = true;      //true的时候 其他参数启用(默认为false)
    float                   brightness = 0;      //亮度:范围[-1; 1],默认值为0。-1完全为黑色,1完全为白色
    float                   contrast = 0;      //对比度:范围[-1; 1],默认值为0。-1完全是灰色的
    float                   saturation = 0;      //饱和度:范围[-1; 1],默认值为0。-1为黑白
    float                   gamma = 1;      //伽玛:范围[0; 5],默认值为1。使用值2.2校正从sRGB到RGB空间的颜色
    float                   sharpening = 0;      //锐化:范围[-1; 5],默认为0。-1是最模糊的,5是最锐利的
} larkColorCorrention;
```

```c++
// XRConfig
// 颜色校正设置 (3.1.8.0新增)
static larkColorCorrention color_corrention;
```

6. 其他相关

```c++
// 是否启用 10 bit 编码 (3.1.8.0新增)
static bool use_10bit_encoder;
// 是否上报 fec 失败，当当前 fec 实效时通知服务端加大冗余包 (3.1.8.0新增)
static bool report_fec_failed;
```

## V3.2.0.2

1. 更新服务器接口到 3.2。 注意 3.2 版本服务器不向下兼容 3.1 版本。
2. 添加数据通道支持。

XRClient 类添加如下成员方法：

```c++
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
```

XRClientObserver 添加数据通道回调：

```c++
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
```


3. 可以发送音频数据到云端应用。

XRClient 类添加如下成员方法：

```c++
/**
    * 发送音频数据给云端
    * @param buffer
    * @param length
    */
void SendAudioData(const char* buffer, int length);
```