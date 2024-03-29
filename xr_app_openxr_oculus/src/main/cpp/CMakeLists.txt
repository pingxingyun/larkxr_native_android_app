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

# build native_app_glue as a static lib
add_library(native_app_glue STATIC
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

# Export ANativeActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
set(CMAKE_SHARED_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

if (ENABLE_CLOUDXR STREQUAL "1")
    message(STATUS "=========start cloudxr ${CLOUDXR_LIBPATH}")
    # Import CloudXR
    add_library(cloudxr-lib SHARED IMPORTED)
    set_target_properties(cloudxr-lib PROPERTIES IMPORTED_LOCATION
            ${CLOUDXR_LIBPATH}/${ANDROID_ABI}/libCloudXRClient.so
    )
    include_directories(${CLOUDXR_INCLUDE})

    add_definitions(-DENABLE_CLOUDXR)
endif()

# source dir
set(src_dir ${CMAKE_CURRENT_SOURCE_DIR})

# source files
set(src_files
        main.cpp
        graphics_device_android.cpp
        openxr_context.cpp
        frame_buffer.cpp
        input_state.cpp
        xr_scene.cpp
        xr_scene_local.cpp
        xr_scene_cloud.cpp
        oxr_application.cpp
)

# header files
set(header_files
        oxr_utils.h
)

add_definitions(-D_GLM_ENABLE_EXPERIMENTAL)
add_definitions(-DXR_USE_GRAPHICS_API_OPENGL_ES)
add_definitions(-DXR_USE_PLATFORM_ANDROID)

add_library( # Sets the name of the library.
        lark_xr_openxr_oculus
        # Sets the library as a shared library.
        SHARED
        # Provides a relative path to your source file(s).
        ${src_files}
        ${header_files}
        )

# add include
target_include_directories(lark_xr_openxr_oculus PRIVATE
        ${src_dir}
        ${common_dir}
        ${ANDROID_NDK}/sources/android/native_app_glue
        # pxy gl
        ${project_base_dir}/lib_pxygl/src/main/cpp
        # common ui
        ${project_base_dir}/lib_xr_common_ui/src/main/cpp
        # lib xr
        ${project_base_dir}/lark_xr/include
        ${third_party_base_dir}/freetype/include/
        ${third_party_base_dir}/glm/include/
        ${third_party_base_dir}/assimp/include/
        ${third_party_base_dir}/oboe/include/
        # openxr oculus
        ${third_party_base_dir}/openxr_oculus/include/
)

find_library( # Sets the name of the path variable.
        log-lib
        log )

target_link_libraries( # Specifies the target library.
        lark_xr_openxr_oculus
        android
        jnigraphics
        native_app_glue
        log

        # clang 注意顺序
        EGL
        GLESv3
        # ndk media codec
        mediandk
        OpenMAXAL

        # openxr oculus
        lib_openxr_loader_oculus

        # oboe
        liboboe
        
        # first party
        lib_lark_common
        lib_lark_pxygl
        lib_lark_xr
        lib_lark_xr_common_ui
        # Links the target library to the log library
        # included in the NDK.
        lib_assimp_android_jniiosystem
)

if (ENABLE_CLOUDXR STREQUAL "1")
    target_link_libraries( # Specifies the target library.
            lark_xr_openxr_oculus
            cloudxr-lib
    )
endif()