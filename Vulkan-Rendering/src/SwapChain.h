#pragma once

#include "VulkanIncludes.h"

#include <vector>

class VulkanSurface;
class DeviceQuery;
class RenderPass;

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

	void CreateSwapChain(DeviceQuery& deviceQuery, VulkanSurface& surface, GLFWwindow* window);

	void CreateFrameBuffers(DeviceQuery& deviceQuery, RenderPass& renderPass);

	VkSwapchainKHR GetSwapChain();

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice& device, VkSurfaceKHR& surface);

	void CreateImageViews(DeviceQuery& deviceQuery);

	void ReCreateSwapChain(DeviceQuery& device, VulkanSurface& surface, RenderPass& renderPass, GLFWwindow* window);
	
	//Clean this Later
	std::vector<VkFramebuffer>& GetSwapChainFrameBuffers();
	std::vector<VkImageView>& GetSwapChainImageViews();
	std::vector<VkImage>& GetSwapChainImages();
	VkFormat& GetSwapChainImageFormat();
	VkExtent2D& GetSwapChainImageExtent();
	
	void CleanUpSwapChain(DeviceQuery device);

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