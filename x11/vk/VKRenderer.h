#ifndef VKRENDERER_H_
#define VKRENDERER_H_

#include "VKUtil.h"
#include "VKShaders.h"
#include "VKPhysicalDevice.h"
#include "VKPipelineV.h"
#include "VKPipelineTex.h"
#include "VKPipelineTexExt.h"
#include "VKRenderPass.h"
#include "VKDescriptorSet.h"
#include "VKDescriptorSetExt.h"
#include "VKSampler.h"
#include "VKCommandPool.h"
#include "VKTexture.h"
#include "VKImage.h"
#include "VKImageMemory.h"
#include "VKTexture.h"

namespace BR {

class VulkanRenderer {
public:
	const VkPhysicalDevice& physicalDevice;
	const VulkanDevice& device;
	
	ShaderStore& shaderStore;
	VulkanRenderPass renderPass;

	std::unique_ptr<PipelineV>       pipelineV;
	std::unique_ptr<PipelineTex>     pipelineAspect;
	std::unique_ptr<PipelineTex>     pipelineAspect1to1;
	std::unique_ptr<PipelineTex>     pipelineStretch;
	std::unique_ptr<PipelineTex>     pipelineInteger;
	std::unique_ptr<PipelineTexExt>  pipelineStretchExt;

	uint32_t graphicsFamily;
	VkQueue& graphicsQueue;
	VulkanSampler sampler;

	VulkanDescriptorLayout descriptorLayout;
	VulkanDescriptorLayoutExt descriptorLayoutExt;
	
	VulkanRenderer(
		const VulkanPhysicalDevice& physicalDevice_, const VulkanDevice& device_,
		uint32_t graphicsFamily_, VkQueue& graphicsQueue_, ShaderStore& shaderStore_,
		VkFormat swapChainFormat, VkExtent2D swapChainExtent
	);

	void reCreatePipeline(VkExtent2D swapChainExtent){
		static const unsigned int pc98Width = 640;
		static const unsigned int pc98Height = 400;

		{
			VkRect2D scissor = getAspectScissor((double)pc98Width / (double) pc98Height, swapChainExtent.width, swapChainExtent.height);

			printf("Recreate aspect pipeline with offset [%u, %u], extent [%u, %u]\n",
				   scissor.offset.x, scissor.offset.y,
				   scissor.extent.width, scissor.extent.height);

			pipelineV        = std::unique_ptr<PipelineV>   (new PipelineV   (device, shaderStore, scissor, renderPass));
			pipelineAspect   = std::unique_ptr<PipelineTex> (new PipelineTex (device, shaderStore, scissor, renderPass, descriptorLayout));
		}

		{
			VkRect2D scissor = getAspectScissor(1, swapChainExtent.width, swapChainExtent.height);

			printf("Recreate 1to1 aspect pipeline with offset [%u, %u], extent [%u, %u]\n",
				   scissor.offset.x, scissor.offset.y,
				   scissor.extent.width, scissor.extent.height);

			pipelineAspect1to1   = std::unique_ptr<PipelineTex> (new PipelineTex (device, shaderStore, scissor, renderPass, descriptorLayout));
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

			pipelineStretchExt  = std::unique_ptr<PipelineTexExt> (
					new PipelineTexExt (device, shaderStore, scissor, renderPass, descriptorLayoutExt));
		}
	}

	virtual ~VulkanRenderer(){}
	
	operator VkRenderPass& (){
		return renderPass;
	}
};

} // namepsace BR

#endif

