#include "VKPhysicalDevice.h"

#include <vector>
#include <stdexcept>
#include <string>
#include <set>

#include "VKUtil.h"
#include <string.h>

namespace BR {

static bool checkSwapchainExtensionSupport(VkPhysicalDevice& device){
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	for (const auto& extension : availableExtensions) {
		if(strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME, extension.extensionName) == 0){
			return true;
		}
	}

	return false;
}

bool VulkanPhysicalDevice::isDeviceSuitable(VkSurfaceKHR& surface){
	if(!hasGraphicsQueue(physicalDevice, surface, queueFamilies)) return false;
	if(!hasPresentQueue (physicalDevice, surface, queueFamilies)) return false;

	if(checkSwapchainExtensionSupport(physicalDevice) == false) return false;

	SwapChainSupportDetails swapChainSupport(physicalDevice, surface);
	if(swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty()) return false;

	return true;
}

VulkanPhysicalDevice::VulkanPhysicalDevice(VkPhysicalDevice physicalDevice_) : physicalDevice(physicalDevice_){
	vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
	printf("Device name %s\n", deviceProperties.deviceName);

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

	queueFamilies.resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());
}

}
