#pragma once
#include <vector>
#include "Leaf.h"
#include "glm/glm.hpp"
#include <random>

static float leafVertices[] = {
    //   position                        UV
    -0.5f,  -0.5f, 0.0f,     0.0f,  0.0f,   // bottom-left
    0.5f,   -0.5f, 0.0f,     1.0f,  0.0f,   // bottom-right
    0.5f,  0.5f, 0.0f,    1.0f, 1.0f,   // top-right
    -0.5f, 0.5f, 0.0f,    0.0f, 1.0f    // top-left
};

static unsigned int leafIndices[] = {
    0, 1, 2,
    2, 3, 0
};

class Emitter
{
private:
    std::vector<Leaf> leaves;
    std::vector<glm::mat4> transformations; //Holds all the transformation data for the leaves
    unsigned int leafVAO, leafVBO, leafEBO;
    unsigned int transformationsVBO;
    int numInstances;
    Shader leafShader;
    Texture leafTexture;

    float rotationSpeed = 0.3f;
    void updateTransformBuffer();
public:
    int instancesCount();
    void update();
    void draw(const glm::mat4& view, const glm::mat4& projection);
    Emitter(int count);
    ~Emitter();
};
