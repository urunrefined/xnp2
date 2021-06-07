#include <vector>
#include <array>
#include <set>
#include <memory>
#include <new>
#include <stdexcept>
#include <fstream>
#include <algorithm>

#include "VKGLFW.h"
#include "VKUtil.h"

namespace BR {

uint32_t findMemoryType(const VkPhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

VkRect2D getIntegerScissor(unsigned int multipleX, unsigned int multipleY, unsigned int width, unsigned int height){
	unsigned int divX = width / multipleX;
	unsigned int divY = height / multipleY;

	if(divX == 0 || divY == 0) return {{0,0}, {width, height}};

	unsigned int leastCommonDenominator = std::min(divX, divY);

	uint32_t extentX = leastCommonDenominator * multipleX;
	uint32_t extentY = leastCommonDenominator * multipleY;

	int32_t offsetX = (width - extentX)  / 2;
	int32_t offsetY = (height - extentY) / 2;

	return {{offsetX, offsetY}, {extentX, extentY}};
}

VkRect2D getAspectScissor(double ideal, unsigned int width, unsigned int height){
	unsigned int xOffset = 0;
	unsigned int yOffset = 0;

	double ratio = (double)width / (double)height;

	if(ratio > ideal){
		xOffset = (width - (unsigned int)((double) width / (ratio / ideal)));
	}
	else if(ratio < ideal){
		yOffset = (height - (unsigned int)((double) height / (1 / (ratio / ideal))));
	}
	
	printf("xOffset %u, yOffset %u\n", xOffset, yOffset);

	if(xOffset > width) xOffset = width;
	if(yOffset > height) yOffset = height;

	//printf("%u, %u | %u, %u | %u, %u\n", width, height, xOffset/2, yOffset / 2, width - xOffset, height - yOffset);
	VkRect2D scissor;

	scissor.offset = VkOffset2D {(int32_t)(xOffset / 2), (int32_t)(yOffset / 2)};
	scissor.extent = VkExtent2D {width - xOffset, height - yOffset};

	if(scissor.extent.width < 1) scissor.extent.width = 1;
	if(scissor.extent.height < 1) scissor.extent.height = 1;

	return scissor;
}

SwapChainSupportDetails::SwapChainSupportDetails(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) {

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	if (formatCount != 0) {
		formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, presentModes.data());
	}
}

static VkFormat findSupportedFormat(const VkPhysicalDevice& physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		} else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format!");
}

VkFormat findDepthFormat(const VkPhysicalDevice& physicalDevice) {
	return findSupportedFormat(physicalDevice,
		{VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
			VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

bool hasGraphicsQueue(VkPhysicalDevice physicalDevice, VkSurfaceKHR& surface,
		const std::vector<VkQueueFamilyProperties>& queueFamilies) {

	uint32_t i = 0;

	for (const auto& queueFamily : queueFamilies) {
		VkBool32 presentSupport;

		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
		if(queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT){
			return true;
		}

		i++;
	}

	return false;
}

bool hasPresentQueue(VkPhysicalDevice physicalDevice, VkSurfaceKHR& surface,
		const std::vector<VkQueueFamilyProperties>& queueFamilies) {

	uint32_t i = 0;

	for (const auto& queueFamily : queueFamilies) {
		VkBool32 presentSupport;

		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
		if(queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT){
			if (presentSupport) {
				return true;
			}
		}

		i++;
	}

	return false;
}

uint32_t getFirstPresentQueue(VkPhysicalDevice physicalDevice, VkSurfaceKHR& surface,
		const std::vector<VkQueueFamilyProperties>& queueFamilies) {

	uint32_t i = 0;

	for (const auto& queueFamily : queueFamilies) {
		VkBool32 presentSupport;

		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
		if(queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT){
			if (presentSupport) {
				return i;
			}
		}

		i++;
	}

	//we should never get here
	throw -1;
}

bool getFirstGraphicsQueue(VkPhysicalDevice physicalDevice, VkSurfaceKHR& surface,
		const std::vector<VkQueueFamilyProperties>& queueFamilies) {

	uint32_t i = 0;

	for (const auto& queueFamily : queueFamilies) {
		VkBool32 presentSupport;

		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
		if(queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT){
			return i;
		}

		i++;
	}

	//we should never get here
	throw -1;
}

std::vector <VkQueueFamilyProperties> findQueueFamilies(VkPhysicalDevice physicalDevice) {
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

	return queueFamilies;
}

}
