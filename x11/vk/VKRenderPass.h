#ifndef VULKANRENDERPASS_H
#define VULKANRENDERPASS_H

#include "VKUtil.h"

namespace BR {

class VulkanRenderPass {
public:
	const VkDevice& device;
	VkRenderPass renderPass;

	VulkanRenderPass(const VkDevice& device_, const VkPhysicalDevice& physicalDevice, VkFormat swapChainImageFormat);
	~VulkanRenderPass();

	operator VkRenderPass& (){
		return renderPass;
	}
};

}

#endif // VULKANRENDERPASS_H
