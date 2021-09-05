#ifndef VKDESCRIPTORSETEXT_H
#define VKDESCRIPTORSETEXT_H

#include <vulkan/vulkan.h>

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
	const VkDescriptorPool& descriptorPoolExt;

	VkDescriptorSet descriptorSet;

public:
	VulkanDescriptorSetExt(const VkDevice& device_, VkImageView& imageView,
		const VkDescriptorPool& descriptorPoolExt_,
		const VkDescriptorSetLayout &layout);

	~VulkanDescriptorSetExt();

	operator const VkDescriptorSet& () const{
		return descriptorSet;
	}
};

}

#endif // VKDESCRIPTORSETEXT_H
