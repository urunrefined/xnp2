#ifndef VKDESCRIPTORSET_H
#define VKDESCRIPTORSET_H

#include <vulkan/vulkan.h>

namespace BR {

class VulkanDescriptorLayout {
	const VkDevice& device;
	VkDescriptorSetLayout layout;

public:

	VulkanDescriptorLayout(const VkDevice& device_);
	~VulkanDescriptorLayout();;

	operator const VkDescriptorSetLayout& () const{
		return layout;
	}
};

class VulkanDescriptorPool {
	const VkDevice& device;
	VkDescriptorPool descriptorPool;

public:
	VulkanDescriptorPool(const VkDevice& device_);
	~VulkanDescriptorPool();

	operator const VkDescriptorPool& () const{
		return descriptorPool;
	}
};

class VulkanDescriptorSet
{
	const VkDevice& device;
	const VkDescriptorPool& descriptorPool;

	VkDescriptorSet descriptorSet;

public:
	VulkanDescriptorSet(const VkDevice& device_, VkImageView& imageView, VkSampler& sampler,
		const VkDescriptorPool& descriptorPool_, const VkDescriptorSetLayout &layout);

	~VulkanDescriptorSet();

	operator const VkDescriptorSet& () const{
		return descriptorSet;
	}
};

}

#endif // VKDESCRIPTORSET_H
