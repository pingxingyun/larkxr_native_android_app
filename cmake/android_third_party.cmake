# set thirdparty_base_dir before include
# thirdparty_base_dir
# set(thirdparty_base_dir ${CMAKE_CURRENT_SOURCE_DIR}/../third_party)

# set project_base_dir before include
include(${project_base_dir}/cmake/utils.cmake)

set(lib_platform_lib /lib)
set(lib_platform_lib_prex /${lib_platform_lib}/android/)
set(lib_android_prex /${lib_platform_lib_prex}${CMAKE_ANDROID_ARCH_ABI}/)

# oculus queset sdk
ANDROID_THIRD_PARTY_SHARED(${project_base_dir} lib_ovr_api ovr libvrapi)
# protobuf
ANDROID_THIRD_PARTY_SHARED(${project_base_dir} lib_protobuf protobuf libprotobuf)

# glm
ANDROID_THIRD_PARTY_SHARED(${project_base_dir} lib_glm glm libglm)

# hp socekt
ANDROID_THIRD_PARTY_SHARED(${project_base_dir} lib_hpsocket hp-socket libhpsocket)

# poco
ANDROID_THIRD_PARTY_SHARED(${project_base_dir} libPocoFoundation poco libPocoFoundation)
ANDROID_THIRD_PARTY_SHARED(${project_base_dir} libPocoFoundationd poco libPocoFoundationd)

ANDROID_THIRD_PARTY_SHARED(${project_base_dir} libPocoEncodings poco libPocoEncodings)
ANDROID_THIRD_PARTY_SHARED(${project_base_dir} libPocoEncodingsd poco libPocoEncodingsd)

ANDROID_THIRD_PARTY_SHARED(${project_base_dir} libPocoJSON poco libPocoJSON)
ANDROID_THIRD_PARTY_SHARED(${project_base_dir} libPocoJSONd poco libPocoJSONd)

ANDROID_THIRD_PARTY_SHARED(${project_base_dir} libPocoNet poco libPocoNet)
ANDROID_THIRD_PARTY_SHARED(${project_base_dir} libPocoNetd poco libPocoNetd)

ANDROID_THIRD_PARTY_SHARED(${project_base_dir} libPocoUtil poco libPocoUtil)
ANDROID_THIRD_PARTY_SHARED(${project_base_dir} libPocoUtild poco libPocoUtild)

ANDROID_THIRD_PARTY_SHARED(${project_base_dir} libPocoXML poco libPocoXML)
ANDROID_THIRD_PARTY_SHARED(${project_base_dir} libPocoXMLd poco libPocoXMLd)

ANDROID_THIRD_PARTY_SHARED(${project_base_dir} libPocoNetSSL poco libPocoNetSSL)
ANDROID_THIRD_PARTY_SHARED(${project_base_dir} libPocoNetSSLd poco libPocoNetSSLd)

ANDROID_THIRD_PARTY_SHARED(${project_base_dir} libPocoCrypto poco libPocoCrypto)
ANDROID_THIRD_PARTY_SHARED(${project_base_dir} libPocoCryptod poco libPocoCryptod)

# ffmpeg
ANDROID_THIRD_PARTY_SHARED(${project_base_dir} lib_ffmpeg_avcodec ffmpeg libavcodec)
ANDROID_THIRD_PARTY_SHARED(${project_base_dir} lib_ffmpeg_avfliter ffmpeg libavfilter)
ANDROID_THIRD_PARTY_SHARED(${project_base_dir} lib_ffmpeg_avformat ffmpeg libavformat)
ANDROID_THIRD_PARTY_SHARED(${project_base_dir} lib_ffmpeg_avutil ffmpeg libavutil)
ANDROID_THIRD_PARTY_SHARED(${project_base_dir} lib_ffmpeg_swresample ffmpeg libswresample)
ANDROID_THIRD_PARTY_SHARED(${project_base_dir} lib_ffmpeg_swscale ffmpeg libswscale)

# assimp-5.0.0 rc1
ANDROID_THIRD_PARTY_SHARED(${project_base_dir} lib_assimp assimp libassimp)

# add assimp android port
ANDROID_THIRD_PARTY_STATIC(${project_base_dir} lib_assimp_android_jniiosystem assimp libandroid_jniiosystem)

# htc wave api
ANDROID_THIRD_PARTY_SHARED(${project_base_dir} lib_freetype freetype libfreetype)

# pico
ANDROID_THIRD_PARTY_SHARED(${project_base_dir} lib_pico pico libPvr_NativeSDK)

# freetype
ANDROID_THIRD_PARTY_SHARED(${project_base_dir} lib_wave_api wvr_client libwvr_api)

# openxr
ANDROID_THIRD_PARTY_SHARED(${project_base_dir} lib_openxr_loader openxr libopenxr_loader)

# svr-xms
ANDROID_THIRD_PARTY_SHARED(${project_base_dir} lib_svrapi svr_xms libsvrapi)

# svr-xms
ANDROID_THIRD_PARTY_SHARED(${project_base_dir} lib_xdevice xms libxdevice)

# nolo
ANDROID_THIRD_PARTY_SHARED(${project_base_dir} libgsxr_dev_lib nolo_dev_api libgsxr_dev_lib)
ANDROID_THIRD_PARTY_SHARED(${project_base_dir} libnvr_local nolo_nibiru libnvr_local)
ANDROID_THIRD_PARTY_SHARED(${project_base_dir} libnvr_ndk_helper nolo_nibiru libnvr_ndk_helper)

# tinyobj
set(thirdparty_tinyobj_base ${thirdparty_base_dir}/tinyobj)
set(thirdparty_include_tinyobj ${thirdparty_tinyobj_base}/src)
set(thirdparty_src_tinyobj ${thirdparty_tinyobj_base}/src/tiny_obj_loader.cc)

# fist party pxygl
ANDROID_FIRST_PARTH_SHARED(${project_base_dir} lib_lark_pxygl lib_pxygl liblark_pxygl)

# first party common ui
ANDROID_FIRST_PARTH_SHARED(${project_base_dir} lib_lark_xr_common_ui lib_xr_common_ui liblark_xr_common_ui)

# larkxr lib
add_library(lib_lark_xr SHARED IMPORTED)
set_target_properties(lib_lark_xr PROPERTIES IMPORTED_LOCATION
        ${project_base_dir}/lark_xr/lib/android/${CMAKE_ANDROID_ARCH_ABI}/liblark_xr.so)
		
add_library(lib_lark_common SHARED IMPORTED)
set_target_properties(lib_lark_common PROPERTIES IMPORTED_LOCATION
        ${project_base_dir}/lark_xr/lib/android/${CMAKE_ANDROID_ARCH_ABI}/liblark_common.so)