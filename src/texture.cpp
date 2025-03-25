///////////////////////////////////////////////////////////////////////////////
//                                                                          //
// FILE :    texture.cpp                                                   //
// AUTHOR :  0xcds4r                                                      //
// CREATED : 25/03/2025                                                  //
//                                                                      //
/////////////////////////////////////////////////////////////////////////

#include "texture.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image_resize.h>

Texture::Texture() : m_id(0), m_width(0), m_height(0), m_channels(0) {
}

Texture::~Texture() {
    if (m_id != 0) {
        glDeleteTextures(1, &m_id);
    }
}

bool Texture::loadFromFile(const std::string& path) 
{
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &m_width, &m_height, &m_channels, 0);
    
    if (!data) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        std::cerr << "Reason: " << stbi_failure_reason() << std::endl;
        return false;
    }
    
    generateTexture();
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, getInternalFormat(), m_width, m_height, 0, getFormat(), GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    stbi_image_free(data);
    
    return true;
}

bool Texture::loadFromMemory(const unsigned char* data, int width, int height, int channels) 
{
    if (!data) {
        std::cerr << "Failed to load texture from memory: null data pointer" << std::endl;
        return false;
    }
    
    m_width = width;
    m_height = height;
    m_channels = channels;
    
    generateTexture();
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, getInternalFormat(), m_width, m_height, 0, getFormat(), GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    return true;
}

void Texture::bind(unsigned int slot) 
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

std::unique_ptr<Texture> Texture::createThumbnail(int size) 
{
    if (m_id == 0) {
        return nullptr;
    }
    
    int thumbWidth, thumbHeight;
    if (m_width > m_height) 
    {
        thumbWidth = size;
        thumbHeight = static_cast<int>(size * (static_cast<float>(m_height) / m_width));
    } 
    else 
    {
        thumbHeight = size;
        thumbWidth = static_cast<int>(size * (static_cast<float>(m_width) / m_height));
    }
    
    unsigned char* resizedData = new unsigned char[thumbWidth * thumbHeight * m_channels];
    
    glBindTexture(GL_TEXTURE_2D, m_id);
    
    unsigned char* originalData = new unsigned char[m_width * m_height * m_channels];
    glGetTexImage(GL_TEXTURE_2D, 0, getFormat(), GL_UNSIGNED_BYTE, originalData);
    
    stbir_resize_uint8(originalData, m_width, m_height, 0,
                      resizedData, thumbWidth, thumbHeight, 0, m_channels);
    
    auto thumbnail = std::make_unique<Texture>();
    thumbnail->loadFromMemory(resizedData, thumbWidth, thumbHeight, m_channels);
    
    delete[] originalData;
    delete[] resizedData;
    
    return thumbnail;
}

void Texture::generateTexture() 
{
    if (m_id != 0) {
        glDeleteTextures(1, &m_id);
    }
    
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

GLenum Texture::getInternalFormat() const {
    switch (m_channels) {
        case 1: return GL_RED;
        case 3: return GL_RGB;
        case 4: return GL_RGBA;
        default: return GL_RGB;
    }
}

GLenum Texture::getFormat() const {
    switch (m_channels) {
        case 1: return GL_RED;
        case 3: return GL_RGB;
        case 4: return GL_RGBA;
        default: return GL_RGB;
    }
}
