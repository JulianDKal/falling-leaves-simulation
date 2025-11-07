#pragma once
#include <GL/glew.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include "glm/gtc/type_ptr.hpp"
#include "Texture.h"
#include "Helpers.h"

class Shader
{
private:
    unsigned int vS = 0, fS = 0;
public:
    unsigned int ID = 0; //For the shader program
    Shader();
    void createProgram(std::filesystem::path vShaderPath, std::filesystem::path fShaderPath);
    ~Shader();
    void useTexture(const Texture& texture, std::string samplerName);
    const void setBool(const std::string &name, bool value);
    const void setInt(const std::string &name, int value);
    const void setFloat(const std::string &name, float value);
    const void setVec3f(const std::string &name, glm::vec3 value);
    const void setMatrix4(const std::string& name, glm::mat4 matrix);
};
