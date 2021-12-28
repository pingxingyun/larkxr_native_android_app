cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(project_base_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../../../)

# set thirdparty_base_dir before include.
set(third_party_base_dir ${project_base_dir}/third_party)
include(${project_base_dir}/cmake/android_third_party.cmake)

# build native_app_glue as a static lib
add_library(libxr_loader SHARED IMPORTED)
set_target_properties(libxr_loader PROPERTIES IMPORTED_LOCATION
        "${PROJECT_SOURCE_DIR}/../../../../third_party/huawei/lib/android/${CMAKE_ANDROID_ARCH_ABI}/libxr_loader.so")

#huawei_arengine_ndk
add_library(huawei_arengine_ndk SHARED IMPORTED)
set_target_properties(huawei_arengine_ndk PROPERTIES IMPORTED_LOCATION
        "${ARENGINE_LIBPATH}/${ANDROID_ABI}/libhuawei_arengine_ndk.so")

set(CMAKE_SHARED_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

# source dir
set(src_dir ${CMAKE_CURRENT_SOURCE_DIR})

# source files
set(src_files
        ${src_dir}/Ar_Demo.cpp
        ${src_dir}/jni_interface.cc
        ${src_dir}/util.cpp
        ${src_dir}/rendering/world_background_renderer.cpp
        ${src_dir}/rendering/world_point_cloud_renderer.cpp
        ${src_dir}/rendering/world_render_manager.cpp
        ${src_dir}/rendering/world_object_renderer.cpp
        ${src_dir}/rendering/world_plane_renderer.cpp
        ${src_dir}/ar_rect_texture.cpp

        # render and base use.
        ${common_files}
        )

# header files
set(header_files
        ${src_dir}/Ar_Demo.h
        ${src_dir}/jni_interface.h
        ${src_dir}/util.h
        ${src_dir}/huawei_arengine_interface.h
        ${src_dir}/rendering/world_background_renderer.h
        ${src_dir}/rendering/world_point_cloud_renderer.h
        ${src_dir}/rendering/world_render_manager.h
        ${src_dir}/rendering/world_object_renderer.h
        ${src_dir}/rendering/world_plane_renderer.h
        ${src_dir}/ar_rect_texture.h

        )

add_definitions(-D_GLM_ENABLE_EXPERIMENTAL)

add_library( # Sets the name of the library.
        ar_huawei_app
        # Sets the library as a shared library.
        SHARED
        # Provides a relative path to your source file(s).
        ${src_files}
        ${header_files}
        )

# add include
target_include_directories(ar_huawei_app PRIVATE
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
        )

find_library( # Sets the name of the path variable.
        log-lib
        log )

target_link_libraries( # Specifies the target library.
        ar_huawei_app
#        cloudxr-lib
        android
        jnigraphics
        # native_app_glue
        libxr_loader
        log
        # clang 注意顺序
        EGL
        GLESv3
        # ndk media codec
        huawei_arengine_ndk

        mediandk
        OpenMAXAL

        # first party
        lib_lark_common
        lib_lark_pxygl
        lib_lark_xr
        lib_lark_xr_common_ui
        # Links the target library to the log library
        # included in the NDK.
        lib_assimp_android_jniiosystem
        )