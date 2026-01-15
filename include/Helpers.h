#pragma once
#include <iostream>
#include "GL/glew.h"
#include "glm/glm.hpp"
#include <vector>

static inline float pi = static_cast<float>(std::numbers::pi);

enum class EmitterShape {
    boxShape,
    circleShape
};

//This is the struct that gets passed to the UI and the Emitter. When the user interacts with the UI,
//the instance of this struct that gets passed around changes. The emitter then applies these changes to the simulation
//This also gets passed to the leaf update method
struct EmitterParams {
    glm::vec3 windForce;
    float size = 1.0f; //Leaf size
    float gravity = 9.81f;
    bool spiralingMotion = false;
    bool tumbling = false; 
    int leafCount;
    float emitRadius;
    float emitHeight;
    EmitterShape shape;
};

//Used to generate the vertex data for the circle shape gizmos
inline std::vector<glm::vec3>* generateCirclePoints(int numOfPoints) {
    float degToAdvance = 360.0f / (float)numOfPoints;
    float currentDeg = 0;
    std::vector<glm::vec3>* resultVec = new std::vector<glm::vec3>(numOfPoints);
    for (int i = 0; i < numOfPoints; i++)
    {
        currentDeg = i * degToAdvance;
        float xVal = cos(glm::radians(currentDeg));
        float zVal = sin(glm::radians(currentDeg));
        glm::vec3 newPoint = {xVal, 0, zVal};
        resultVec->at(i) = newPoint;
    }
    return resultVec;
}

// sectorCount is the number of horizontal sectors, stackCounts the number of vertical sectors 
inline std::vector<glm::vec3>* generateSpherePoints(int sectorCount, int stackCount){
    std::vector<glm::vec3>* result = new std::vector<glm::vec3>();
    float sectorStep = 2 * pi / sectorCount;
    float stackStep = pi / stackCount;
    float sectorAngle, stackAngle;
    glm::vec3 coordinate;

    for (int i = 0; i < stackCount; i++)
    {
        stackAngle = pi / 2 - i * stackStep; // starting from pi/2 to -pi/2
        coordinate.x = cosf(stackAngle); 
        coordinate.y = sinf(stackAngle);
        coordinate.z = cosf(stackAngle);

        for (int j = 0; j < sectorCount; j++)
        {
            coordinate.x *= cosf(sectorAngle);
            coordinate.z *= sinf(sectorAngle);
            result->push_back(coordinate);
        }
        
    }
    return result;
    
}

inline GLenum getErrorCode_(const char* file, int line) {
    GLenum errorCode;
    while((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode)
        {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }

    return errorCode;
}

#define getErrorCode() getErrorCode_(__FILE__, __LINE__)