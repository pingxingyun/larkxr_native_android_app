// 
// Created by fcx@pingxingyun.com
// 2020-04-07 17:05
//
#pragma once
#ifndef XR_IMAGE_BUFFER_INCLUDE
#define XR_IMAGE_BUFFER_INCLUDE

#include <cstdint>
#include "lark_xr/types.h"
#include "lark_xr/lark_xr.h"

namespace lark {
const int VIDEO_FRAME_NUM_DATA_POINTERS = 3;
const int VIDEO_FRAME_Y_PLANE_INDEX = 0;
const int VIDEO_FRAME_U_PLANE_INDEX = 1;
const int VIDEO_FRAME_V_PLANE_INDEX = 2;
class XRVideoFrameImp;
// RGB24 pix fomat
/**
 * 视频帧可包括
 */
class LARK_XR_API XRVideoFrame {
public:
    enum class FrameType
    {
        kNone = larkxrHwRenderTextureType_None,
        kYUV420P,           ///< planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
        kRGB24,             ///< packed RGB 8:8:8, 24bpp, RGBRGB...
        kNV12,              ///< planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V)
        kNV21,              ///<  as above, but U and V bytes are swapped
        // hw
        kNative_Multiview  = larkxrHwRenderTextureType_Android_Multiview,      /// android opengl multiview 左右眼在一起
        kNative_Stereo     = larkxrHwRenderTextureType_Android_Stereo,        /// android opengl 双面分开，左眼一个纹理右眼一个纹理
		kNative_D3D11_Multiview = larkxrHwRenderTextureType_D3D11_Multiview,                 /// windows  d3d11 native texture 左右眼在一起
		kNative_D3D11_Stereo = larkxrHwRenderTextureType_D3D11_Stereo,                 /// windows  d3d11 native texture 左右眼在一起
		kNative_D3D11_NV12 = larkxrHwRenderTextureType_D3D11_NV12,            /// windows  d3d11 nv12 native texture 左右眼在一起
		kNative_D3D11_Y_UV = larkxrHwRenderTextureType_D3D11_Y_UV_SRV,        /// windows  d3d11 nv12 native texture 左右眼在一起
    };
    // copy
    XRVideoFrame(const XRVideoFrame& videoFrame);
    // move
    XRVideoFrame(XRVideoFrame&& videoFrame) noexcept;
    // create empty video frame
    XRVideoFrame(uint64_t frame_index);
    // create video frame with rgb24 buffer
    XRVideoFrame(const uint8_t* data, uint64_t frame_index, int width, int height);
    // create video frame with rgb24 buffer and copy
    XRVideoFrame(const uint8_t* data, uint64_t frame_index, int width, int height, bool copy);
    // create video frame with yuv buffer
    XRVideoFrame(uint8_t* data[], int stride[], uint64_t frame_index, int width, int height);
    // create video frame with yuv buffer and copy
    XRVideoFrame(uint8_t* data[], int stride[], uint64_t frame_index, int width, int height, bool copy);
    // create video frame with yuv buffer and copy
    XRVideoFrame(uint8_t* data[], int stride[], uint64_t frame_index, int width, int height, bool copy, FrameType frameType);
    // android opengl native multiview texture.
    XRVideoFrame(int texture, uint64_t frameIndex);
    // android opengl native stereo view texture;
    XRVideoFrame(int textureLeft, int textureRight, uint64_t frameIndex);
    // d3d11 nv12 native texture
    XRVideoFrame(void* d3d11_texture, int width, int height, uint64_t frameIndex);
    // larkxrHwRenderTextureType_D3D11_Y_UV_SRV
    XRVideoFrame(void* d3d11_texture_left, void* d3d11_texture_right, int width, int height, uint64_t frameIndex);
    ~XRVideoFrame();

    XRVideoFrame& operator=(const XRVideoFrame& other);

    // warning render under opengl context.
    void Render() const;

    void Resize(int width, int height);

    int GetBufferByteSize() const;
    int width() const;
    int height() const;
    int ChromaWidth() const;
    int ChromaHeight() const;
    const uint8_t* DataRGB() const;
    uint64_t frame_index() const;
    void set_frame_index(uint64_t frame_index);
    bool copy() const;
    FrameType frame_type() const;
    const uint8_t* MutableDataY() const;
    const uint8_t* MutableDataU() const;
    const uint8_t* MutableDataV() const;
    int StrideY() const;
    int StrideU() const;
    int StrideV() const;

    // native texture
    int texture() const;
    int texture_left() const;
    int texture_right() const;
    int texture_type() const;
    bool IsNative() const;
    const void* d3d11_texture() const;
    const void* d3d11_texture_left() const;
    const void* d3d11_texture_right() const;

    larkxrHwRenderTexture GetHwVideoFrame() const;
private:
    XRVideoFrameImp* xr_video_frame_imp_;
};
}
#endif // XR_IMAGE_BUFFER_INCLUDE