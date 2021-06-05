#ifndef VKPHYSICALDEVICEENUMERATIONS_H
#define VKPHYSICALDEVICEENUMERATIONS_H

#include "VKPhysicalDevice.h"

namespace BR {

class VulkanPhysicalDeviceEnumerations
{
public:
	std::vector<VulkanPhysicalDevice> physicalDevices;

	VulkanPhysicalDeviceEnumerations(VkInstance& instance);
};

}

#endif // VKPHYSICALDEVICEENUMERATIONS_H
