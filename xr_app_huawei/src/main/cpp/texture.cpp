

#include "texture.h"
#include "logger.h"

#define LOG_TAG "texture"


Texture::Texture() :
    mGetTexidMethodId(nullptr),
    mGetTexidMethodIdCube(nullptr),
    mGClass(nullptr),
    mJvm(nullptr),
    mJniEnv(nullptr)
{

}

Texture::~Texture()
{

}


void Texture::initJavaMethod(JNIEnv* jni, JavaVM* jvm, jobject activity)
{
    mJvm = jvm;
    LOGI("findClassAndMethod");
    static const char *surfaceTexClassName = "com/huawei/hvrsdk/nativedemo/BitmapDecoder";
    const jclass cls = jni->FindClass(surfaceTexClassName);
    if (cls == 0)
    {
        LOGE("can not find surfaceClass(%s)", surfaceTexClassName);
        return;
    }

    const jmethodID constructMethodId = jni->GetMethodID(cls, "<init>", "(Landroid/content/Context;)V");
    if (constructMethodId == 0)
    {
        LOGE("couldn't get constructMethodId ");
        jni->DeleteLocalRef(cls);
        return;
    }

    jobject obj = jni->NewObject( cls, constructMethodId, activity);
    if ( obj == 0 )
    {
        LOGE( "NewObject() failed" );

        jni->DeleteLocalRef( cls );
        return;
    }
    mGClass = jni->NewGlobalRef( obj );
    if ( mGClass == 0 )
    {
        LOGE( "NewGlobalRef() failed" );
        jni->DeleteLocalRef( obj );
        jni->DeleteLocalRef( cls );
        return;
    }

    mGetTexidMethodId = jni->GetMethodID( cls, "getCubeTexid", "()I" );
    mGetTexidMethodIdCube = jni->GetMethodID( cls, "getCubeTexidCube", "()I" );
    jni->DeleteLocalRef( cls );
    LOGI( "initJavaMethod ok" );
}


void Texture::build()
{

    //Step1 Attach CurrentThread
    if (nullptr == mJvm)
    {
        LOGI("Error : Env not initialized!");
    }
    if (JNI_OK != mJvm->AttachCurrentThread(&mJniEnv, nullptr))
    {
        LOGI("AttachCurrentThread failed!");
    }
    if(mGClass!= NULL && mGetTexidMethodId != NULL)
    {
        mTextureId = mJniEnv->CallIntMethod(mGClass, mGetTexidMethodId);
        LOGI("getTextureid %d", mTextureId);
    }

    if(mGClass!= NULL && mGetTexidMethodIdCube != NULL)
    {
        mTextureIdCube = mJniEnv->CallIntMethod(mGClass, mGetTexidMethodIdCube);
        LOGI("getTextureidCube %d", mTextureIdCube);
    }

}

void Texture::unBuild()
{

    // Detach CurrentThread
    if (nullptr == mJvm)
    {
        LOGE("Error : Env not initialized!");
        return;
    }
    if (JNI_OK != mJvm->DetachCurrentThread())
    {
        LOGE("DetachCurrentThread failed!");
    }
    mJniEnv = nullptr;
    mTextureId = -1;
}


