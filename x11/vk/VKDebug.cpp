#include "VKDebug.h"

#include <iostream>

namespace BR {

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t obj,
	size_t location,
	int32_t code,
	const char* layerPrefix,
	const char* msg,
	void* userData)
{
	(void) flags; (void) objType; (void) obj; (void) location; (void) code; (void) layerPrefix; (void) userData;
	std::cerr << "validation layer: " << msg << std::endl;

	return VK_FALSE;
}

VkResult VulkanDebugCallback::createDebugReportCallbackEXT()
{
	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = debugCallback;


	auto func = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	if (func != nullptr) {
		return func(instance, &createInfo, nullptr, &callback);
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

VulkanDebugCallback::VulkanDebugCallback(VkInstance &instance_, bool enableValidationLayers_) :
	instance(instance_), enableValidationLayers(enableValidationLayers_)
{
	if (!enableValidationLayers) return;

	if (createDebugReportCallbackEXT() != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug callback!");
	}
}

VulkanDebugCallback::~VulkanDebugCallback()
{
	if(!enableValidationLayers) return;

	auto func = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
	if (func != nullptr) {
		func(instance, callback, nullptr);
	}
}

}
