#include "window.hpp"

namespace core
{

void Window::init(u32 width, u32 height, const std::string &title)
{
    m_width = width;
    m_height = height;
    m_title = title;

    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_window = glfwCreateWindow(
        m_width,
        m_height,
        m_title.c_str(),
        nullptr,
        nullptr
    );

    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

#ifdef _WIN32
    m_nativeWindow = glfwGetWin32Window(m_window);
#elif __linux__
    m_nativeWindow = glfwGetX11Window(m_window);
#endif

    m_keys.fill(false);
    m_prevKeys.fill(false);

    m_mouseButtonStates.fill(false);

    glfwSetWindowUserPointer(m_window, this);

    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);
    glfwSetKeyCallback(m_window, keyCallback);
    glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
    glfwSetCursorPosCallback(m_window, cursorPosCallback);
}

void Window::destroy()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Window::pollEvents()
{
    m_prevKeys = m_keys;
    m_mouseRel = {0.0f, 0.0f};

    glfwPollEvents();
}

void Window::framebufferSizeCallback(
    GLFWwindow *window,
    int width,
    int height
)
{
    Window *self = static_cast<Window *>(glfwGetWindowUserPointer(window));
    self->m_width = width;
    self->m_height = height;
}

void Window::keyCallback(
    GLFWwindow *window,
    int key,
    int scancode,
    int action,
    int mods
)
{
    UNUSED(scancode);
    UNUSED(mods);

    Window *self = static_cast<Window *>(glfwGetWindowUserPointer(window));
    
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        self->m_keys[key] = true;
    } else if (action == GLFW_RELEASE) {
        self->m_keys[key] = false;
    }
}

void Window::mouseButtonCallback(
    GLFWwindow *window,
    int button,
    int action,
    int mods
)
{
    UNUSED(mods);

    Window *self = static_cast<Window *>(glfwGetWindowUserPointer(window));
    
    if (action == GLFW_PRESS) {
        self->m_mouseButtonStates[button] = true;
    } else if (action == GLFW_RELEASE) {
        self->m_mouseButtonStates[button] = false;
    }
}

void Window::cursorPosCallback(
    GLFWwindow *window,
    double xpos,
    double ypos
)
{
    Window *self = static_cast<Window *>(glfwGetWindowUserPointer(window));
    
    if (self->m_firstMouse) {
        self->m_mousePos.x = xpos;
        self->m_mousePos.y = ypos;
        self->m_firstMouse = false;
    }

    self->m_mouseRel.x = xpos - self->m_mousePos.x;
    self->m_mouseRel.y = ypos - self->m_mousePos.y;
    self->m_mousePos.x = xpos;
    self->m_mousePos.y = ypos;
}

} // namespace core