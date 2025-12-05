#include "Camera.h"

void Camera::zoom(float modifier) {
    radius -= modifier;
}

void Camera::rotate(float modifierX, float modifierY) {
    rotation.x -= modifierX; //rotation to the left and right, around the vertical axis
    rotation.y += modifierY; //rotation up and down, around the horizontal axis
    rotation.y = glm::clamp((float)rotation.y, 0.0f, (float)std::numbers::pi / 2.05f);

    glm::vec3 hello = glm::vec3(1.0f);
}

void Camera::update() {
    //calculate spherical coordinates for the camera position
    float camX = sin(rotation.x) * cos(rotation.y) * radius;
    float camY = sin(rotation.y) * radius;
    float camZ = cos(rotation.x) * cos(rotation.y) * radius;

    glm::vec3 cameraUp = glm::vec3(0, 1.0f, 0);

    viewMatrix = glm::mat4(1.0f);
    viewMatrix = glm::lookAt(glm::vec3(camX, camY, camZ), glm::vec3(0.0, 0.0, 0.0), cameraUp);
}

const glm::mat4 &Camera::getViewMatrix() const
{
    return viewMatrix;
}

Camera::Camera()
{
    radius = 15.0f;
    rotation.y = glm::radians(40.0f);
    rotation.x = glm::radians(40.0f);
}

Camera::~Camera()
{
}
