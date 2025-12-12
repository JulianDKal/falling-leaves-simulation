#pragma once
#include "glm/glm.hpp"
#include "GL/glew.h"
#include "Shader.h"


class Leaf
{
private:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f);

    glm::mat4 model;
    float drag = 0.66f;
    float size = 0.6f;
    float fallingSpeed = 0.02f;
    // unsigned int vao, vbo, ebo;
    
public:
    Leaf(const glm::vec3& pos, float speedVariation);
    void setRotation(const glm::vec3& newRotation);
    void addRotation(const glm::vec3& newRotation);
    const glm::mat4& getLeafModel() const;
    void update(float dT);
    ~Leaf();    
};


