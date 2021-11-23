#ifndef TEXTURE_H
#define TEXTURE_H


#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <GLES2/gl2ext.h>
#include <jni.h>


class Texture {
public:

    Texture();
    ~Texture();
    void initJavaMethod(JNIEnv* jni, JavaVM* jvm, jobject activity);
    void build();
    void unBuild();
public:

    unsigned int mTextureId;
    unsigned int mTextureIdCube;

private:
    jmethodID mGetTexidMethodId;
    jmethodID mGetTexidMethodIdCube;
    jobject mGClass;
    JavaVM* mJvm;
    JNIEnv* mJniEnv;
};
#endif