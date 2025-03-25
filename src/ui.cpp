///////////////////////////////////////////////////////////////////////////////
//                                                                          //
// FILE :    ui.cpp                                                        //
// AUTHOR :  0xcds4r                                                      //
// CREATED : 25/03/2025                                                  //
//                                                                      //
/////////////////////////////////////////////////////////////////////////

#include "ui.h"
#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

UIElement::UIElement(float x, float y, float width, float height)
    : m_x(x), m_y(y), m_width(width), m_height(height), m_visible(true)
{}

bool UIElement::isMouseOver(float mouseX, float mouseY) const
{
    if (!m_visible) return false;
    
    return (mouseX >= m_x && mouseX <= m_x + m_width &&
            mouseY >= m_y && mouseY <= m_y + m_height);
}

void UIElement::setPosition(float x, float y)
{
    m_x = x;
    m_y = y;
}

void UIElement::setSize(float width, float height)
{
    m_width = width;
    m_height = height;
}

void UIElement::setVisible(bool visible)
{
    m_visible = visible;
}

bool UIElement::isVisible() const
{
    return m_visible;
}

Button::Button(float x, float y, float width, float height, const std::string& text)
    : UIElement(x, y, width, height), m_text(text), m_callback(nullptr)
{
}

void Button::render()
{
    if (!m_visible) return;
    
    glBegin(GL_QUADS);
    glColor3f(0.3f, 0.3f, 0.3f);
    glVertex2f(m_x, m_y);
    glVertex2f(m_x + m_width, m_y);
    glVertex2f(m_x + m_width, m_y + m_height);
    glVertex2f(m_x, m_y + m_height);
    glEnd();
    
    glBegin(GL_LINE_LOOP);
    glColor3f(0.7f, 0.7f, 0.7f);
    glVertex2f(m_x, m_y);
    glVertex2f(m_x + m_width, m_y);
    glVertex2f(m_x + m_width, m_y + m_height);
    glVertex2f(m_x, m_y + m_height);
    glEnd();
    
    glColor3f(1.0f, 1.0f, 1.0f);
    // TODO
}

void Button::onMouseClick()
{
    if (m_callback) {
        m_callback();
    }
}

void Button::setText(const std::string& text)
{
    m_text = text;
}

void Button::setCallback(std::function<void()> callback)
{
    m_callback = callback;
}

Label::Label(float x, float y, const std::string& text)
    : UIElement(x, y, 0, 0), m_text(text)
{
    m_width = text.length() * 8.0f;  
    m_height = 20.0f;
}

void Label::render()
{
    if (!m_visible) return;
    
    glColor3f(1.0f, 1.0f, 1.0f);
    // TODO
}

void Label::setText(const std::string& text)
{
    m_text = text;
    m_width = text.length() * 8.0f;
}

UIManager::UIManager()
    : m_windowWidth(0), m_windowHeight(0), m_hoveredElement(nullptr),
      m_vao(0), m_vbo(0), m_textVAO(0), m_textVBO(0)
{}

UIManager::~UIManager()
{
    if (m_vao != 0) {
        glDeleteVertexArrays(1, &m_vao);
    }
    if (m_vbo != 0) {
        glDeleteBuffers(1, &m_vbo);
    }
    if (m_textVAO != 0) {
        glDeleteVertexArrays(1, &m_textVAO);
    }
    if (m_textVBO != 0) {
        glDeleteBuffers(1, &m_textVBO);
    }
    
    for (auto element : m_elements) {
        delete element;
    }

    m_elements.clear();
}

void UIManager::initialize(int windowWidth, int windowHeight)
{
    m_windowWidth = windowWidth;
    m_windowHeight = windowHeight;
    
    // TODO
    
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    
    glGenVertexArrays(1, &m_textVAO);
    glGenBuffers(1, &m_textVBO);
}

void UIManager::render()
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, m_windowWidth, m_windowHeight, 0, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glDisable(GL_DEPTH_TEST);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    for (auto element : m_elements) {
        if (element->isVisible()) {
            element->render();
        }
    }
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    
    glPopAttrib();
}

void UIManager::handleMouseMove(float mouseX, float mouseY)
{
    m_hoveredElement = nullptr;
    for (auto it = m_elements.rbegin(); it != m_elements.rend(); ++it) {
        if ((*it)->isMouseOver(mouseX, mouseY)) {
            m_hoveredElement = *it;
            break;
        }
    }
}

bool UIManager::handleMouseClick(float mouseX, float mouseY)
{
    for (auto it = m_elements.rbegin(); it != m_elements.rend(); ++it) {
        if ((*it)->isMouseOver(mouseX, mouseY)) {
            (*it)->onMouseClick();
            return true;
        }
    }
    
    return false;
}

void UIManager::addElement(UIElement* element)
{
    m_elements.push_back(element);
}

void UIManager::removeElement(UIElement* element)
{
    auto it = std::find(m_elements.begin(), m_elements.end(), element);
    if (it != m_elements.end()) {
        m_elements.erase(it);
    }
}

void UIManager::setWindowSize(int width, int height)
{
    m_windowWidth = width;
    m_windowHeight = height;
}
