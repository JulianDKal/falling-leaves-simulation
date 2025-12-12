#include "Leaf.h"

Leaf::Leaf(const glm::vec3& pos, float speedVariation)
{
    position = pos;
    velocity.y *= speedVariation;
}

void Leaf::setRotation(const glm::vec3 &newRotation)
{
    rotation = newRotation;
}

void Leaf::addRotation(const glm::vec3& newRotation) {
    rotation += newRotation;
}

const glm::mat4 &Leaf::getLeafModel() const
{
    return model;
}

void Leaf::update(float dT)
{
    getErrorCode();

    model = glm::mat4(1.0f);

    velocity += gravity * dT;

    velocity *= drag;

    position += velocity * dT;
    if(position.y < 0) {
        position.y = 15.0f;
        velocity.x = 0;
        velocity.y = 0;
        velocity.z = 0;
    }
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(size));
    
    
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    // std::cout << rotation.x << " " << rotation.y << " " << rotation.z << std::endl;
    getErrorCode();
}

Leaf::~Leaf()
{
}