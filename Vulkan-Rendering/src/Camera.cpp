#include "Camera.h"

Camera::Camera()
{
    m_camPosition = glm::vec3();
}

glm::vec3& Camera::GetCameraPosition()
{
    return m_camPosition;
}   

void Camera::SetCameraX(float x)
{
    m_camPosition = glm::vec3(m_camPosition.x + x, m_camPosition.y, m_camPosition.z);
}

void Camera::SetCameraZ(float z)
{
    m_camPosition = glm::vec3(m_camPosition.x, m_camPosition.y, m_camPosition.z + z);
}