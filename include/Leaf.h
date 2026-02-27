#pragma once
#include "glm/glm.hpp"
#include "GL/glew.h"
#include "Shader.h"
#include "Helpers.h"

class Leaf
{
private:
    glm::vec3 rotation;
    glm::mat4 model;
    
public:
    Leaf() = default;
    Leaf(const glm::vec3& rot);
    glm::mat4& getLeafModel();
    const glm::vec3& getRotation() const;
    ~Leaf();    
};


