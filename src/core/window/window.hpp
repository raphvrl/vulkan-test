#pragma once

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#elif __linux__
#define GLFW_EXPOSE_NATIVE_X11
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <glm/ext.hpp>

#include <string>
#include <stdexcept>
#include <array>

#include "core/types.hpp"

namespace core
{

class Window
{

public:
    Window() = default;
    virtual ~Window() = default;
    
    void init(u32 width, u32 height, const std::string &title);
    void destroy();

    void pollEvents();

public:

    bool isOpen() const { return !glfwWindowShouldClose(m_window); }
    void close() { glfwSetWindowShouldClose(m_window, GLFW_TRUE); }

public:

    bool isKeyPressed(int key) const { return m_keys[key]; }
    bool isKeyJustPressed(int key) const {
        return m_keys[key] && !m_prevKeys[key];
    }

    bool isMouseButtonPressed(int button) const {
        return m_mouseButtonStates[button];
    }

public:

    glm::vec2 getMousePos() const { return m_mousePos; }
    glm::vec2 getMouseRel() const { return m_mouseRel; }
    
    GLFWwindow *get() const { return m_window; }
    void *getNativeWindow() const { return m_nativeWindow; }

    u32 getWidth() const { return m_width; }
    u32 getHeight() const { return m_height; }

    f32 getAspect() const {
        return static_cast<f32>(m_width) / static_cast<f32>(m_height);
    }

private:
    GLFWwindow *m_window = nullptr;
    void *m_nativeWindow = nullptr;

    u32 m_width;
    u32 m_height;
    std::string m_title;

    std::array<bool, GLFW_KEY_LAST + 1> m_keys;
    std::array<bool, GLFW_KEY_LAST + 1> m_prevKeys;

    std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> m_mouseButtonStates;

    bool m_firstMouse = true;
    glm::vec2 m_mousePos;
    glm::vec2 m_mouseRel;

private:
    static void framebufferSizeCallback(
        GLFWwindow *window,
        int width,
        int height
    );

    static void keyCallback(
        GLFWwindow *window,
        int key,
        int scancode,
        int action,
        int mods
    );

    static void mouseButtonCallback(
        GLFWwindow *window,
        int button,
        int action,
        int mods
    );

    static void cursorPosCallback(
        GLFWwindow *window,
        double xpos,
        double ypos
    );
};

} // namespace core