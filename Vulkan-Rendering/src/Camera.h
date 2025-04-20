#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERTO_TO_ONE
#include <ostream>
#include <iostream>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include "VulkanCore.h"

class Camera
{
public:
    Camera();
    ~Camera() = default;

    glm::vec3& GetCameraPosition();

    void SetCameraX(float x);
    void SetCameraZ(float z);
    
private:
    glm::vec3 m_camPosition;
};

inline void KeyInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        std::cout << "W was pressed!" << std::endl;
        VulkanCore::GetCamera()->SetCameraX(100);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        std::cout << "A was pressed!" << std::endl;
        VulkanCore::GetCamera()->SetCameraZ(100);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        std::cout << "S was pressed!" << std::endl;
        VulkanCore::GetCamera()->SetCameraX(-100);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        std::cout << "D was pressed!" << std::endl;
        VulkanCore::GetCamera()->SetCameraZ(-100);
    }
}