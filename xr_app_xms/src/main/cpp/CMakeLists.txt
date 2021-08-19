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

add_definitions(-DGLM_ENABLE_EXPERIMENTAL)

# build native_app_glue as a static lib
set(APP_GLUE_DIR ${ANDROID_NDK}/sources/android/native_app_glue)
include_directories(${APP_GLUE_DIR})
add_library( app-glue STATIC ${APP_GLUE_DIR}/android_native_app_glue.c)
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

# source dir
set(src_dir ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB LOCAL_HEADERS  "*.h")
file(GLOB LOCAL_SOURCE   "*.cpp")
file(GLOB LOCAL_HEADERS_FRAMEWORK  "framework/*.h")
file(GLOB LOCAL_SOURCE_FRAMEWORK   "framework/*.cpp")

include_directories(${third_party_base_dir}/openxr/include)

add_library( # Sets the name of the library.
        lark_xr_xms
		SHARED
		${LOCAL_SOURCE}
        ${LOCAL_HEADERS}
		${LOCAL_HEADERS_FRAMEWORK}
		${LOCAL_SOURCE_FRAMEWORK}
)

target_include_directories(lark_xr_xms
		PRIVATE
		${src_dir}/framework/
		# pxy gl
		${project_base_dir}/lib_pxygl/src/main/cpp
		# common ui
		${project_base_dir}/lib_xr_common_ui/src/main/cpp
		# lib xr
		${project_base_dir}/lark_xr/include
		${third_party_base_dir}/ovr/include
		${third_party_base_dir}/protobuf/include/src/
		${third_party_base_dir}/freetype/include/
		${third_party_base_dir}/glm/include/
		${third_party_base_dir}/assimp/include/
		${third_party_base_dir}/wvr_client/include/
		${third_party_base_dir}/ffmpeg/include/
		${third_party_base_dir}/svr_xms/include/
)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       lark_xr_xms
                       android
		               jnigraphics
                       vulkan
					   EGL
					   GLESv3
                       app-glue
                       log
						# first party
						lib_lark_common
						lib_lark_pxygl
						lib_lark_xr
						lib_lark_xr_common_ui
						# Links the target library to the log library
						# included in the NDK.
						# assimp android port
						lib_assimp_android_jniiosystem
						# ndk glue
						#        app_glue
						# svr
						lib_svrapi
						# xms device
		                lib_xdevice
                       )