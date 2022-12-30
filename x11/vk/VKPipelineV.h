#ifndef VKPIPELINEV_H
#define VKPIPELINEV_H

#include "VKShaders.h"
#include "VKRenderBuffer.h"

namespace BR {

class PipelineV {

public:
	const VkDevice& device;

	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;

	PipelineV(const VkDevice& device_, ShaderStore& shader3D, const VkRect2D& scissor, const VkRenderPass& renderPass);

	~PipelineV();

	VkPipeline& getPipeline();

	void record(
		VulkanRenderBuffer& renderBuffer,
		size_t drawCount);
};

}

#endif // VKPIPELINEV_H
