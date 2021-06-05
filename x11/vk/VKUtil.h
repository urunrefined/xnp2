#ifndef VKUTIL_H_
#define VKUTIL_H_

#include <vulkan/vulkan.h>

#include <vector>

namespace BR {

class SwapChainSupportDetails {
public:
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;

	SwapChainSupportDetails(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);
};

uint32_t findMemoryType(const VkPhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
VkFormat findDepthFormat(const VkPhysicalDevice& physicalDevice);
VkRect2D getScissor(double ideal, unsigned int width, unsigned int height);

bool hasGraphicsQueue(
	VkPhysicalDevice physicalDevice,
	VkSurfaceKHR& surface,
	const std::vector<VkQueueFamilyProperties>& queueFamilies);

bool hasPresentQueue(
	VkPhysicalDevice physicalDevice,
	VkSurfaceKHR& surface,
	const std::vector<VkQueueFamilyProperties>& queueFamilies);

uint32_t getFirstPresentQueue(
	VkPhysicalDevice physicalDevice,
	VkSurfaceKHR& surface,
	const std::vector<VkQueueFamilyProperties>& queueFamilies);

bool getFirstGraphicsQueue(
	VkPhysicalDevice physicalDevice,
	VkSurfaceKHR& surface,
	const std::vector<VkQueueFamilyProperties>& queueFamilies);

}

#endif
