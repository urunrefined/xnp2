#include "VKRenderer.h"
#include "VKBuffers.h"
#include "VKCommandBuffer.h"

#include <string.h>

namespace BR {

VulkanRenderer::VulkanRenderer(
		const VulkanPhysicalDevice& physicalDevice_, const VulkanDevice& device_,
		uint32_t graphicsFamily_, VkQueue& graphicsQueue_, ShaderStore& shaderStore_,
		VkFormat swapChainFormat, VkExtent2D swapChainExtent)
	: physicalDevice(physicalDevice_), device(device_), shaderStore(shaderStore_),
		renderPass(device, physicalDevice, swapChainFormat),
		//descriptorCommandPool(device, device.graphicsFamily)
		graphicsFamily(graphicsFamily_),
		graphicsQueue(graphicsQueue_),
		sampler(physicalDevice, device),
		descriptorLayout(device),
		descriptorLayoutExt(device)
{
	reCreatePipeline(swapChainExtent);
}
	
} //namespace D5
