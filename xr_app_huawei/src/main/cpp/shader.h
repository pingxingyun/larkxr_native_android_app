#ifndef SHADER_H
#define SHADER_H


#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <GLES2/gl2ext.h>
#include "logger.h"
#define LOG_TAG "Shader"

const char * g_vertex_shader =
        "attribute vec3 aPos;\n"
        "uniform mat4 projection;\n"
        "uniform mat4 view;\n"
        "varying highp vec3 TexCoords;\n"
        "\n"
        "void main()\n"
        "{\n"
        "	TexCoords = aPos;\n"
        "	vec4 position = projection * view * vec4(aPos, 1.0);\n"
        "	gl_Position = position.xyzw;\n"
        "}\n";

const char * g_fragment_shader =
        "uniform samplerCube Texture0;\n"
        "varying highp vec3 TexCoords;\n"
        "void main()\n"
        "{\n"
        "	gl_FragColor = textureCube(Texture0, TexCoords);\n"
        "}\n";

//textureCube  ==> texture

const char * g_c_vertex_shader =
        "attribute vec3 aPos;\n"
        "attribute vec3 aColor;\n"
        "uniform mat4 projection;\n"
        "uniform vec3 u_cameraPos;\n"
        "uniform mat4 view;\n"
        "uniform mat4 viewOffset;\n"
        "uniform mat4 viewCtlMatrix;\n"
        "varying highp vec3 TexCoords;\n"
        "varying highp vec3 TexColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   TexColor = aColor;\n"
        "	TexCoords = vec3(viewOffset * viewCtlMatrix * vec4(aPos, 1.0));\n"
        "	vec4 position = projection * view * viewOffset * viewCtlMatrix * vec4(aPos, 1.0);\n"
        "	gl_Position = position.xyzw;\n"
        "}\n";


const char * g_c_fragment_shader =
        "uniform sampler2D Texture0;\n"
        "varying highp vec3 TexCoords;\n"
        "varying highp vec3 TexColor;\n"
        "void main()\n"
        "{\n"
        " gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);\n"
        //" gl_FragColor = vec4(TexColor, 1.0);\n"
        //"	gl_FragColor = texture(Texture0, TexCoords);\n"
        "}\n";


class Shader
{
public:

    Shader()
    {

    }

    ~Shader()
    {

    }

    void build()
    {
        LOGI("shader build");
        // Compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &g_vertex_shader, NULL);
        glCompileShader(vertex);

        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &g_fragment_shader, NULL);
        glCompileShader(fragment);

        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);

        glLinkProgram(ID);

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    void buildCube()
    {
        LOGI("shader buildCube()");
        // Compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &g_c_vertex_shader, NULL);
        glCompileShader(vertex);

        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &g_c_fragment_shader, NULL);
        glCompileShader(fragment);

        // shader Program
        IDCube = glCreateProgram();
        glAttachShader(IDCube, vertex);
        glAttachShader(IDCube, fragment);

        glLinkProgram(IDCube);

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    void unBuild()
    {

    }

    void unBuildCube()
    {

    }

    void use() 
    {
        glUseProgram(ID); 
    }

    void useCube()
    {
        LOGI("shader useCube");
        glUseProgram(IDCube);
    }

    void unUse()
    {
        glUseProgram(0);
    }

    void unUseCube()
    {
        LOGI("shader unUseCube");
        glUseProgram(0);
    }

    void setInt(const char* name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name), value); 
    }

    void setIntCube(const char* name, int value) const
    {
        LOGI("shader setIntCube");
        glUniform1i(glGetUniformLocation(IDCube, name), value);
    }

    void setIntCubeName(const char* name) const
    {
        LOGI("shader setIntCube");
        glUniform3f(glGetUniformLocation(IDCube, name), 0.0, 0.0, 2.2);
    }

    void setMat4(const char* name, const float* mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, mat);
    }

    void setMat4Cube(const char* name, const float* mat) const
    {
        LOGI("shader setMat4Cube");
        glUniformMatrix4fv(glGetUniformLocation(IDCube, name), 1, GL_FALSE, mat);
    }

public:
    unsigned int ID;
    unsigned int IDCube;
};
#endif