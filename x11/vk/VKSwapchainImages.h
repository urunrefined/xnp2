#ifndef VKSWAPCHAINIMAGES_H
#define VKSWAPCHAINIMAGES_H

#include "vulkan/vulkan.h"

#include <vector>

namespace BR {

class VulkanSwapchainImages {
public:
	const VkPhysicalDevice& physicalDevice;
	VkDevice& device;
	VkSurfaceKHR& surface;

	VkSwapchainKHR swapChain;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImage> swapChainImages;
	uint32_t graphicsFamily;
	uint32_t presentFamily;

	VulkanSwapchainImages(const VkPhysicalDevice& physicalDevice_, VkDevice& device_,
		VkSurfaceKHR& surface_, uint32_t width, uint32_t height,
		uint32_t graphicsFamily_, uint32_t presentFamily_);

	void reCreate(uint32_t width, uint32_t height);

	~VulkanSwapchainImages();

	operator VkSwapchainKHR&(){
		return swapChain;
	}
};

}

#endif // VKSWAPCHAINIMAGES_H
