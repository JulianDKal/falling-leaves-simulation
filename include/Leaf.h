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
    glm::mat4 model;
    float fallingSpeed = 0.02f;
    // unsigned int vao, vbo, ebo;
    
public:
    Leaf() = default;
    Leaf(const glm::vec3& pos, float speedVariation);
    void setRotation(const glm::vec3& newRotation);
    void addRotation(const glm::vec3& newRotation);
    const glm::mat4& getLeafModel() const;
    void update(const EmitterParams& params);
    ~Leaf();    
};


