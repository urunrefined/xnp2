#include "VKDescriptorPoolExt.h"
#include <stdexcept>
#include "Core.h"

namespace BR {

VulkanDescriptorPoolExt::VulkanDescriptorPoolExt(const VkDevice& device_, uint32_t descriptorCount)
	: device(device_)
{
	VkDescriptorPoolSize poolSizes[2];

	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = descriptorCount * 2;

	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = descriptorCount;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = arraySize(poolSizes);
	poolInfo.pPoolSizes = poolSizes;
	poolInfo.maxSets = descriptorCount;
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

VulkanDescriptorPoolExt::~VulkanDescriptorPoolExt(){
	vkDestroyDescriptorPool(device, descriptorPool, 0);
}

}
