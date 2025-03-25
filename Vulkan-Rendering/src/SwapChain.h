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

	void CreateSwapChain(GLFWwindow* window);

	void CreateFrameBuffers();

	VkSwapchainKHR GetVulkanSwapChain();

	SwapChainSupportDetails QuerySwapChainSupport();

	void CreateImageViews();

	void ReCreateSwapChain(GLFWwindow* window);
	
	//Clean this Later
	std::vector<VkFramebuffer>& GetSwapChainFrameBuffers();
	std::vector<VkImageView>& GetSwapChainImageViews();
	std::vector<VkImage>& GetSwapChainImages();
	VkFormat& GetSwapChainImageFormat();
	VkExtent2D& GetSwapChainImageExtent();
	
	void CleanUpSwapChain();

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