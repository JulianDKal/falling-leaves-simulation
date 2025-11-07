#pragma once
#include "glm/glm.hpp"
#include "GL/glew.h"
#include "Shader.h"

static float leafVertices[] = {
        -0.5f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f
    };

class Leaf
{
private:
    glm::vec3 position;
    glm::vec3 rotation;
    float size = 1;
    unsigned int vao, vbo;
    
public:
    Leaf(const glm::vec3& pos);
    void draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection);
    ~Leaf();    
};


