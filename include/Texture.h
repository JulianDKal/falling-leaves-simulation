#pragma once
#include "stb_image.h"
#include <string>
#include <filesystem>
#include <iostream>
#include "GL/glew.h"
#include "Helpers.h"

class Texture {
private: 
    unsigned int textureHandle; //The handle that his texture is identified with
    unsigned int textureUnitIndex; //Set with glActiveTexture(), sets the active texture unit that the sampler has to use

    int width, height, nrChannels;
public: 
    Texture();
    int initialize(const std::string& imagePath, unsigned int texUnit);
    unsigned int getTextureUnit() const;
    unsigned int getHandle() const;
};