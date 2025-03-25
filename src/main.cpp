///////////////////////////////////////////////////////////////////////////////
//                                                                          //
// FILE :    main.cpp                                                      //
// AUTHOR :  0xcds4r                                                      //
// CREATED : 25/03/2025                                                  //
//                                                                      //
/////////////////////////////////////////////////////////////////////////

#include "picasa_app.h"
#include "shader.h"
#include "texture.h"
#include "ui.h"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

static PicasaApp* g_appInstance = nullptr;

class PicasaAppWithUI : public PicasaApp {
public:
    PicasaAppWithUI() : PicasaApp(), m_uiManager(nullptr), m_infoLabel(nullptr) {
    }
    
    ~PicasaAppWithUI() {
        delete m_uiManager;
    }
    
    bool initialize(int width, int height, const std::string& title) {
        // Initialize base class
        if (!PicasaApp::initialize(width, height, title)) {
            return false;
        }
        
        m_uiManager = new UIManager();
        m_uiManager->initialize(width, height);
        
        setupUI();
        
        return true;
    }
    
    void run() override 
    {
        while (!glfwWindowShouldClose(m_window)) 
        {
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            
            render();
            
            m_uiManager->render();
            
            updateInfoLabel();
            
            glfwSwapBuffers(m_window);
            glfwPollEvents();
        }
    }
    
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        PicasaAppWithUI* app = static_cast<PicasaAppWithUI*>(g_appInstance);
        if (!app) return;
        
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            
            if (app->m_uiManager->handleMouseClick(xpos, ypos)) {
                return;
            }
        }
        
        PicasaApp::mouseButtonCallback(window, button, action, mods);
    }
    
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
        PicasaAppWithUI* app = static_cast<PicasaAppWithUI*>(g_appInstance);
        if (!app) return;
        
        app->m_uiManager->handleMouseMove(xpos, ypos);
        
        PicasaApp::cursorPosCallback(window, xpos, ypos);
    }
    
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
        PicasaAppWithUI* app = static_cast<PicasaAppWithUI*>(g_appInstance);
        if (!app) return;
        
        app->m_uiManager->setWindowSize(width, height);

        PicasaApp::framebufferSizeCallback(window, width, height);
    }
    
private:
    UIManager* m_uiManager;
    Label* m_infoLabel;
    Button* m_prevButton;
    Button* m_nextButton;
    Button* m_rotateButton;
    Button* m_zoomInButton;
    Button* m_zoomOutButton;
    Button* m_toggleViewButton;
    
    void setupUI() {
        m_infoLabel = new Label(10, 10, "No image loaded");
        m_uiManager->addElement(m_infoLabel);
        
        m_prevButton = new Button(10, m_height - 50, 80, 40, "Previous");
        m_prevButton->setCallback([this]() { previousImage(); });
        m_uiManager->addElement(m_prevButton);
        
        m_nextButton = new Button(100, m_height - 50, 80, 40, "Next");
        m_nextButton->setCallback([this]() { nextImage(); });
        m_uiManager->addElement(m_nextButton);
        
        m_rotateButton = new Button(190, m_height - 50, 80, 40, "Rotate");
        m_rotateButton->setCallback([this]() { m_rotation += 90.0f; });
        m_uiManager->addElement(m_rotateButton);
        
        m_zoomInButton = new Button(280, m_height - 50, 80, 40, "Zoom In");
        m_zoomInButton->setCallback([this]() { 
            m_scale *= 1.1f; 
            updateViewTransform();
        });
        m_uiManager->addElement(m_zoomInButton);
        
        m_zoomOutButton = new Button(370, m_height - 50, 80, 40, "Zoom Out");
        m_zoomOutButton->setCallback([this]() { 
            m_scale /= 1.1f; 
            updateViewTransform();
        });
        m_uiManager->addElement(m_zoomOutButton);
        
        m_toggleViewButton = new Button(460, m_height - 50, 120, 40, "Toggle View");
        m_toggleViewButton->setCallback([this]() { 
            m_showThumbnails = !m_showThumbnails; 
        });
        m_uiManager->addElement(m_toggleViewButton);
    }
    
    void updateInfoLabel() {
        if (m_currentTexture) {
            std::string info = "Image: ";
            
            size_t lastSlash = m_current_image_path.find_last_of("/\\");
            std::string filename = (lastSlash != std::string::npos) ? 
                m_current_image_path.substr(lastSlash + 1) : m_current_image_path;
                
            info += filename + " | ";
            info += std::to_string(m_currentTexture->getWidth()) + "x" + 
                    std::to_string(m_currentTexture->getHeight()) + " | ";
            info += "Zoom: " + std::to_string(static_cast<int>(m_scale * 100)) + "% | ";
            info += "Rotation: " + std::to_string(static_cast<int>(m_rotation)) + "°";
            
            m_infoLabel->setText(info);
        } else {
            m_infoLabel->setText("No image loaded");
        }
    }
};

int main(int argc, char* argv[]) 
{
    PicasaAppWithUI app;
    
    if (!app.initialize(1024, 768, "OpenGL Picasa Demo")) {
        std::cerr << "Failed to initialize application" << std::endl;
        return -1;
    }
    
    if (argc > 1) 
    {
        std::string path = argv[1];
        
        fs::path fs_path(path);
        if (fs::is_directory(fs_path)) 
        {
            app.loadFolder(path);
        } 
        else if (fs::is_regular_file(fs_path)) 
        {
            app.loadFolder(fs_path.parent_path().string());
            app.loadImage(path);
        } 
        else 
        {
            std::cerr << "Invalid path: " << path << std::endl;
        }
    }
    
    app.run();
    
    return 0;
}
