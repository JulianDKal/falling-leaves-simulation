#include "Texture.h"

Texture::Texture()
{
    textureHandle = 0;
    textureUnitIndex = 0;
}

int Texture::initialize(const std::string& imagePath, unsigned int texUnit)
{
    getErrorCode();
    textureUnitIndex = texUnit;
    // glDeleteTextures(1, &textureHandle);

    if(!std::filesystem::exists(imagePath)) {
        std::cerr << "The image path specified could not be found: " << imagePath << std::endl;
        return 0;
    }
    glGenTextures(1, &textureHandle);
    glActiveTexture(GL_TEXTURE0 + textureUnitIndex);

    getErrorCode();
    glBindTexture(GL_TEXTURE_2D, textureHandle);

    getErrorCode();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int desiredChannels = 3;
    GLenum imageFormat, internalGlFormat;
    std::filesystem::path path(imagePath);
    if(path.extension() == ".png") {
        desiredChannels = 4;
        imageFormat = GL_RGBA;
        internalGlFormat = GL_RGBA;
    }
    else {
        desiredChannels = 3;
        imageFormat = GL_RGB;
        internalGlFormat = GL_RGB;
    }

    unsigned char* gridImageData = stbi_load(imagePath.c_str(), &width, &height, &nrChannels, desiredChannels);

    std::cout << "Processed Texture " << path.filename() << " | width: " << width << " height: " << height << " nrChannels: " << nrChannels << std::endl;

    if(gridImageData) {
    glTexImage2D(GL_TEXTURE_2D, 0, internalGlFormat, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, gridImageData);
    glGenerateMipmap(GL_TEXTURE_2D);

    getErrorCode();
    }
    else {
        std::cerr << "STB_image error: " << stbi_failure_reason() << std::endl;
        return 0;
    }

    stbi_image_free(gridImageData);
    return 1;
}

unsigned int Texture::getTextureUnit() const
{
    return textureUnitIndex;
}

unsigned int Texture::getHandle() const
{
    return textureHandle;
}
