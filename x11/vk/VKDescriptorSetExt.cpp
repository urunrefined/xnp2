#include "VKDescriptorSetExt.h"

#include <vector>
#include <stdexcept>

#include "string.h"

namespace BR {

VulkanDescriptorLayoutExt::VulkanDescriptorLayoutExt(const VkDevice& device_) : device(device_){
	VkDescriptorSetLayoutBinding samplerLayoutBinding [3]{};
	samplerLayoutBinding[0].binding = 0;
	samplerLayoutBinding[0].descriptorCount = 1;
	samplerLayoutBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	samplerLayoutBinding[0].pImmutableSamplers = nullptr;
	samplerLayoutBinding[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	samplerLayoutBinding[1].binding = 1;
	samplerLayoutBinding[1].descriptorCount = 1;
	samplerLayoutBinding[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	samplerLayoutBinding[1].pImmutableSamplers = nullptr;
	samplerLayoutBinding[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	samplerLayoutBinding[2].binding = 2;
	samplerLayoutBinding[2].descriptorCount = 1;
	samplerLayoutBinding[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding[2].pImmutableSamplers = nullptr;
	samplerLayoutBinding[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 3;
	layoutInfo.pBindings = samplerLayoutBinding;

	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &layout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

VulkanDescriptorLayoutExt::~VulkanDescriptorLayoutExt(){
	vkDestroyDescriptorSetLayout(device, layout, 0);
}

VulkanDescriptorSetExt::VulkanDescriptorSetExt(
	const VkDevice& device_,
	VkImageView& imageView,
	const VkDescriptorPool& descriptorPoolExt_,
	const VkDescriptorSetLayout &layout)
	:
	device(device_),
	descriptorPoolExt(descriptorPoolExt_)
{

}

VulkanDescriptorSetExt::~VulkanDescriptorSetExt(){
	vkFreeDescriptorSets(device, descriptorPoolExt, 1, &descriptorSet);
}

}
