
#ifndef MODEL_H
#define MODEL_H

#include <vector>


class Model
{
public:
    Model();
    ~Model();
    void draw() const;
    void build();
    void unBuild();

    void drawCube() const;
    void buildCube();
    void unBuildCube();

public:
    unsigned    mVBO;
    unsigned    mEBO;
    unsigned    mVAO;
    std::vector<unsigned short> mIndices;

    unsigned    mVBOCube;
    unsigned    mEBOCube;
    unsigned    mVAOCube;
    std::vector<unsigned short> mIndicesCube;
};

#endif
