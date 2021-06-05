#ifndef VKCOMMANDBUFFER_H
#define VKCOMMANDBUFFER_H

#include <vulkan/vulkan.h>

namespace BR {

class VulkanCommandBuffer
{
	const VkDevice& device;
	const VkCommandPool& commandPool;
	VkQueue graphicsQueue;

public:
	VkCommandBuffer commandBuffer;

	VulkanCommandBuffer(const VkDevice& device_, const VkCommandPool& commandPool_, VkQueue graphicsQueue_);
	void submit();
	~VulkanCommandBuffer();

	operator VkCommandBuffer& (){
		return commandBuffer;
	}
};

} // namespace BR

#endif // VKCOMMANDBUFFER_H
