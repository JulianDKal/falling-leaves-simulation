#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>

class Camera
{
private:
    glm::vec3 camPos;
    glm::vec2 rotation;
    glm::mat4 viewMatrix;
    float radius = 15.0f;
public:
    Camera();
    void zoom(float modifier);
    //modifierX = rotation to the left and right, around the vertical axis
    //modifierY = rotation up and down, around the horizontal axis
    void rotate(float modifierX, float modifierY);
    void update();
    const glm::mat4& getViewMatrix() const;
    ~Camera();
};
