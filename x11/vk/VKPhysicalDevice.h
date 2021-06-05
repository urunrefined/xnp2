#ifndef VKPHYSICALDEVICE_H
#define VKPHYSICALDEVICE_H

#include <vulkan/vulkan.h>

#include "Array.h"
#include <vector>

namespace BR {

class VulkanPhysicalDevice {
public:
	VkPhysicalDevice physicalDevice;
	VkPhysicalDeviceProperties deviceProperties;
	std::vector<VkQueueFamilyProperties> queueFamilies;

	VulkanPhysicalDevice(VkPhysicalDevice physicalDevice_);
	~VulkanPhysicalDevice() = default;

	bool isDeviceSuitable(VkSurfaceKHR& surface);

	VkDeviceSize getMinUniformBufferOffsetAlignment() const {
		return deviceProperties.limits.minUniformBufferOffsetAlignment;
	}

	operator const VkPhysicalDevice& () const {
		return physicalDevice;
	}
};

}

#endif // VKPHYSICALDEVICE_H
