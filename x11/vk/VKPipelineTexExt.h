#ifndef VKPIPELINETEXEXT_H
#define VKPIPELINETEXEXT_H

#include "VKUtil.h"
#include "VKShaders.h"
#include "VKRenderBuffer.h"
#include "VKPipeline.h"

namespace BR {

class PipelineTexExt {

public:
	const VkDevice& device;

	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;

	PipelineTexExt(const VkDevice& device_, ShaderStore& shader3D, const VkRect2D& scissor, const VkRenderPass& renderPass, const VkDescriptorSetLayout& desciptorSetLayout);

	~PipelineTexExt();

	VkPipeline& getPipeline();

	void record(
		VulkanRenderBuffer& renderBuffer,
		const VkDescriptorSet& descriptorSet,
		size_t drawCount);
};

}

#endif // VKPIPELINETEXEXT_H
