#ifndef VKBUFFERS_H
#define VKBUFFERS_H

#include <vulkan/vulkan.h>

namespace BR {

class VulkanBufferGeneric {
	const VkDevice& device;

	VkBuffer buffer;
	VkDeviceMemory bufferMemory;

public:
	VkDeviceSize size;

	VulkanBufferGeneric(const VkDevice& device_, const VkPhysicalDevice& physicalDevice, VkDeviceSize size_, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
	VulkanBufferGeneric(const VulkanBufferGeneric&) = delete;

	virtual ~VulkanBufferGeneric();

	operator VkBuffer&(){
		return buffer;
	}

	operator VkDeviceMemory&(){
		return bufferMemory;
	}
};

class VulkanGBufferData {
	const VkDevice& device;

public:
	VulkanBufferGeneric bufferCard;
	VulkanBufferGeneric stagingBuffer;

	VulkanGBufferData(const VkDevice& device_, const VkPhysicalDevice& physicalDevice, size_t size);
	VulkanGBufferData(const VulkanGBufferData& that) = delete;

	virtual ~VulkanGBufferData(){}

	virtual void update(const char *data, size_t begin, size_t updateSize);

	operator VkBuffer&(){
		return bufferCard;
	}

};

}

#endif // VKBUFFERS_H
