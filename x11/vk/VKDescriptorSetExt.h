#ifndef VKDESCRIPTORSETEXT_H
#define VKDESCRIPTORSETEXT_H

#include <vulkan/vulkan.h>

#include "VKPhysicalDevice.h"
#include "VKBuffers.h"
#include "Matrix4x4.h"

namespace BR {

class VulkanDescriptorLayoutExt {
	const VkDevice& device;
	VkDescriptorSetLayout layout;

public:

	VulkanDescriptorLayoutExt(const VkDevice& device_);
	~VulkanDescriptorLayoutExt();;

	operator const VkDescriptorSetLayout& () const{
		return layout;
	}
};

class VulkanDescriptorSetExt
{
	const VkDevice& device;
	VulkanPhysicalDevice& physicalDevice;
	const VkDescriptorPool& descriptorPoolExt;
	VkDescriptorSet descriptorSet;

public:
	VulkanUniformBuffer uniformBuffer;

	VulkanDescriptorSetExt(
		const VkDevice& device_,
		VulkanPhysicalDevice& physicalDevice,
		VkImageView& imageView,
		VkSampler& sampler,
		const VkDescriptorPool& descriptorPoolExt_,
		const VkDescriptorSetLayout &layout);

	~VulkanDescriptorSetExt();

	void updateWorldMatrix(const Matrix4x4f& world);
	void updateModelMatrix(size_t which, const Matrix4x4f& model);

	operator const VkDescriptorSet& () const{
		return descriptorSet;
	}
};

}

#endif // VKDESCRIPTORSETEXT_H
