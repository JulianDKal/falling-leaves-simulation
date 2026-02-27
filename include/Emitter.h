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

static float pointVertices[] = {
    0.0f, 0.0f, 0.0f
};

class Emitter
{
private:
    std::vector<Leaf> leaves;
    unsigned int leafVAO, leafVBO, leafEBO;
    std::vector<glm::vec3>* sphereCoordinates, *sphereNormals;
    std::vector<unsigned int>* sphereIndices;
    unsigned int sphereVAO, sphereVBO, sphereEBO, sphereNormalsVBO;
    unsigned int pointVAO, pointVBO;

    //store the rotation matrices, positions, rotations and current velocity for each leaf.
    unsigned int transformationsSSBO, rotationsSSBO, velocitySSBO; 
    Shader computeShader;
    int numInstances;
    Shader leafShader, sphereShader, pointShader;
    Texture leafTexture;

    float rotationSpeed = 0.3f;
    float physicsAccumulator = 0.0f; // for fixed timestep
    const float fixedDT = 0.016f; // for fixed timestep
     Leaf createLeaf(const EmitterParams &params, std::mt19937 &gen, std::uniform_real_distribution<float> &rotDist);

    void uploadInitialTransforms();
public:
    void fixedUpdatePhysics(float fixedDT);
    void update(float dT, const EmitterParams& params);
    void draw(const glm::mat4& view, const glm::mat4& projection, const EmitterParams& params);
    void resizeParticleCount(const EmitterParams& params);
    void changeEmitArea(const EmitterParams& params);
    Emitter(const EmitterParams& params);
    ~Emitter();
};
