#include "VKImageMemory.h"

namespace BR {

VulkanImageMemory::VulkanImageMemory(const VkPhysicalDevice &physicalDevice, const VkDevice &device_, VkImage image, VkMemoryPropertyFlags properties) : device(device_){
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	if(vkBindImageMemory(device, image, imageMemory, 0) != VK_SUCCESS){
		vkFreeMemory(device, imageMemory, 0);
		throw std::runtime_error("failed to bind image memory!");
	}
}

VulkanImageMemory::~VulkanImageMemory(){
	vkFreeMemory(device, imageMemory, 0);
}

}
