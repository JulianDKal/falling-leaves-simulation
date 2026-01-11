#pragma once
#include <vector>
#include "Leaf.h"
#include "glm/glm.hpp"
#include <random>
#include "Helpers.h"
#include "Profiler.h"
#include <iostream>
#include <utility>

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
    float totalTime;
    float physicsAccumulator = 0.0f; // for fixed timestep
    const float fixedDT = 0.016f; // for fixed timestep
    void updateTransformBuffer();

    Leaf createLeaf(const EmitterParams &params, std::mt19937 &gen,
                    std::uniform_real_distribution<float> &posDist,
                    std::uniform_real_distribution<float> &rotDist,
                    std::uniform_real_distribution<float> &speedDist,
                    std::uniform_real_distribution<float> &oneDist);

    glm::vec3 generateRandomRotation(std::mt19937 &gen,
                                     std::uniform_real_distribution<float> &rotDist);
                                     
public:
    int instancesCount();

    void fixedUpdatePhysics(float fixedDT);
    void update(float dT, const EmitterParams& params);
    void draw(const glm::mat4& view, const glm::mat4& projection);
    void setTimeUniform(float time);
    void resizeParticleCount(const EmitterParams& params);
    void changeEmitArea(const EmitterParams& params);
    Emitter(const EmitterParams& params);
    ~Emitter();
};
