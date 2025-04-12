#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERTO_TO_ONE
#include <glm/glm.hpp>

class Camera
{
public:
    Camera();
    ~Camera() = default;

    void SetPerspectiveProjection(float fov, float aspect, float near, float far);

    const glm::mat4& GetProjection();
    
private:
    glm::mat4 m_projectionMatrix;
};
