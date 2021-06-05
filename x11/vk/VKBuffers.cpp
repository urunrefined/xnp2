#include "VKBuffers.h"
#include "VKUtil.h"
#include <string.h>

#include <stdexcept>

namespace BR {

VulkanBufferGeneric::VulkanBufferGeneric(const VkDevice &device_, const VkPhysicalDevice &physicalDevice, VkDeviceSize size_, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) : device(device_), size(size_){
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		vkDestroyBuffer(device, buffer, nullptr);
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

VulkanBufferGeneric::~VulkanBufferGeneric(){
	vkFreeMemory(device, bufferMemory, nullptr);
	vkDestroyBuffer(device, buffer, nullptr);
}

VulkanGBufferData::VulkanGBufferData(const VkDevice &device_, const VkPhysicalDevice &physicalDevice, size_t size) :
	device(device_),
	bufferCard   (device, physicalDevice, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
	stagingBuffer(device, physicalDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
{}

void VulkanGBufferData::update(const char *data, size_t begin, size_t updateSize) {
	void* dest;

	vkMapMemory(device, stagingBuffer, begin, updateSize, 0, &dest);
	memcpy(dest, data, updateSize);
	vkUnmapMemory(device, stagingBuffer);
}

}
