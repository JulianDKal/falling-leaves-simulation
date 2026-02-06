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

enum class ParticleShape {
    leafShape, 
    sphereShape, 
    pointShape
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
    ParticleShape particleShape;
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
inline std::vector<glm::vec3>* generateSpherePoints(int sectorCount, int stackCount, float radius){
    std::vector<glm::vec3>* result = new std::vector<glm::vec3>();
    float sectorStep = 2 * pi / sectorCount;
    float stackStep = pi / stackCount;
    float sectorAngle, stackAngle;
    float xz, y;

    for (int i = 0; i <= stackCount; i++)
    {
        stackAngle = pi / 2 - i * stackStep; // starting from pi/2 to -pi/2
        xz = cosf(stackAngle); 
        y = sinf(stackAngle);

        for (int j = 0; j <= sectorCount; j++)
        {
            sectorAngle = j * sectorStep;

            glm::vec3 coordinate;
            coordinate.x = xz * radius * cosf(sectorAngle);
            coordinate.y = radius * y;
            coordinate.z = xz * radius * sinf(sectorAngle);
            result->push_back(coordinate);  
        }
    }
    return result;
}

inline std::vector<unsigned int>* generateSphereIndices(int sectorCount, int stackCount){
    std::vector<unsigned int>* indices = new std::vector<unsigned int>(); 
    // k1--k1+1
    // |  / |
    // | /  |
    // k2--k2+1

    int k1, k2;
    for(int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if(i != 0) //We don't have to do this if we are at the beginning
            {
                indices->push_back(k1);
                indices->push_back(k2);
                indices->push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if(i != (stackCount-1)) //We don't have to do this if we are at the last line
            {
                indices->push_back(k1 + 1);
                indices->push_back(k2);
                indices->push_back(k2 + 1);
            }

        }
    }

    return indices;
        
}

inline std::vector<glm::vec3>* generateSphereNormals(int sectorCount, int stackCount) {
    std::vector<glm::vec3>* result = new std::vector<glm::vec3>();
    float sectorStep = 2 * pi / sectorCount;
    float stackStep = pi / stackCount;
    float sectorAngle, stackAngle;
    float xz, y;

    for (int i = 0; i <= stackCount; i++)
    {
        stackAngle = pi / 2 - i * stackStep; // starting from pi/2 to -pi/2
        xz = cosf(stackAngle); 
        y = sinf(stackAngle);

        for (int j = 0; j <= sectorCount; j++)
        {
            sectorAngle = j * sectorStep;

            glm::vec3 normal;
            normal.x = xz * cosf(sectorAngle);
            normal.y = y;
            normal.z = xz * sinf(sectorAngle);
            result->push_back(normal);  
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