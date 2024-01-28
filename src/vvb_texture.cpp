#include "vvb_texture.hpp"

// std
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// vulkan base
#include "vvb_buffer.hpp"


VvbTexture::VvbTexture(VvbDevice& device, const std::string& textureFilePath)
	: device(device)
{
	createTextureImage(textureFilePath);
	textureImageView = device.createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);
	createTextureSampler();
}

VvbTexture::VvbTexture(VvbDevice& device)
	: device(device)
{
	createTextureImage();
	textureImageView = device.createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);
	createTextureSampler();
}

VvbTexture::~VvbTexture()
{
	vkDestroyImage(device.getDevice(), textureImage, nullptr);
	vkFreeMemory(device.getDevice(), textureImageMemory, nullptr);

	vkDestroyImageView(device.getDevice(), textureImageView, nullptr);

	vkDestroySampler(device.getDevice(), textureSampler, nullptr);
}

void VvbTexture::createTextureImage(const std::string& textureFilePath)
{
	int texWidth, texHeight, texChannels;

	stbi_uc* pixels = stbi_load(textureFilePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

	VkDeviceSize imageSize = texWidth * texHeight;

	std::cout << texWidth << " " << texHeight;

	if (!pixels)
		throw std::runtime_error("failed to load texture image : " + textureFilePath);

	VvbBuffer stvvbingBuffer{
		device,
		imageSize,
		4,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	};

	stvvbingBuffer.map();
	stvvbingBuffer.write(pixels);
	stvvbingBuffer.unmap();

	stbi_image_free(pixels);

	device.createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory, mipLevels, VK_SAMPLE_COUNT_1_BIT);

	device.transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);

	device.copyBufferToImage(stvvbingBuffer.getBuffer(), textureImage, texWidth, texHeight);

	//device.transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels);

	device.generateMipmaps(textureImage, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, mipLevels);
}

void VvbTexture::createTextureImage()
{
	VvbBuffer stvvbingBuffer{
		device,
		1,
		4,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	};

	unsigned long textureColor = 0x00FFFFFF;
	mipLevels = 1;

	stvvbingBuffer.map();
	stvvbingBuffer.write(&textureColor);
	stvvbingBuffer.unmap();

	device.createImage(1, 1, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory, mipLevels, VK_SAMPLE_COUNT_1_BIT);

	device.transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);

	device.copyBufferToImage(stvvbingBuffer.getBuffer(), textureImage, 1, 1);

	//device.transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels);

	device.generateMipmaps(textureImage, VK_FORMAT_R8G8B8A8_SRGB, 1, 1, mipLevels);
}

void VvbTexture::createTextureSampler()
{
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.pNext = VK_NULL_HANDLE; // optional
	samplerInfo.flags = 0; // optional
	samplerInfo.magFilter = VK_FILTER_NEAREST;
	samplerInfo.minFilter = VK_FILTER_NEAREST;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = device.getPhysicalDeviceProperties().limits.maxSamplerAnisotropy;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.minLod = 0.0f; // static_cast<float>(mipLevels / 2);
	samplerInfo.maxLod = static_cast<float>(mipLevels);
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	if (vkCreateSampler(device.getDevice(), &samplerInfo, VK_NULL_HANDLE, &textureSampler) != VK_SUCCESS)
		throw std::runtime_error("failed to create texture sampler!");
}
