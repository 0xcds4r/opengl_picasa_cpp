///////////////////////////////////////////////////////////////////////////////
//                                                                          //
// FILE :    ui.h                                                          //
// AUTHOR :  0xcds4r                                                      //
// CREATED : 25/03/2025                                                  //
//                                                                      //
/////////////////////////////////////////////////////////////////////////

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <functional>

class UIElement {
public:
    UIElement(float x, float y, float width, float height);
    virtual ~UIElement() = default;
    
    bool isMouseOver(float mouseX, float mouseY) const;
    virtual void render() = 0;
    virtual void onMouseClick() {}
    
    void setPosition(float x, float y);
    void setSize(float width, float height);
    void setVisible(bool visible);
    bool isVisible() const;
    
protected:
    float m_x, m_y;
    float m_width, m_height;
    bool m_visible;
};

class Button : public UIElement {
public:
    Button(float x, float y, float width, float height, const std::string& text);
    
    void render() override;
    void onMouseClick() override;
    
    void setText(const std::string& text);
    void setCallback(std::function<void()> callback);
    
private:
    std::string m_text;
    std::function<void()> m_callback;
};

class Label : public UIElement {
public:
    Label(float x, float y, const std::string& text);
    
    void render() override;
    void setText(const std::string& text);
    
private:
    std::string m_text;
};

class UIManager {
public:
    UIManager();
    ~UIManager();
    
    void initialize(int windowWidth, int windowHeight);
    void render();
    
    void handleMouseMove(float mouseX, float mouseY);
    bool handleMouseClick(float mouseX, float mouseY);
    
    void addElement(UIElement* element);
    void removeElement(UIElement* element);
    
    void setWindowSize(int width, int height);
    
private:
    std::vector<UIElement*> m_elements;
    int m_windowWidth;
    int m_windowHeight;
    UIElement* m_hoveredElement;
    
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_textVAO;
    GLuint m_textVBO;
};
