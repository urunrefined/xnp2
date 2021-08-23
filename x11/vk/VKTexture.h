#ifndef BR_VULKANTEXTURE_H
#define BR_VULKANTEXTURE_H

#include "VKImage.h"
#include "VKImageMemory.h"
#include "VKCommandPool.h"
#include "VKImageView.h"
#include "VKDescriptorSet.h"
#include "VKSampler.h"


namespace BR {

static const unsigned int pc98Width = 640;
static const unsigned int pc98Height = 400;

class Image {
public:
	unsigned char data[pc98Width * pc98Height * 4] = {0};
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

	VulkanTexture(const VkDevice& device_, const VkPhysicalDevice& physicalDevice_, const VkQueue& graphicsQueue_, int graphicsFamily, VulkanDescriptorLayout& descriptorLayout, VulkanSampler& sampler) :
		device(device_),
		physicalDevice(physicalDevice_),
		graphicsQueue(graphicsQueue_),
		texture(device, 640, 400, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT),
		textureMemory(physicalDevice, device, texture, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),

		commandPool(device, graphicsFamily),
		textureView(device, texture, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT),
		descriptorPool(device),
		textureDirty(true),
		descriptorSet(device, textureView, sampler, descriptorPool, descriptorLayout)
	{
	}

	void update();


};

} // namespace BR

#endif // BR_VULKANTEXTURE_H
