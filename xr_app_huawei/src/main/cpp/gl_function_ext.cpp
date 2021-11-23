#include "gl_function_ext.h"
#include <string>

using std::string;

const char * getEglErrorChars()
{
    const char* ret = "ERR_EGL_UNKNOWN";
    
    switch(eglGetError())
    {
        case EGL_SUCCESS:
            ret = "EGL_SUCCESS";
            break;
        case EGL_NOT_INITIALIZED:
            ret = "ERR_EGL_NOT_INITIALIZED";
            break;
        case EGL_BAD_ACCESS:
            ret = "ERR_EGL_BAD_ACCESS";
            break;
        case EGL_BAD_ALLOC:
            ret = "ERR_EGL_BAD_ALLOC";
            break;
        case EGL_BAD_ATTRIBUTE:
            ret = "ERR_EGL_BAD_ATTRIBUTE";
            break;
        case EGL_BAD_CONTEXT:
            ret = "ERR_EGL_BAD_CONTEXT";
            break;
        case EGL_BAD_CONFIG:
            ret = "ERR_EGL_BAD_CONFIG";
            break;
        case EGL_BAD_CURRENT_SURFACE:
            ret = "ERR_EGL_BAD_CURRENT_SURFACE";
            break;
        case EGL_BAD_DISPLAY:
            ret = "ERR_EGL_BAD_DISPLAY";
            break;
        case EGL_BAD_SURFACE:
            ret = "ERR_EGL_BAD_SURFACE";
            break;
        case EGL_BAD_MATCH:
            ret = "ERR_EGL_BAD_MATCH";
            break;
        case EGL_BAD_PARAMETER:
            ret = "ERR_EGL_BAD_PARAMETER";
            break;
        case EGL_BAD_NATIVE_PIXMAP:
            ret = "ERR_EGL_BAD_NATIVE_PIXMAP";
            break;
        case EGL_BAD_NATIVE_WINDOW:
            ret = "ERR_EGL_BAD_NATIVE_WINDOW";
            break;
        case EGL_CONTEXT_LOST:
            ret = "ERR_EGL_CONTEXT_LOST";
            break;
    }
    return ret;
}

PFNGLGENVERTEXARRAYSOESPROC         glGenVertexArraysOESEXT;
PFNGLBINDVERTEXARRAYOESPROC          glBindVertexArrayOESEXT;
PFNGLDELETEVERTEXARRAYSOESPROC   glDeleteVertexArraysOESEXT;

void initGLExtFunctionPointer()
{
    const char *glExtFuncs = (const char *)glGetString(GL_EXTENSIONS);
    if(glExtFuncs == nullptr)
    {
        return;
    }

    string ext(glExtFuncs);
    if(ext.find("GL_OES_vertex_array_object") != string::npos)
    {
        glGenVertexArraysOESEXT = (PFNGLGENVERTEXARRAYSOESPROC)eglGetProcAddress("glGenVertexArraysOES");
        glBindVertexArrayOESEXT = (PFNGLBINDVERTEXARRAYOESPROC)eglGetProcAddress("glBindVertexArrayOES");
        glDeleteVertexArraysOESEXT = (PFNGLDELETEVERTEXARRAYSOESPROC)eglGetProcAddress("glDeleteVertexArraysOES");
    }
}




