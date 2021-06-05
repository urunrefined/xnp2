#ifndef VKIMAGEVIEW_H
#define VKIMAGEVIEW_H

#include <vulkan/vulkan.h>

namespace BR {

class VulkanImageView {
	VkImageView imageView;
	const VkDevice& device;

public:
	VulkanImageView(const VulkanImageView& imageView) = delete;

	VulkanImageView(const VkDevice& device_, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	~VulkanImageView();

	operator VkImageView& (){
		return imageView;
	}
};
}

#endif // VKIMAGEVIEW_H
