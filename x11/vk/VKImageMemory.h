#ifndef VKIMAGEMEMORY_H
#define VKIMAGEMEMORY_H

#include <vulkan/vulkan.h>
#include "VKUtil.h"

#include <stdexcept>

namespace BR {

class VulkanImageMemory
{
	const VkDevice& device;
	VkDeviceMemory imageMemory;

public:
	VulkanImageMemory(const VkPhysicalDevice& physicalDevice, const VkDevice& device_, VkImage image, VkMemoryPropertyFlags properties);

	~VulkanImageMemory();

	operator VkDeviceMemory& (){
		return imageMemory;
	}
};

}

#endif // VKIMAGEMEMORY_H
