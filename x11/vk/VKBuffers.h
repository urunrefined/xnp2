#ifndef VKBUFFERS_H
#define VKBUFFERS_H

#include <vulkan/vulkan.h>
#include "Range.h"

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

class VulkanCmbBuffer {
	const VkDevice& device;

public:
	VulkanBufferGeneric bufferCard;
	VulkanBufferGeneric stagingBuffer;

	Ranges ranges;

	VulkanCmbBuffer(const VkDevice& device_, const VkPhysicalDevice& physicalDevice, VkBufferUsageFlags usage, size_t size);
	VulkanCmbBuffer(const VulkanCmbBuffer& that) = delete;

	virtual ~VulkanCmbBuffer(){}

	virtual void update(const char *data, size_t begin, size_t updateSize);

	operator VkBuffer&(){
		return bufferCard;
	}

};

class VulkanVtxBuffer : public VulkanCmbBuffer {
public:
	VulkanVtxBuffer(const VkDevice& device_, const VkPhysicalDevice& physicalDevice, size_t size);
	VulkanVtxBuffer(const VulkanVtxBuffer& that) = delete;
};

class VulkanUniformBuffer : public VulkanCmbBuffer{
public:
	VulkanUniformBuffer(const VkDevice& device_, const VkPhysicalDevice& physicalDevice, size_t size);
	VulkanUniformBuffer(const VulkanVtxBuffer& that) = delete;
};

}

#endif // VKBUFFERS_H
