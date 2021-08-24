#ifndef BR_VULKANTEXTURE_H
#define BR_VULKANTEXTURE_H

#include "VKImage.h"
#include "VKImageMemory.h"
#include "VKCommandPool.h"
#include "VKImageView.h"
#include "VKDescriptorSet.h"
#include "VKSampler.h"

#include <vector>

namespace BR {

class Image {
public:
	const uint16_t width;
	const uint16_t height;

	std::vector<unsigned char> data;

	Image(uint16_t width_, uint16_t height_) : width(width_), height(height_){
		data.resize(width * height * 4);
	}
};

class VulkanTexture
{
	VkDevice device;
	VkPhysicalDevice physicalDevice;
	VkQueue graphicsQueue;
	VulkanImage texture;
	VulkanImageMemory textureMemory;
	VulkanCommandPool commandPool;
	VulkanImageView textureView;

	VulkanDescriptorPool descriptorPool;

public:
	bool textureDirty;
	Image image;
	VulkanDescriptorSet descriptorSet;

	VulkanTexture(const VkDevice& device_, const VkPhysicalDevice& physicalDevice_, const VkQueue& graphicsQueue_, int graphicsFamily, VulkanDescriptorLayout& descriptorLayout, VulkanSampler& sampler,
			uint16_t width, uint16_t height) :
		device(device_),
		physicalDevice(physicalDevice_),
		graphicsQueue(graphicsQueue_),
		texture(device, width, height, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT),
		textureMemory(physicalDevice, device, texture, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),

		commandPool(device, graphicsFamily),
		textureView(device, texture, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT),
		descriptorPool(device),
		textureDirty(true),
		image(width, height),
		descriptorSet(device, textureView, sampler, descriptorPool, descriptorLayout)
	{
	}

	void update();


};

} // namespace BR

#endif // BR_VULKANTEXTURE_H
