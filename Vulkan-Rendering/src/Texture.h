#pragma once
#include "VulkanIncludes.h"

class Texture
{
public:
	Texture();
	~Texture() = default;

	void CreateTextureImage(const std::string texture);

	void CreateTextureImageView();

	void CreateTextureSampler();

	VkImageView CreateImageView(VkImage& image, VkFormat format, VkImageAspectFlags flags, uint32_t mipLevels);

	void CreateDepthResource();

	void CleanUp();

private:

	void CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

	void TransitionImageLayout(VkImage& image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

	void CopyBufferToImage(VkBuffer& buffer, VkImage& image, uint32_t width, uint32_t height);

	void GenerateMipmaps(VkImage& image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

	VkImage m_depthImage;
	VkDeviceMemory m_depthImageMemory;
	VkImageView m_depthImageView;
	uint32_t m_mipLevels;
	VkImage m_textureImage;
	VkDeviceMemory m_textureImageMemory;
	VkImageView m_textureImageView;
	VkSampler m_textureSampler;
};