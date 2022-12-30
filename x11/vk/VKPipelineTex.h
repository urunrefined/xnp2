#ifndef VKPIPELINETEX_H
#define VKPIPELINETEX_H

#include "VKShaders.h"
#include "VKRenderBuffer.h"

namespace BR {

class PipelineTex {

public:
	const VkDevice& device;

	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;

	PipelineTex(const VkDevice& device_, ShaderStore& shader3D, const VkRect2D& scissor, const VkRenderPass& renderPass, const VkDescriptorSetLayout& desciptorSetLayout);

	~PipelineTex();

	VkPipeline& getPipeline();

	void record(
		VulkanRenderBuffer& renderBuffer,
		const VkDescriptorSet& descriptorSet,
		size_t drawCount);
};

}

#endif // VKPIPELINETEX_H
