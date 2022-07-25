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

	std::vector<VkDescriptorSet> descriptorSets;

public:
	VulkanUniformBuffer uniformBuffer;
private:
	size_t modelCount;
public:

	VulkanDescriptorSetExt(
		const VkDevice& device_,
		VulkanPhysicalDevice& physicalDevice,
		VkImageView& imageView,
		VkSampler& sampler,
		const VkDescriptorPool& descriptorPoolExt_,
		const VkDescriptorSetLayout &layout,
		size_t modelCount_);

	~VulkanDescriptorSetExt();

	void updateWorldMatrix(const Matrix4x4f& world);
	void updateModelMatrix(const Matrix4x4f& world, size_t index);

	const VkDescriptorSet& get(size_t index) const{
		return descriptorSets[index];
	}
};

}

#endif // VKDESCRIPTORSETEXT_H
