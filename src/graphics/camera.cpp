#include "camera.hpp"

namespace gfx
{

Camera::Camera()
{
    m_pos = glm::vec3(0.0f, 0.0f, 3.0f);
    m_front = glm::vec3(0.0f, 0.0f, -1.0f);
    m_up = glm::vec3(0.0f, 1.0f, 0.0f);
    m_right = glm::vec3(1.0f, 0.0f, 0.0f);

    m_worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    m_yaw = -90.0f;
    m_pitch = 0.0f;

    m_fov = 45.0f;
    m_aspect = 16.0f / 9.0f;
    m_near = 0.1f;
    m_far = 100.0f;
}

void Camera::update()
{
    m_front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front.y = sin(glm::radians(m_pitch));
    m_front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

    m_front = glm::normalize(m_front);
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));

    m_view = glm::lookAt(m_pos, m_pos + m_front, m_up);
    m_proj = glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);
    m_proj[1][1] *= -1;
}

} // namespace gfx