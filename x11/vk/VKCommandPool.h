#ifndef VKCOMMANDPOOL_H
#define VKCOMMANDPOOL_H

#include <vulkan/vulkan.h>

namespace BR {

class VulkanCommandPool {
	const VkDevice& device;

public:
	VkCommandPool commandPool;

	VulkanCommandPool(const VkDevice& device_, uint32_t graphicsFamily);
	VulkanCommandPool & operator=(const VulkanCommandPool&) = delete;
	VulkanCommandPool(const VulkanCommandPool&) = delete;

	~VulkanCommandPool();

	operator VkCommandPool& (){
		return commandPool;
	}
};

}

#endif // VKCOMMANDPOOL_H
