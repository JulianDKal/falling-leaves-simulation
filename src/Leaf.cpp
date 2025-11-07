#include "Leaf.h"

Leaf::Leaf(const glm::vec3& pos)
{
    position = pos;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(leafVertices), leafVertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

}

void Leaf::draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection)
{
    getErrorCode();
    glBindVertexArray(vao);
    glUseProgram(shader.ID);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(size));
    
    // model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    // model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    // model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    
    // Set the matrices as uniforms
    shader.setMatrix4("model", model);
    shader.setMatrix4("view", view);
    shader.setMatrix4("projection", projection);

    glDrawArrays(GL_TRIANGLES, 0, 3);
    getErrorCode();
}

Leaf::~Leaf()
{
}