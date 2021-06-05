#ifndef VKDEVICE_H
#define VKDEVICE_H

#include <vulkan/vulkan.h>
#include "Array.h"

#include <vector>

namespace BR {

class VulkanDevice {
public:
	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentQueue;

	uint32_t graphicsFamily;
	uint32_t presentFamily;

	VulkanDevice(bool enableValidationLayer, const VkPhysicalDevice& physicalDevice, VkSurfaceKHR& surface, const std::vector<VkQueueFamilyProperties>& queueFamilies);
	~VulkanDevice();

	operator VkDevice&(){
		return device;
	}

	operator const VkDevice&() const {
		return device;
	}
};

}

#endif // VKDEVICE_H
