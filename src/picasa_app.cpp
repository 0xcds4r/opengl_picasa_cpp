///////////////////////////////////////////////////////////////////////////////
//                                                                          //
// FILE :    picasa_app.cpp                                                //
// AUTHOR :  0xcds4r                                                      //
// CREATED : 25/03/2025                                                  //
//                                                                      //
/////////////////////////////////////////////////////////////////////////

#include "picasa_app.h"
#include "shader.h"
#include "texture.h"

#include <iostream>
#include <filesystem>
#include <algorithm>
#include <cmath>

namespace fs = std::filesystem;

static PicasaApp* g_appInstance = nullptr;

PicasaApp::PicasaApp() 
    : m_window(nullptr), 
      m_width(800), 
      m_height(600),
      m_vao(0), 
      m_vbo(0), 
      m_ebo(0),
      m_currentIndex(0),
      m_scale(1.0f),
      m_offset(0.0f, 0.0f),
      m_rotation(0.0f),
      m_isDragging(false),
      m_showThumbnails(true),
      m_thumbnailSize(150)
{
    g_appInstance = this;
}

PicasaApp::~PicasaApp() 
{
    if (m_vao != 0) {
        glDeleteVertexArrays(1, &m_vao);
    }
    if (m_vbo != 0) {
        glDeleteBuffers(1, &m_vbo);
    }
    if (m_ebo != 0) {
        glDeleteBuffers(1, &m_ebo);
    }

    glfwTerminate();
}

bool PicasaApp::initialize(int width, int height, const std::string& title) 
{
    m_width = width;
    m_height = height;
    
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(m_window);
    
    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);
    glfwSetKeyCallback(m_window, keyCallback);
    glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
    glfwSetCursorPosCallback(m_window, cursorPosCallback);
    glfwSetScrollCallback(m_window, scrollCallback);
    
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        return false;
    }
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    setupShaders();
    setupGeometry();
    
    return true;
}

void PicasaApp::run() 
{
    while (!glfwWindowShouldClose(m_window)) 
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        render();
        
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

void PicasaApp::loadFolder(const std::string& folderPath) 
{
    m_imageFiles = getImageFilesInFolder(folderPath);
    
    if (!m_imageFiles.empty()) 
    {
        m_currentIndex = 0;
        loadImage(m_imageFiles[m_currentIndex]);
        generateThumbnails();
    }
}

void PicasaApp::loadImage(const std::string& imagePath) 
{
    m_currentTexture = std::make_unique<Texture>();
    if (!m_currentTexture->loadFromFile(imagePath)) {
        std::cerr << "Failed to load image: " << imagePath << std::endl;
        m_currentTexture.reset();
        return;
    }
    
    m_scale = 1.0f;
    m_offset = glm::vec2(0.0f, 0.0f);
    m_rotation = 0.0f;
    
    auto it = std::find(m_imageFiles.begin(), m_imageFiles.end(), imagePath);
    if (it != m_imageFiles.end()) {
        m_currentIndex = std::distance(m_imageFiles.begin(), it);
    }
}

void PicasaApp::setupShaders() 
{
    m_shader = std::make_unique<Shader>();
    if (!m_shader->loadFromFiles("shaders/image.vert", "shaders/image.frag")) {
        std::cerr << "Failed to load shaders" << std::endl;
    }
}

void PicasaApp::setupGeometry() 
{
    float vertices[] = 
    {
        // positions          // texture coords
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,   // bottom left
         0.5f, -0.5f, 0.0f,   1.0f, 0.0f,   // bottom right
         0.5f,  0.5f, 0.0f,   1.0f, 1.0f,   // top right
        -0.5f,  0.5f, 0.0f,   0.0f, 1.0f    // top left
    };
    
    unsigned int indices[] = 
    {
        0, 1, 2,   // first triangle
        2, 3, 0    // second triangle
    };
    
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void PicasaApp::render() 
{
    if (m_showThumbnails) 
    {
        renderThumbnails();
    } 
    else 
    {
        renderImage();
    }
    
    renderUI();
}

void PicasaApp::renderImage() 
{
    if (!m_currentTexture || !m_shader) {
        return;
    }
    
    m_shader->use();
    
    float imageAspect = static_cast<float>(m_currentTexture->getWidth()) / m_currentTexture->getHeight();
    float windowAspect = static_cast<float>(m_width) / m_height;
    
    float scaleX, scaleY;
    if (imageAspect > windowAspect) 
    {
        scaleX = 1.0f;
        scaleY = windowAspect / imageAspect;
    } 
    else 
    {
        scaleX = imageAspect / windowAspect;
        scaleY = 1.0f;
    }
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(m_offset.x, m_offset.y, 0.0f));
    model = glm::rotate(model, glm::radians(m_rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(scaleX * m_scale, scaleY * m_scale, 1.0f));
    
    glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    
    m_shader->setMat4("model", glm::value_ptr(model));
    m_shader->setMat4("projection", glm::value_ptr(projection));
    
    m_currentTexture->bind(0);
    m_shader->setInt("imageTexture", 0);
    
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void PicasaApp::renderThumbnails() 
{
    if (m_thumbnails.empty() || !m_shader) {
        return;
    }
    
    m_shader->use();
    
    int cols = m_width / (m_thumbnailSize + 10);
    if (cols < 1) cols = 1;
    
    int rows = (m_thumbnails.size() + cols - 1) / cols;
    float cellWidth = 2.0f / cols;
    float cellHeight = 2.0f / rows;
    
    glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    m_shader->setMat4("projection", glm::value_ptr(projection));
    
    for (size_t i = 0; i < m_thumbnails.size(); i++) 
    {
        int row = i / cols;
        int col = i % cols;
        
        float x = -1.0f + col * cellWidth + cellWidth * 0.5f;
        float y = 1.0f - row * cellHeight - cellHeight * 0.5f;
        
        float thumbAspect = static_cast<float>(m_thumbnails[i]->getWidth()) / m_thumbnails[i]->getHeight();
        float scaleX = cellWidth * 0.9f;
        float scaleY = scaleX / thumbAspect;
        
        if (scaleY > cellHeight * 0.9f) 
        {
            scaleY = cellHeight * 0.9f;
            scaleX = scaleY * thumbAspect;
        }
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x, y, 0.0f));
        model = glm::scale(model, glm::vec3(scaleX, scaleY, 1.0f));
        
        if (i == static_cast<size_t>(m_currentIndex)) {
            model = glm::scale(model, glm::vec3(1.1f, 1.1f, 1.0f));
        }
        
        m_shader->setMat4("model", glm::value_ptr(model));
        
        m_thumbnails[i]->bind(0);
        m_shader->setInt("imageTexture", 0);
        
        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    
    glBindVertexArray(0);
}

void PicasaApp::renderUI() {
    // TODO
}

void PicasaApp::updateViewTransform() {
    m_scale = std::max(0.1f, std::min(m_scale, 10.0f));
}

void PicasaApp::nextImage() {
    if (m_imageFiles.empty()) {
        return;
    }
    
    m_currentIndex = (m_currentIndex + 1) % m_imageFiles.size();
    loadImage(m_imageFiles[m_currentIndex]);
}

void PicasaApp::previousImage() {
    if (m_imageFiles.empty()) {
        return;
    }
    
    m_currentIndex = (m_currentIndex - 1 + m_imageFiles.size()) % m_imageFiles.size();
    loadImage(m_imageFiles[m_currentIndex]);
}

void PicasaApp::generateThumbnails() {
    m_thumbnails.clear();
    
    for (const auto& imagePath : m_imageFiles) {
        auto texture = std::make_unique<Texture>();
        if (texture->loadFromFile(imagePath)) {
            auto thumbnail = texture->createThumbnail(m_thumbnailSize);
            if (thumbnail) {
                m_thumbnails.push_back(std::move(thumbnail));
            }
        }
    }
}

std::vector<std::string> PicasaApp::getImageFilesInFolder(const std::string& folderPath) 
{
    std::vector<std::string> imageFiles;
    
    try {
        for (const auto& entry : fs::directory_iterator(folderPath)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                
                if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".gif") {
                    imageFiles.push_back(entry.path().string());
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error reading directory: " << e.what() << std::endl;
    }
    
    return imageFiles;
}

void PicasaApp::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    if (g_appInstance) {
        g_appInstance->m_width = width;
        g_appInstance->m_height = height;
        glViewport(0, 0, width, height);
    }
}

void PicasaApp::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (!g_appInstance) {
        return;
    }
    
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, true);
                break;
            case GLFW_KEY_RIGHT:
                g_appInstance->nextImage();
                break;
            case GLFW_KEY_LEFT:
                g_appInstance->previousImage();
                break;
            case GLFW_KEY_UP:
                g_appInstance->m_rotation += 90.0f;
                break;
            case GLFW_KEY_DOWN:
                g_appInstance->m_rotation -= 90.0f;
                break;
            case GLFW_KEY_TAB:
                g_appInstance->m_showThumbnails = !g_appInstance->m_showThumbnails;
                break;
            case GLFW_KEY_SPACE:
                g_appInstance->m_scale = 1.0f;
                g_appInstance->m_offset = glm::vec2(0.0f, 0.0f);
                g_appInstance->m_rotation = 0.0f;
                break;
        }
    }
}

void PicasaApp::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (!g_appInstance) {
        return;
    }
    
    if (button == GLFW_MOUSE_BUTTON_LEFT) 
    {
        if (action == GLFW_PRESS) 
        {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            g_appInstance->m_dragStart = glm::vec2(xpos, ypos);
            g_appInstance->m_isDragging = true;
            
            if (g_appInstance->m_showThumbnails) 
            {
                float ndcX = (2.0f * xpos / g_appInstance->m_width) - 1.0f;
                float ndcY = 1.0f - (2.0f * ypos / g_appInstance->m_height);
                
                int cols = g_appInstance->m_width / (g_appInstance->m_thumbnailSize + 10);
                if (cols < 1) cols = 1;
                
                float cellWidth = 2.0f / cols;
                float cellHeight = cellWidth * (g_appInstance->m_height / static_cast<float>(g_appInstance->m_width));
                
                int col = static_cast<int>((ndcX + 1.0f) / cellWidth);
                int row = static_cast<int>((1.0f - ndcY) / cellHeight);
                
                int index = row * cols + col;
                if (index >= 0 && index < g_appInstance->m_thumbnails.size()) 
                {
                    g_appInstance->m_currentIndex = index;
                    g_appInstance->loadImage(g_appInstance->m_imageFiles[index]);
                    g_appInstance->m_showThumbnails = false;
                }
            }
        } else if (action == GLFW_RELEASE) 
        {
            g_appInstance->m_isDragging = false;
        }
    }
}

void PicasaApp::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) 
{
    if (!g_appInstance || !g_appInstance->m_isDragging) {
        return;
    }
    
    glm::vec2 currentPos(xpos, ypos);
    glm::vec2 delta = currentPos - g_appInstance->m_dragStart;
    
    delta.x /= g_appInstance->m_width * 0.5f;
    delta.y /= g_appInstance->m_height * 0.5f;
    
    delta.y = -delta.y;

    g_appInstance->m_offset += delta;
    g_appInstance->m_dragStart = currentPos;
}

void PicasaApp::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) 
{
    if (!g_appInstance) {
        return;
    }
    
    if (yoffset > 0) {
        g_appInstance->m_scale *= 1.1f;
    } else {
        g_appInstance->m_scale /= 1.1f;
    }
    
    g_appInstance->updateViewTransform();
}
