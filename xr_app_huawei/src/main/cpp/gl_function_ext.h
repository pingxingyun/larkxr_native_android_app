#ifndef VR_GlFunction_H
#define VR_GlFunction_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <GLES2/gl2ext.h>

#define  GLES_VERSION 3  //version number, used for eglsetup func

//Vertex array object functions
extern PFNGLGENVERTEXARRAYSOESPROC         glGenVertexArraysOESEXT;
extern PFNGLBINDVERTEXARRAYOESPROC          glBindVertexArrayOESEXT;
extern PFNGLDELETEVERTEXARRAYSOESPROC   glDeleteVertexArraysOESEXT;

extern const char * getEglErrorChars();

extern void initGLExtFunctionPointer();


#endif

