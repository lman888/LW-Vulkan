#include "Camera.h"

Camera::Camera()
{
    m_projectionMatrix = glm::mat4(1.0f);
}

void Camera::SetPerspectiveProjection(float fov, float aspect, float near, float far)
{
    assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);

    const float tanHalfFovy = tan(fov / 2.0f);
    m_projectionMatrix = glm::mat4(0.0f);
    m_projectionMatrix[0][0] = 1.0f / (aspect * tanHalfFovy);
    m_projectionMatrix[1][1] = 1.0f / (tanHalfFovy);
    m_projectionMatrix[2][2] = far / (far - near);
    m_projectionMatrix[2][3] = 1.0f;
    m_projectionMatrix[3][2] = -(far * near) / (far - near);
}

const glm::mat4& Camera::GetProjection()
{
    return m_projectionMatrix;
}