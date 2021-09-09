# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)

# Declares and names the project.

project("xvisiolarkxr")

# set thirdparty_base_dir before include.
set(project_base_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../../../)
set(third_party_base_dir ${project_base_dir}/third_party)
include(${project_base_dir}/cmake/android_third_party.cmake)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             native-lib.cpp
             larkxr_client.cpp
             World.cpp
             render/texture_rect.cpp
             render/MatrixState.cpp
             scene_local.cpp
             scene_base.cpp
             xvisio_vr_utils.cpp
             INxrSensorClient/NxrMoudle.cpp
)

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
        ${third_party_base_dir}/poco/include/
        ${third_party_base_dir}/nolo_nibiru/include/
        )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       native-lib

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                        GLESv3
                        m
                        EGL
                        android
                        OpenSLES
                        libnvr_local
                        libnvr_ndk_helper

                        lib_lark_common
                        lib_lark_xr
                        lib_lark_pxygl
                        lib_lark_xr_common_ui
)