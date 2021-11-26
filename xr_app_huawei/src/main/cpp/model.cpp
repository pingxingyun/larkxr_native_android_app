#define XR_USE_PLATFORM_ANDROID
#define XR_USE_GRAPHICS_API_OPENGL_ES


#include "model.h"
#include "logger.h"
#include "gl_function_ext.h"
#include "openxr/openxr.h"

#define LOG_TAG "Model"


Model::Model() : mVBO(0), mEBO(0), mVAO(0)
{

}

Model::~Model()
{

}

void Model::unBuild()
{
    LOGI("Model::unBuild");
    glDeleteVertexArraysOESEXT(1, &mVAO);
    glDeleteBuffers(1, &mEBO);
    glDeleteBuffers(1, &mVBO);
    mEBO = 0;
    mVBO = 0;
    mVAO = 0;

}

void Model::unBuildCube()
{
    LOGI("Model::unBuildCube");
    glDeleteVertexArraysOESEXT(1, &mVAOCube);
    glDeleteBuffers(1, &mEBOCube);
    glDeleteBuffers(1, &mVBOCube);
    mEBOCube = 0;
    mVBOCube = 0;
    mVAOCube = 0;

}
void Model::draw() const
{
    //LOGI("Model::draw()");
    glBindVertexArrayOESEXT(mVAO);

    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_SHORT, nullptr);

    glBindVertexArrayOESEXT(0);

}

void Model::drawCube() const
{
    //LOGI("Model::drawCube()");
    glBindVertexArrayOESEXT(mVAOCube);

    glDrawElements(GL_TRIANGLES, mIndicesCube.size(), GL_UNSIGNED_SHORT, nullptr);

    glBindVertexArrayOESEXT(0);

}

void Model::build()
{
    LOGI("Model::build");
    //Create Vertex of a Cube
    std::vector<XrVector3f> vertex_position;
    vertex_position.resize(8);

    vertex_position[0].x = -2.0f ;
    vertex_position[0].y = 2.0f ;
    vertex_position[0].z = -2.0f ;

    vertex_position[1].x = -2.0f ;
    vertex_position[1].y = -2.0f ;
    vertex_position[1].z = -2.0f ;

    vertex_position[2].x = 2.0f ;
    vertex_position[2].y = -2.0f ;
    vertex_position[2].z = -2.0f ;

    vertex_position[3].x = 2.0f ;
    vertex_position[3].y = 2.0f ;
    vertex_position[3].z = -2.0f ;

    vertex_position[4].x = -2.0f ;
    vertex_position[4].y = 2.0f ;
    vertex_position[4].z = 2.0f ;

    vertex_position[5].x = -2.0f ;
    vertex_position[5].y = -2.0f ;
    vertex_position[5].z = 2.0f ;

    vertex_position[6].x = 2.0f ;
    vertex_position[6].y = -2.0f ;
    vertex_position[6].z = 2.0f ;

    vertex_position[7].x = 2.0f ;
    vertex_position[7].y = 2.0f ;
    vertex_position[7].z = 2.0f ;

    //Create Index
    mIndices.resize( 36 );
    //1
    int offest = 0;
    mIndices[offest + 0] = 0;
    mIndices[offest + 1] = 1;
    mIndices[offest + 2] = 2;
    mIndices[offest + 3] = 2;
    mIndices[offest + 4] = 3;
    mIndices[offest + 5] = 0;
    //2
    offest +=6;
    mIndices[offest + 0] = 3;
    mIndices[offest + 1] = 2;
    mIndices[offest + 2] = 6;
    mIndices[offest + 3] = 6;
    mIndices[offest + 4] = 7;
    mIndices[offest + 5] = 3;
    //3
    offest +=6;
    mIndices[offest + 0] = 7;
    mIndices[offest + 1] = 6;
    mIndices[offest + 2] = 5;
    mIndices[offest + 3] = 5;
    mIndices[offest + 4] = 4;
    mIndices[offest + 5] = 7;
    //4
    offest +=6;
    mIndices[offest + 0] = 4;
    mIndices[offest + 1] = 5;
    mIndices[offest + 2] = 1;
    mIndices[offest + 3] = 1;
    mIndices[offest + 4] = 0;
    mIndices[offest + 5] = 4;
    //5
    offest +=6;
    mIndices[offest + 0] = 4;
    mIndices[offest + 1] = 0;
    mIndices[offest + 2] = 3;
    mIndices[offest + 3] = 3;
    mIndices[offest + 4] = 7;
    mIndices[offest + 5] = 4;
    //6
    offest +=6;
    mIndices[offest + 0] = 1;
    mIndices[offest + 1] = 5;
    mIndices[offest + 2] = 6;
    mIndices[offest + 3] = 6;
    mIndices[offest + 4] = 2;
    mIndices[offest + 5] = 1;



    //buildModel;
    glGenVertexArraysOESEXT(1, &mVAO);
    glBindVertexArrayOESEXT(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_position[0]), nullptr);

    glBufferData(GL_ARRAY_BUFFER, vertex_position.size() * sizeof(vertex_position[0]), &vertex_position[0], GL_STATIC_DRAW);

    glGenBuffers(1, &mEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(mIndices[0]), &mIndices[0], GL_STATIC_DRAW);

    glBindVertexArrayOESEXT(0);
    glDisableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void Model::buildCube()
{
    //Create Vertex of a Cube
    LOGI("Model::buildCube");
    std::vector<XrVector3f> vertex_position;
    vertex_position.resize(8);

    vertex_position[0].x = -0.08f ;
    vertex_position[0].y = 0.08f ;
    vertex_position[0].z = -0.32f ;

    vertex_position[1].x = -0.08f ;
    vertex_position[1].y = -0.08f ;
    vertex_position[1].z = -0.32f ;

    vertex_position[2].x = 0.08f ;
    vertex_position[2].y = -0.08f ;
    vertex_position[2].z = -0.32f ;

    vertex_position[3].x = 0.08f ;
    vertex_position[3].y = 0.08f ;
    vertex_position[3].z = -0.32f ;

    vertex_position[4].x = -0.08f ;
    vertex_position[4].y = 0.08f ;
    vertex_position[4].z = 0.32f ;

    vertex_position[5].x = -0.08f ;
    vertex_position[5].y = -0.08f ;
    vertex_position[5].z = 0.32f ;

    vertex_position[6].x = 0.08f ;
    vertex_position[6].y = -0.08f ;
    vertex_position[6].z = 0.32f ;

    vertex_position[7].x = 0.08f ;
    vertex_position[7].y = 0.08f ;
    vertex_position[7].z = 0.32f ;

    std::vector<XrVector3f> vertex_color;
    vertex_color.resize(8);

    vertex_color[0].x = 0.5f ;
    vertex_color[0].y = 0.5f ;
    vertex_color[0].z = 0.5f ;

    vertex_color[1].x = 0.5f ;
    vertex_color[1].y = 0.5f ;
    vertex_color[1].z = -0.5f ;

    vertex_color[2].x = -0.5f ;
    vertex_color[2].y = 0.5f ;
    vertex_color[2].z = 0.5f ;

    vertex_color[3].x = -0.5f ;
    vertex_color[3].y = 0.5f ;
    vertex_color[3].z = -0.5f ;

    vertex_color[4].x = 0.5f ;
    vertex_color[4].y = -0.5f ;
    vertex_color[4].z = 0.5f ;

    vertex_color[5].x = 0.5f ;
    vertex_color[5].y = -0.5f ;
    vertex_color[5].z = -0.5f ;

    vertex_color[6].x = -0.5f ;
    vertex_color[6].y = -0.5f ;
    vertex_color[6].z = 0.5f ;

    vertex_color[7].x = -0.5f ;
    vertex_color[7].y = -0.5f ;
    vertex_color[7].z = -0.5f ;

    //Create Index
    mIndicesCube.resize( 36 );
    //1
    int offest = 0;
    mIndicesCube[offest + 0] = 0;
    mIndicesCube[offest + 1] = 1;
    mIndicesCube[offest + 2] = 2;
    mIndicesCube[offest + 3] = 2;
    mIndicesCube[offest + 4] = 3;
    mIndicesCube[offest + 5] = 0;
    //2
    offest +=6;
    mIndicesCube[offest + 0] = 3;
    mIndicesCube[offest + 1] = 2;
    mIndicesCube[offest + 2] = 6;
    mIndicesCube[offest + 3] = 6;
    mIndicesCube[offest + 4] = 7;
    mIndicesCube[offest + 5] = 3;
    //3
    offest +=6;
    mIndicesCube[offest + 0] = 7;
    mIndicesCube[offest + 1] = 6;
    mIndicesCube[offest + 2] = 5;
    mIndicesCube[offest + 3] = 5;
    mIndicesCube[offest + 4] = 4;
    mIndicesCube[offest + 5] = 7;
    //4
    offest +=6;
    mIndicesCube[offest + 0] = 4;
    mIndicesCube[offest + 1] = 5;
    mIndicesCube[offest + 2] = 1;
    mIndicesCube[offest + 3] = 1;
    mIndicesCube[offest + 4] = 0;
    mIndicesCube[offest + 5] = 4;
    //5
    offest +=6;
    mIndicesCube[offest + 0] = 4;
    mIndicesCube[offest + 1] = 0;
    mIndicesCube[offest + 2] = 3;
    mIndicesCube[offest + 3] = 3;
    mIndicesCube[offest + 4] = 7;
    mIndicesCube[offest + 5] = 4;
    //6
    offest +=6;
    mIndicesCube[offest + 0] = 1;
    mIndicesCube[offest + 1] = 5;
    mIndicesCube[offest + 2] = 6;
    mIndicesCube[offest + 3] = 6;
    mIndicesCube[offest + 4] = 2;
    mIndicesCube[offest + 5] = 1;



    //buildModel;
    glGenVertexArraysOESEXT(1, &mVAOCube);
    glBindVertexArrayOESEXT(mVAOCube);

    glGenBuffers(1, &mVBOCube);
    glBindBuffer(GL_ARRAY_BUFFER, mVBOCube);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_position[0]), nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_color[0]), nullptr);
    glBufferData(GL_ARRAY_BUFFER, vertex_position.size() * sizeof(vertex_position[0]), &vertex_position[0], GL_STATIC_DRAW);


    glGenBuffers(1, &mEBOCube);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBOCube);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndicesCube.size() * sizeof(mIndicesCube[0]), &mIndicesCube[0], GL_STATIC_DRAW);

    glBindVertexArrayOESEXT(0);
    glDisableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
