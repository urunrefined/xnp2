#ifndef VKIMAGE_H
#define VKIMAGE_H

#include "vulkan/vulkan.h"

namespace BR {

class VulkanImage {
	VkDevice& device;
public:
	VkImage image;

	VulkanImage(VkDevice &device_, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage);
	~VulkanImage();

	operator VkImage& (){
		return image;
	}

};

}

#endif // VKIMAGE_H
