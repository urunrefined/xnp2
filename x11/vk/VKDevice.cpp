#include "VKDevice.h"

#include <vulkan/vulkan.h>
#include "VKUtil.h"

#include <vector>
#include <set>
#include <stdexcept>

namespace BR {

static const std::vector<const char*> validationLayers = {
	//"VK_LAYER_LUNARG_vktrace",
	//"VK_LAYER_MESA_overlay",
	"VK_LAYER_KHRONOS_validation"
};

VulkanDevice::VulkanDevice(bool enableValidationLayer, const VkPhysicalDevice& physicalDevice, VkSurfaceKHR& surface, const std::vector<VkQueueFamilyProperties>& queueFamilies) {

	uint32_t graIdx = getFirstPresentQueue (physicalDevice, surface, queueFamilies);
	uint32_t preIdx = getFirstGraphicsQueue(physicalDevice, surface, queueFamilies);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {graIdx, preIdx};

	float queuePriority = 1.0f;
	for (int queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = (uint32_t) queueCreateInfos.size();

	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.fillModeNonSolid = VK_TRUE;

	createInfo.pEnabledFeatures = &deviceFeatures;


	const char* extensions[] {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	createInfo.enabledExtensionCount = sizeof(extensions) / sizeof(extensions[0]);
	createInfo.ppEnabledExtensionNames = extensions;

	if (enableValidationLayer) {
		createInfo.enabledLayerCount = (uint32_t)validationLayers.size();
		createInfo.ppEnabledLayerNames = validationLayers.data();
	} else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(device, graIdx, 0, &graphicsQueue);
	vkGetDeviceQueue(device, preIdx, 0, &presentQueue);

	graphicsFamily = graIdx;
	presentFamily = preIdx;
}

VulkanDevice::~VulkanDevice(){
	vkDeviceWaitIdle(device);
	vkDestroyDevice(device, nullptr);
}

}
