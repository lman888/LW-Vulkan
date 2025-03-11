#pragma once

#include "VulkanIncludes.h"

#include <vector>

class VulkanSurface;
class DeviceQuery;

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

	void CreateSwapChain(DeviceQuery deviceQuery, VulkanSurface surface, GLFWwindow* window);

	VkSwapchainKHR GetSwapChain() const;

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

	void CreateImageViews(DeviceQuery deviceQuery);

	//Clean this Later
	std::vector<VkFramebuffer>& GetSwapChainFrameBuffers();
	std::vector<VkImageView> GetSwapChainImageViews();
	std::vector<VkImage> GetSwapChainImages();
	VkFormat GetSwapChainImageFormat();
	VkExtent2D GetSwapChainImageExtent();
	

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