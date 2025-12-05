#pragma once
#include "glm/glm.hpp"
#include "GL/glew.h"
#include "Shader.h"

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

class Leaf
{
private:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::mat4 model;
    float size = 0.3f;
    unsigned int vao, vbo, ebo;
    
public:
    Leaf(const glm::vec3& pos);
    void setRotation(const glm::vec3& newRotation);
    void addRotation(const glm::vec3& newRotation);
    void draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection);
    ~Leaf();    
};


