///////////////////////////////////////////////////////////////////////////////
//                                                                          //
// FILE :    picasa_app.h                                                  //
// AUTHOR :  0xcds4r                                                      //
// CREATED : 25/03/2025                                                  //
//                                                                      //
/////////////////////////////////////////////////////////////////////////

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>
#include <memory>

class Shader;
class Texture;

class PicasaApp {
public:
    PicasaApp();
    virtual ~PicasaApp();

    bool initialize(int width, int height, const std::string& title);
    virtual void run();
    void loadFolder(const std::string& folderPath);
    void loadImage(const std::string& imagePath);
    
    // TODO
protected:
    GLFWwindow* m_window;
    int m_width;
    int m_height;
    
    std::unique_ptr<Shader> m_shader;
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_ebo;
    
    std::vector<std::string> m_imageFiles;
    int m_currentIndex;
    std::unique_ptr<Texture> m_currentTexture;
    std::string m_current_image_path;
    
    float m_scale;
    glm::vec2 m_offset;
    float m_rotation;
    glm::vec2 m_dragStart;
    bool m_isDragging;
    
    bool m_showThumbnails;
    std::vector<std::unique_ptr<Texture>> m_thumbnails;
    int m_thumbnailSize;
    
    void setupShaders();
    void setupGeometry();
    
    void render();
    void renderImage();
    void renderThumbnails();
    void renderUI();
    
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    
    void updateViewTransform();
    void nextImage();
    void previousImage();
    void generateThumbnails();
    std::vector<std::string> getImageFilesInFolder(const std::string& folderPath);
};
