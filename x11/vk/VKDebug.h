#ifndef VKDEBUG_H
#define VKDEBUG_H

#include <vulkan/vulkan.h>

namespace BR {

class VulkanDebugCallback{
public:
	VkInstance& instance;
	bool enableValidationLayers;

	VkDebugReportCallbackEXT callback;

	VkResult createDebugReportCallbackEXT();

	VulkanDebugCallback(VkInstance& instance_, bool enableValidationLayers_);

	~VulkanDebugCallback();
};

} // namespace Vk

#endif // VKDEBUG_H
