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
	std::unique_ptr<PipelineTex>  pipelineAspect;
	std::unique_ptr<PipelineTex>  pipelineStretch;
	std::unique_ptr<PipelineTex>  pipelineInteger;

	VkImage& texture;
	uint32_t graphicsFamily;
	VkQueue& graphicsQueue;
	bool textureDirty;

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

		{
			VkRect2D scissor = getAspectScissor((double)pc98Width / (double) pc98Height, swapChainExtent.width, swapChainExtent.height);

			printf("Recreate apect pipeline with offset [%u, %u], extent [%u, %u]\n",
				   scissor.offset.x, scissor.offset.y,
				   scissor.extent.width, scissor.extent.height);

			pipelineV        = std::unique_ptr<PipelineV>   (new PipelineV   (device, shaderStore, scissor, renderPass));
			pipelineAspect   = std::unique_ptr<PipelineTex> (new PipelineTex (device, shaderStore, scissor, renderPass, descriptorLayout));
		}
		{
			VkRect2D scissor = getIntegerScissor(pc98Width, pc98Height, swapChainExtent.width, swapChainExtent.height);

			printf("Recreate integer pipeline with offset [%u, %u], extent [%u, %u]\n",
				   scissor.offset.x, scissor.offset.y,
				   scissor.extent.width, scissor.extent.height);

			pipelineInteger  = std::unique_ptr<PipelineTex> (new PipelineTex (device, shaderStore, scissor, renderPass, descriptorLayout));
		}
		{

			VkRect2D scissor {{0,0}, {swapChainExtent.width, swapChainExtent.height}};

			printf("Recreate stretch pipeline with offset [%u, %u], extent [%u, %u]\n",
				   scissor.offset.x, scissor.offset.y,
				   scissor.extent.width, scissor.extent.height);

			pipelineStretch  = std::unique_ptr<PipelineTex> (
					new PipelineTex (device, shaderStore, scissor, renderPass, descriptorLayout));
		}
	}
	
	void updateImage();

	virtual ~VulkanRenderer(){}
	
	operator VkRenderPass& (){
		return renderPass;
	}
};

} // namepsace BR

#endif

