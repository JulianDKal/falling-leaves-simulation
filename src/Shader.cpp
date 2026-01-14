#include "Shader.h"

Shader::Shader()
{

}

void Shader::createProgram(std::filesystem::path vShaderPath, std::filesystem::path fShaderPath)
{
    std::fstream vShaderFile, fShaderFile;
    std::stringstream vShaderStream, fShaderStream;
    std::string vShaderString, fShaderString;

    if (!std::filesystem::exists(vShaderPath)) {
        std::cerr << "Vertex shader file not found: " << vShaderPath << std::endl;
        return; // or throw an exception
    }
    if (!std::filesystem::exists(fShaderPath)) {
        std::cerr << "Fragment shader file not found: " << fShaderPath << std::endl;
        return; // or throw an exception
    }

    vShaderFile.open(vShaderPath);
    fShaderFile.open(fShaderPath);

    // Check if files opened successfully
    if (!vShaderFile.is_open()) {
        std::cerr << "Failed to open vertex shader file: " << vShaderPath << std::endl;
        return;
    }
    if (!fShaderFile.is_open()) {
        std::cerr << "Failed to open fragment shader file: " << fShaderPath << std::endl;
        vShaderFile.close(); // Clean up the other file
        return;
    }
    
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();

    //Can't do the following line! the string returned by getShaderText is temporary and gets destroyed after this line
    //const char* shaderText = getShaderText(vertexShaderPath).c_str();

    vShaderString = vShaderStream.str();
    fShaderString = fShaderStream.str();

    const char* vShader = vShaderString.c_str();
    const char* fShader = fShaderString.c_str();

    
    vS = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vS, 1, &vShader, NULL);
    glCompileShader(vS);

    int success;
    char infoLog[512];
    glGetShaderiv(vS, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vS, 512, NULL, infoLog);
        std::cerr << "ERROR::VERTEX_SHADER_COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    fS = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fS, 1, &fShader, NULL);
    glCompileShader(fS);

    glGetShaderiv(fS, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fS, 512, NULL, infoLog);
        std::cerr << "ERROR::FRAGMENT_SHADER_COMPILATION_FAILED\n" << infoLog << std::endl;
    }


    ID = glCreateProgram();

    glAttachShader(ID, vS);
    glAttachShader(ID, fS);

    glLinkProgram(ID);
    

    glDeleteShader(vS);
    glDeleteShader(fS);
    vS = fS = 0;

}

void Shader::createComputeProgram(std::filesystem::path computeShaderPath)
{
    std::fstream computeShaderFile;
    std::stringstream computeShaderStream;
        if (!std::filesystem::exists(computeShaderPath)) {
        std::cerr << "compute shader file not found: " << computeShaderPath << std::endl;
        return; // or throw an exception
    }

    computeShaderFile.open(computeShaderPath);
    if(!computeShaderFile.is_open()){
        std::cerr << "Failed to open compute shader file: " << computeShaderPath << std::endl;
        return;
    }
    
    computeShaderStream << computeShaderFile.rdbuf();
    std::string computeShader = computeShaderStream.str();
    const char* computeShaderText = computeShader.c_str();

    unsigned int compute = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute, 1, &computeShaderText, NULL);
    glCompileShader(compute);

    int success;
    char infoLog[512];
    glGetShaderiv(compute, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(compute, 512, NULL, infoLog);
        std::cerr << "ERROR::COMPUTE_SHADER_COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    ID = glCreateProgram();
    glAttachShader(ID, compute);
    glLinkProgram(ID);


}

Shader::~Shader()
{
}

void Shader::useTexture(const Texture &texture, std::string samplerName)
{
    glActiveTexture(GL_TEXTURE0 + texture.getTextureUnit());
    glBindTexture(GL_TEXTURE_2D, texture.getHandle());
    setInt(samplerName, texture.getTextureUnit());
}

const void Shader::setBool(const std::string &name, bool value)
{
    glUseProgram(ID);

    int uniformLocation = glGetUniformLocation(ID, name.c_str());
        if (uniformLocation == -1) {
        std::cerr << "ERROR::SHADER::UNIFORM_NOT_FOUND: " << name << std::endl;
        return; 
    }

    glUniform1f(uniformLocation, value); //setting the uniform
}

const void Shader::setInt(const std::string &name, int value)
{
    glUseProgram(ID);

    int uniformLocation = glGetUniformLocation(ID, name.c_str());
        if (uniformLocation == -1) {
        std::cerr << "ERROR::SHADER::UNIFORM_NOT_FOUND: " << name << std::endl;
        return; 
    }

    glUniform1i(uniformLocation, value); //setting the uniform
}

const void Shader::setFloat(const std::string &name, float value)
{
    glUseProgram(ID);

    int uniformLocation = glGetUniformLocation(ID, name.c_str());
        if (uniformLocation == -1) {
        std::cerr << "ERROR::SHADER::UNIFORM_NOT_FOUND: " << name << std::endl;
        return; 
    }

    glUniform1f(uniformLocation, value); //setting the uniform

}

const void Shader::setVec3f(const std::string &name, glm::vec3 value)
{
    glUseProgram(ID);

     int uniformLocation = glGetUniformLocation(ID, name.c_str());
        if (uniformLocation == -1) {
        std::cerr << "ERROR::SHADER::UNIFORM_NOT_FOUND: " << name << std::endl;
        return; 
    }

    glUniform3f(uniformLocation, value.x, value.y, value.z);
}

const void Shader::setMatrix4(const std::string& name, glm::mat4 matrix) {
    glUseProgram(ID);

     int uniformLocation = glGetUniformLocation(ID, name.c_str());
        if (uniformLocation == -1) {
        std::cerr << "ERROR::SHADER::UNIFORM_NOT_FOUND: " << name << std::endl;
        return; 
    }

    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(matrix));
}
