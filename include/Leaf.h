#pragma once
#include "glm/glm.hpp"
#include "GL/glew.h"
#include "Shader.h"
#include "Helpers.h"

class Leaf
{
private:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 force = glm::vec3(0.0f);
    glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 acceleration = glm::vec3(0.0f);
    glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f);

    glm::mat4 model;
    float mass = 1.0f;
    float drag = 0.9f;
    float size = 0.6f;
    float fallingSpeed = 0.02f;
    // unsigned int vao, vbo, ebo;
    
public:
    Leaf() = default;
    Leaf(const glm::vec3& pos, float speedVariation);
    // New constructor (custom velocity)
    Leaf(const glm::vec3& pos, const glm::vec3& initialVelocity, const glm::vec3& initialRotation);
    void setRotation(const glm::vec3& newRotation);
    void addRotation(const glm::vec3& newRotation);
    glm::mat4& getLeafModel();
    void physicsUpdate(float fixedDT);
    void update(const EmitterParams& params);
    ~Leaf();    
};


