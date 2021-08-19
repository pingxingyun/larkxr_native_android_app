/*
 * NVRUtils.h
 *
 *  Created on: 2016��3��18��
 *      Author: Steven
 */

#ifndef NVRUTILS_H_
#define NVRUTILS_H_

#include "NVRLog.h"
#include <EGL/egl.h>
#include "time.h"
#include <unistd.h>			// for usleep
#include <GLES3/gl3.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

static const char * getGLErrorMsg(GLenum code) {
	switch (code) {
	case 0:
		return "NO_ERROR";
	case 0x0500:
		return "INVALID_ENUM";
	case 0x0501:
		return "INVALID_VALUE";
	case 0x0502:
		return "INVALID_OPERATION";
	case 0x0503:
		return "STACK_OVERFLOW";
	case 0x0504:
		return "STACK_UNDERFLOW";
	case 0x0505:
		return "OUT_OF_MEMORY";
	case 0x0506:
		return "INVALID_FRAMEBUFFER_OPERATION";
	default:
		return "UNKNOWN";
	}
}

static void glCheck(const char *tag) {

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		NLOGE("TAG: %s : An OpenGL error occurred %s", tag,
				getGLErrorMsg(error));
		return;
	}
}

class ShaderUnit {
public:
	GLuint mVertex;
	GLuint mFragment;
	GLuint mProgram;

};

class NVRUtils {
public:

	void setAssetsManger(JNIEnv* env, jobject assets) {
		assetManager = AAssetManager_fromJava(env, assets);
	}
	void loadShaderUnit(ShaderUnit *unit, const char* vertexFileName,
			const char* fragmentFileName);
	char* readShaderFileFromAssert(const char *filename);
	GLuint compileShader(const char *source, GLenum type);
	GLuint linkProgram(GLuint v_shaders, GLuint frag_shaders);
	void printArray(float* value, int len, const char* tag);

private:
	AAssetManager* assetManager;

};

#endif /* NVRUTILS_H_ */
