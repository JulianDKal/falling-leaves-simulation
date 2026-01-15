#include "Leaf.h"

Leaf::Leaf(const glm::vec3 &pos, const glm::vec3 &rot)
{
    position = pos;
    rotation = rot;
}

glm::mat4 &Leaf::getLeafModel()
{
    model = glm::mat4(1.0);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    return model;
}

const glm::vec3& Leaf::getPosition() const
{
    return position;
}

const glm::vec3& Leaf::getRotation() const
{
    return rotation;
}

Leaf::~Leaf()
{
}