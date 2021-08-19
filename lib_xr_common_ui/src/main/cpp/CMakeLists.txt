# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

set(LIBNAME lib_xr_common_ui)
set(LARK_LIBNAME "lark_xr_common_ui")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(project_base_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../../../)

# set thirdparty_base_dir before include.
set(third_party_base_dir ${project_base_dir}/third_party)
set(STATIC_POCO_LIB OFF)
include(${project_base_dir}/cmake/android_third_party.cmake)

# source dir
# source dir
set(src_dir ${CMAKE_CURRENT_SOURCE_DIR})
set(common_dir ${src_dir}/)

set(common_files
    ${common_dir}/asset_files.h
    ${common_dir}/asset_files.cpp
    ${common_dir}/build_config.h
    ${common_dir}/build_config.cpp
    ${common_dir}/log.h
    ${common_dir}/utils.h
    ${common_dir}/application.h
    ${common_dir}/application.cpp
    ${common_dir}/input.h
    ${common_dir}/input.cpp
    ${common_dir}/env_context.cpp
    ${common_dir}/rect_texture.cpp
    ${common_dir}/test_obj.cpp
    # ui base
    ${common_dir}/ui/raycast.cpp
    ${common_dir}/ui/menu_view.cpp
    ${common_dir}/ui/component/base.cpp
    ${common_dir}/ui/component/image.cpp
    ${common_dir}/ui/component/text.cpp
    ${common_dir}/ui/component/slider.cpp
    ${common_dir}/ui/component/color_box.cpp
    ${common_dir}/ui/component/border.cpp
    ${common_dir}/ui/component/button.cpp
    ${common_dir}/ui/component/keyboard.cpp
    # ui view
    ${common_dir}/ui/view.cpp
    ${common_dir}/ui/navigation.cpp
    ${common_dir}/ui/localization.cpp
    ${common_dir}/ui/controller.cpp
    # ui home
    ${common_dir}/ui/home/cover_item.cpp
    ${common_dir}/ui/home/home.cpp
    # loading
    ${common_dir}/ui/loading/loading.cpp
    ${common_dir}/ui/aa_bb.cpp
    # setup server addr.
    ${common_dir}/ui/setup_server/setup_server_addr.cpp
    # setup
    ${common_dir}/ui/setup/setup.cpp
    ${common_dir}/ui/setup/item_base.cpp
    ${common_dir}/ui/setup/ipd.cpp
    ${common_dir}/ui/setup/room_height.cpp
    ${common_dir}/ui/setup/coderate.cpp
    ${common_dir}/ui/setup/resolution.cpp
    ${common_dir}/ui/setup/kcp.cpp
    ${common_dir}/ui/setup/h265.cpp
    ${common_dir}/ui/setup/fps.cpp
    ${common_dir}/ui/setup/haptics_feedback.cpp
    ${common_dir}/ui/setup/ffr_setup.cpp
    ${common_dir}/ui/setup/fec_report.cpp
    ${common_dir}/ui/setup/use_10bitencode.cpp
    ${common_dir}/ui/setup/quick_config_setup.cpp
)

# source files
set(src_files
    ${common_files}
)

# header files
set(header_files
)

add_definitions(-D_GLM_ENABLE_EXPERIMENTAL)

add_library( # Sets the name of the library.
    ${LIBNAME}
     # Sets the library as a shared library.
     SHARED
     # Provides a relative path to your source file(s).
     ${src_files}
     ${header_files}
)

add_library(${LARK_LIBNAME} ALIAS ${LIBNAME})

set_target_properties(${LIBNAME}
        PROPERTIES
        OUTPUT_NAME "${LARK_LIBNAME}"
)

# add include
target_include_directories(${LIBNAME} PRIVATE
    ${src_dir}
    ${common_dir}
    ${ANDROID_NDK}/sources/android/native_app_glue
    # pxy gl
    ${project_base_dir}/lib_pxygl/src/main/cpp
    # lib xr
    ${project_base_dir}/lark_xr/include/
    ${third_party_base_dir}/freetype/include/
    ${third_party_base_dir}/glm/include/
    ${third_party_base_dir}/assimp/include/
)

include(${project_base_dir}/cmake/utils.cmake)
ANDROID_SHARE_OUTPUT(${LIBNAME} ${project_base_dir})

find_library( # Sets the name of the path variable.
        log-lib
        log )

target_link_libraries( # Specifies the target library.
    ${LIBNAME}
    android
    jnigraphics
    log

    # clang 注意顺序
    EGL
    GLESv3

    # first party
    lib_lark_common
    lib_lark_pxygl
    lib_lark_xr
    # Links the target library to the log library
    # included in the NDK.
    # freetype
    lib_freetype
    # glm
    lib_glm
)