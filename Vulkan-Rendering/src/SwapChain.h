#pragma once

#include "VulkanIncludes.h"

#include <vector>

class VulkanCore;

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector <VkPresentModeKHR> presentModes;
};

class SwapChain
{
public:
	SwapChain();
	~SwapChain();

	void CreateSwapChain(GLFWwindow* window, VulkanCore* vulkanCore);

	void CreateFrameBuffers(VulkanCore* vulkanCore);

	VkSwapchainKHR GetSwapChain();

	SwapChainSupportDetails QuerySwapChainSupport(VulkanCore* vulkanCore);

	void CreateImageViews(VulkanCore* vulkanCore);

	void ReCreateSwapChain(GLFWwindow* window, VulkanCore* vulkanCore);
	
	//Clean this Later
	std::vector<VkFramebuffer>& GetSwapChainFrameBuffers();
	std::vector<VkImageView>& GetSwapChainImageViews();
	std::vector<VkImage>& GetSwapChainImages();
	VkFormat& GetSwapChainImageFormat();
	VkExtent2D& GetSwapChainImageExtent();
	
	void CleanUpSwapChain(VulkanCore* vulkanCore);

private:
	
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);

	VkSwapchainKHR swapChain;
	std::vector<VkFramebuffer> swapChainFrameBuffers;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
};