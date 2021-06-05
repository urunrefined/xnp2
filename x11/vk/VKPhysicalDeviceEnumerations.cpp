#include "VKPhysicalDeviceEnumerations.h"

namespace BR {

VulkanPhysicalDeviceEnumerations::VulkanPhysicalDeviceEnumerations(VkInstance& instance)
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		return;
	}

	std::vector<VkPhysicalDevice> physicalDevicesC(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevicesC.data());

	for (const auto& physicalDevice : physicalDevicesC) {
		physicalDevices.push_back(physicalDevice);
	}
}

}
