#pragma once

#include <glm/ext.hpp>

#include "core/types.hpp"

namespace gfx
{

class Camera
{

public:
    Camera();
    ~Camera() = default;

    void update();


public:
    void setPosition(const glm::vec3 &pos) { m_pos = pos; }
    void setFront(const glm::vec3 &front) { m_front = front; }
    void setUp(const glm::vec3 &up) { m_up = up; }
    void setRight(const glm::vec3 &right) { m_right = right; }

    void setFov(f32 fov) { m_fov = fov; }
    void setAspect(f32 aspect) { m_aspect = aspect; }
    void setNearPlane(f32 value) { m_near = value; }
    void setFarPlane(f32 value) { m_far = value; }

public:
    const glm::vec3 &getPosition() const { return m_pos; }
    const glm::vec3 &getFront() const { return m_front; }
    const glm::vec3 &getUp() const { return m_up; }
    const glm::vec3 &getRight() const { return m_right; }

    const glm::mat4 &getView() const { return m_view; }

    f32 getFov() const { return m_fov; }
    f32 getAspect() const { return m_aspect; }
    f32 getNear() const { return m_near; }
    f32 getFar() const { return m_far; }

    const glm::mat4 &getProjection() const { return m_proj; }

private:
    glm::vec3 m_pos;

    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;

    glm::mat4 m_view;

    f32 m_fov;
    f32 m_aspect;
    f32 m_near;
    f32 m_far;

    glm::mat4 m_proj;
};

} // namespace gfx