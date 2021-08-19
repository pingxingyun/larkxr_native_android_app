cmake_minimum_required(VERSION 3.10.2)

project("nololarkxr")

# set thirdparty_base_dir before include.
set(project_base_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../../../)
set(third_party_base_dir ${project_base_dir}/third_party)
include(${project_base_dir}/cmake/android_third_party.cmake)

add_library( # Sets the name of the library.
        native-lib
        SHARED
        native-lib.cpp
        larkxr_client.cpp
        World.cpp
        gsxr/gsxr_dev.cpp
        render/texture_rect.cpp
        render/MatrixState.cpp
        scene_local.cpp
        scene_base.cpp)

# add include
target_include_directories(native-lib PRIVATE
        ${src_dir}
        ${common_dir}
        ${ANDROID_NDK}/sources/android/native_app_glue
        # lib xr
        ${project_base_dir}/lark_xr/include
        # pxy gl
        ${project_base_dir}/lib_pxygl/src/main/cpp
        # common ui
        ${project_base_dir}/lib_xr_common_ui/src/main/cpp
        ${third_party_base_dir}/protobuf/include/src/
        ${third_party_base_dir}/freetype/include/
        ${third_party_base_dir}/glm/include/
        ${third_party_base_dir}/assimp/include/
        ${third_party_base_dir}/nolo_dev_api/include/
        ${third_party_base_dir}/nolo_nibiru/include/
)

target_link_libraries( # Specifies the target library.
        native-lib
        log
        GLESv3
        m
        EGL
        android
        OpenSLES
        libnvr_local
        libnvr_ndk_helper
        libgsxr_dev_lib

        lib_lark_common
        lib_lark_xr
        lib_lark_pxygl
        lib_lark_xr_common_ui)