#ifndef VKRENDERER_H_
#define VKRENDERER_H_

#include "VKUtil.h"
#include "VKShaders.h"
#include "VKPhysicalDevice.h"
#include "VKPipelineV.h"
#include "VKPipelineTex.h"
#include "VKRenderPass.h"
#include "VKDescriptorSet.h"
#include "VKSampler.h"
#include "VKCommandPool.h"

namespace BR {

static const unsigned int pc98Width = 640;
static const unsigned int pc98Height = 400;

class Image {
public:
	unsigned char data[pc98Width * pc98Height * 4] = {0};
};

class VulkanRenderer {
public:
	const VkPhysicalDevice& physicalDevice;
	const VulkanDevice& device;
	
	ShaderStore& shaderStore;
	VulkanRenderPass renderPass;

	std::unique_ptr<PipelineV>    pipelineV;
	std::unique_ptr<PipelineTex>  pipelineTex;

	VkImage& texture;
	uint32_t graphicsFamily;
	VkQueue& graphicsQueue;

	VulkanCommandPool commandPool;

	VulkanImageView textureView;
	VulkanSampler sampler;

	VulkanDescriptorLayout descriptorLayout;
	VulkanDescriptorPool descriptorPool;
	VulkanDescriptorSet descriptorSet;

	Image image;
	
	VulkanRenderer(
		const VulkanPhysicalDevice& physicalDevice_, const VulkanDevice& device_,
		uint32_t graphicsFamily_, VkQueue& graphicsQueue_, ShaderStore& shaderStore_,
		VkFormat swapChainFormat, VkExtent2D swapChainExtent, VkImage& texture_
	);

	void reCreatePipeline(VkExtent2D swapChainExtent){
		VkRect2D scissor = getScissor((double)pc98Width / (double) pc98Height, swapChainExtent.width, swapChainExtent.height);

		printf("Recreate color pipelines with offset [%u, %u], extent [%u, %u]\n",
			   scissor.offset.x, scissor.offset.y,
			   scissor.extent.width, scissor.extent.height);

		pipelineV    = std::unique_ptr<PipelineV>   (new PipelineV   (device, shaderStore, scissor, renderPass));
		pipelineTex  = std::unique_ptr<PipelineTex> (new PipelineTex (device, shaderStore, scissor, renderPass, descriptorLayout));
	}
	
	void updateImage();

	virtual ~VulkanRenderer(){}
	
	operator VkRenderPass& (){
		return renderPass;
	}
};

} // namepsace BR

#endif

