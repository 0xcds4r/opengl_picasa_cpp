///////////////////////////////////////////////////////////////////////////////
//                                                                          //
// FILE :    texture.h                                                     //
// AUTHOR :  0xcds4r                                                      //
// CREATED : 25/03/2025                                                  //
//                                                                      //
/////////////////////////////////////////////////////////////////////////

#pragma once

#include <GL/glew.h>
#include <string>
#include <memory>

class Texture {
public:
    Texture();
    ~Texture();

    bool loadFromFile(const std::string& path);
    bool loadFromMemory(const unsigned char* data, int width, int height, int channels);
    void bind(unsigned int slot = 0);
    
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    int getChannels() const { return m_channels; }
    GLuint getId() const { return m_id; }
    
    std::unique_ptr<Texture> createThumbnail(int size);

private:
    GLuint m_id;
    int m_width;
    int m_height;
    int m_channels;
    
    void generateTexture();
    GLenum getInternalFormat() const;
    GLenum getFormat() const;
};
