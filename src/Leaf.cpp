#include "Leaf.h"

Leaf::Leaf(const glm::vec3& pos)
{
    position = pos;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glGenBuffers(1, &ebo);
    
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(leafVertices), leafVertices, GL_DYNAMIC_DRAW);

    // EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(leafIndices), leafIndices, GL_STATIC_DRAW);

    // POS (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // UV  (location = 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

}

void Leaf::setRotation(const glm::vec3 &newRotation)
{
    rotation = newRotation;
}

void Leaf::addRotation(const glm::vec3& newRotation) {
    rotation += newRotation;
}

void Leaf::draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection)
{
    getErrorCode();
    glBindVertexArray(vao);
    glUseProgram(shader.ID);

    model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(size));
    
    
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    // std::cout << rotation.x << " " << rotation.y << " " << rotation.z << std::endl;

    
    // Set the matrices as uniforms
    shader.setMatrix4("model", model);
    shader.setMatrix4("view", view);
    shader.setMatrix4("projection", projection);
    getErrorCode();

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    getErrorCode();
}

Leaf::~Leaf()
{
}