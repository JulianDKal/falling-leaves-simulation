#pragma once
#include <vector>
#include "Leaf.h"
#include "glm/glm.hpp"
#include <random>

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
public:
    int instancesCount();
    void update();
    void draw(const glm::mat4& view, const glm::mat4& projection);
    Emitter(int count);
    ~Emitter();
};
