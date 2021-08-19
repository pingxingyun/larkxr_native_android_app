# summary

本项目为使用云雀 VR Android SDK 接入 VR 云渲染的 Demo 项目。演示多种 VR 一体机头显设备接入的方式，包括 oculus quest 系列，htc focus 系列，pico neo 系列等。

[云雀VRSDK文档](./doc/云雀VR_SDK.md)

[平行云](https://www.pingxingyun.com/)

[云雀VR服务端部署手册](https://www.pingxingyun.com/online/vr3.html?id=447)

项目目录结构如下：

```path
cmake-----------------------------cmake 配置
doc-------------------------------文档
lib_pxygl-------------------------适用于 VR 双目渲染的 opengl 的封装
lib_xr_common_ui------------------app 的通用原生 ui
xr_app_oculus---------------------oculus quest 系列头显适配
xr_app_htc------------------------htc focus 系列头显适配
xr_app_pico-----------------------pico neo 系列头显的适配
third_party-----------------------第三方库
```

## 云雀 VR SDK 兼容测试

- [:heavy_check_mark:] [oculus quest](https://www.oculus.com/quest-2/)
- [:heavy_check_mark:] [htc focus](https://www.vive.com/cn/product/vive-focus/)
- [:heavy_check_mark:] [pico neo](https://www.pico-interactive.com/)
- [:heavy_check_mark:] [Sonic](https://www.nolovr.com/index)

## code style

[Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
