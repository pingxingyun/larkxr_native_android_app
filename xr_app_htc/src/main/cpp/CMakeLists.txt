# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(project_base_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../../../)

# set thirdparty_base_dir before include.
set(third_party_base_dir ${project_base_dir}/third_party)
include(${project_base_dir}/cmake/android_third_party.cmake)

# source dir
set(src_dir ${CMAKE_CURRENT_SOURCE_DIR})

# source files
set(src_files
        ${common_files}
        ${src_dir}/main.cpp
        ${src_dir}/wave_application.cpp
        ${src_dir}/wvr_frame_buffer.cpp
        ${src_dir}/wvr_scene.cpp
        ${src_dir}/wvr_scene_local.cpp
        ${src_dir}/wvr_scene_cloud.cpp
        ${src_dir}/wvr_utils.cpp
        ${src_dir}/matrices.cpp
        ${src_dir}/matrices.h
        ${src_dir}/vectors.h
        )

# header files
set(header_files
        ${src_dir}/wave_application.h
        )

add_definitions(-D_GLM_ENABLE_EXPERIMENTAL)

add_library( # Sets the name of the library.
        lark_xr_htc
        # Sets the library as a shared library.
        SHARED
        # Provides a relative path to your source file(s).
        ${src_files}
        ${header_files}
        )

# add include
target_include_directories(lark_xr_htc PRIVATE
        ${src_dir}
        ${common_dir}
        ${ANDROID_NDK}/sources/android/native_app_glue
        # pxy gl
        ${project_base_dir}/lib_pxygl/src/main/cpp
        # common ui
        ${project_base_dir}/lib_xr_common_ui/src/main/cpp
        # lib xr
        ${project_base_dir}/lark_xr/include
        ${third_party_base_dir}/protobuf/include/src/
        ${third_party_base_dir}/freetype/include/
        ${third_party_base_dir}/glm/include/
        ${third_party_base_dir}/assimp/include/
        ${third_party_base_dir}/wvr_client/include/
        )

find_library( # Sets the name of the path variable.
        log-lib
        log )

target_link_libraries( # Specifies the target library.
        lark_xr_htc
        android
        jnigraphics
        log

        # clang 注意顺序
        EGL
        GLESv3

        # ndk media codec
        mediandk
        OpenMAXAL

        lib_lark_common
        lib_lark_xr
        # first party
        lib_lark_pxygl
        lib_lark_xr_common_ui
        # wave api
        lib_wave_api
        # Links the target library to the log library
        # included in the NDK.
        lib_assimp_android_jniiosystem
)