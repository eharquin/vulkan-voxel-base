#pragma once

// vulkan base
#include "vvb_device.hpp"

class VvbTexture
{
public:
	VvbTexture(VvbDevice& device, const std::string& textureFilePath);
	VvbTexture(VvbDevice& device);
	~VvbTexture();

	void createTextureImage(const std::string& textureFilePath);
	void createTextureImage();
	void createTextureSampler();

	// test
	VkImageView getTextureImageView() const { return textureImageView; }
	VkSampler getTextureSampler() const { return textureSampler; }

private:
	VvbDevice& device;
	VkImage textureImage;
	uint32_t mipLevels;
	VkDeviceMemory textureImageMemory;

	VkImageView textureImageView;
	VkSampler textureSampler;
};