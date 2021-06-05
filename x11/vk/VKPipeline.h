#ifndef VKPIPELINE_H_
#define VKPIPELINE_H_

#include "VKUtil.h"
#include "VKShaders.h"
#include "VKRenderBuffer.h"
#include "Array.h"
#include "stdexcept"

namespace BR {

enum class RenderAttributes : size_t {
	FILL_PAINTER = 0,
	FILL_ZCLIP,
	LINE_PAINTER_LIST,
	LINE_PAINTER_STRIP,
	LINE_PAINTER_TRI_LIST,
	LINE_ZCLIP_LIST,
	LINE_ZCLIP_STRIP,
	LINE_ZCLIP_TRI_LIST,
	COUNT
};

enum class BlendBool {
	BLEND_ENABLE,
	BLEND_DISABLE
};

enum class DepthBool {
	DEPTH_ENABLE,
	DEPTH_DISABLE
};

enum class Topology {
	TOPOLOGY_TRIANGLE_LIST,
	TOPOLOGY_LINE_LIST,
	TOPOLOGY_LINE_STRIP,
};

template <size_t N>
static void createPipeline(
	const VkDevice& device,
	std::array<VkPipelineShaderStageCreateInfo, N>& shaderStages,
	const VkRect2D& scissor,
	const VkRenderPass& renderPass,
	VkPipeline *pipelines,
	size_t pipelineCount,
	ArrayView<VkPolygonMode> polygonModes,
	//ArrayView<BlendBool> blendEnable,
	ArrayView<DepthBool> depthEnable,
	ArrayView<VkPrimitiveTopology> topology)
{
	//TODO: blendEnable is currently ignored

	assert(pipelineCount == polygonModes.count);
	//assert(pipelineCount == blendEnable.count);
	assert(pipelineCount == depthEnable.count);

	VkPipelineInputAssemblyStateCreateInfo inputAssemblies [topology.count];

	for (size_t i = 0; i < topology.count; i++){
		VkPipelineInputAssemblyStateCreateInfo& inputAssembly = inputAssemblies[i];
		inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = topology[i];
		inputAssembly.primitiveRestartEnable = VK_FALSE;
	}

	VkViewport viewport = {};
	viewport.x = (float) scissor.offset.x;
	viewport.y = (float) scissor.offset.y;
	viewport.width = (float) scissor.extent.width;
	viewport.height = (float) scissor.extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;


	VkPipelineRasterizationStateCreateInfo rasterizers [polygonModes.count];

	for (size_t i = 0; i < polygonModes.count; i++){
		VkPipelineRasterizationStateCreateInfo& rasterizer = rasterizers[i];
		rasterizer = {};

		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = polygonModes[i];
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
	}

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineDepthStencilStateCreateInfo depthStencils [depthEnable.count];

	for (size_t i = 0; i < depthEnable.count; i++){
		VkPipelineDepthStencilStateCreateInfo& depthStencil = depthStencils[i];
		depthStencil = {};

		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

		if(depthEnable.data()[i] == DepthBool::DEPTH_ENABLE){
			depthStencil.depthTestEnable = VK_TRUE;
			depthStencil.depthWriteEnable = VK_TRUE;
		}
		else{
			depthStencil.depthTestEnable = VK_FALSE;
			depthStencil.depthWriteEnable = VK_FALSE;
		}

		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;
	}

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;


	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	VkGraphicsPipelineCreateInfo pipelineInfos[pipelineCount];

	for (size_t i = 0; i < pipelineCount; i++){
		VkGraphicsPipelineCreateInfo& pipelineInfo = pipelineInfos[i];
		pipelineInfo = {};

		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = (uint32_t)shaderStages.size();
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = 0;
		pipelineInfo.pInputAssemblyState = &inputAssemblies[i];
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizers[i];
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencils[i];
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.layout = 0;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = 0;
	}

	if (vkCreateGraphicsPipelines(
		device,
		VK_NULL_HANDLE,
		(uint32_t)pipelineCount,
		pipelineInfos,
		nullptr,
		pipelines) != VK_SUCCESS)
	{}
}
//createPipeline2(device_, shaderStages, vertexInputInfo, scissor, renderPass, pipelineLayout, pipelines, arraySize(pipelines));
template <size_t N>
static void createPipeline2(
	const VkDevice& device,
	std::array<VkPipelineShaderStageCreateInfo, N>& shaderStages,
	const VkRect2D& scissor,
	const VkRenderPass& renderPass,
	VkPipeline *pipelines,
	size_t pipelineCount)
{
	enum DepthBool depthing [(size_t)RenderAttributes::COUNT] = {
		DepthBool::DEPTH_DISABLE,
		DepthBool::DEPTH_ENABLE,
		DepthBool::DEPTH_DISABLE,
		DepthBool::DEPTH_DISABLE,
		DepthBool::DEPTH_DISABLE,
		DepthBool::DEPTH_ENABLE,
		DepthBool::DEPTH_ENABLE,
		DepthBool::DEPTH_ENABLE
	};

	//enum BlendBool blending [(size_t)Gl::RenderAttributes::COUNT] = {
	//	BlendBool::BLEND_ENABLE,
	//	BlendBool::BLEND_ENABLE,
	//	BlendBool::BLEND_ENABLE,
	//	BlendBool::BLEND_ENABLE,
	//	BlendBool::BLEND_ENABLE,
	//	BlendBool::BLEND_ENABLE,
	//	BlendBool::BLEND_ENABLE,
	//	BlendBool::BLEND_ENABLE
	//};

	enum VkPolygonMode polyMode [(size_t)RenderAttributes::COUNT] = {
		VK_POLYGON_MODE_FILL,
		VK_POLYGON_MODE_FILL,
		VK_POLYGON_MODE_LINE,
		VK_POLYGON_MODE_LINE,
		VK_POLYGON_MODE_LINE,
		VK_POLYGON_MODE_LINE,
		VK_POLYGON_MODE_LINE,
		VK_POLYGON_MODE_LINE
	};

	enum VkPrimitiveTopology topology [(size_t)RenderAttributes::COUNT] = {
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
		VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
		VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
	};

	//ArrayView<BlendBool>           blendingArr = makeArray(blending);
	ArrayView<DepthBool>           depthingArr = makeArray(depthing);
	ArrayView<VkPolygonMode>       polyModeArr = makeArray(polyMode);
	ArrayView<VkPrimitiveTopology> topologyArr = makeArray(topology);

	createPipeline(
		device,
		shaderStages,
		scissor,
		renderPass,
		pipelines,
		pipelineCount,
		polyModeArr,
		//blendingArr,
		depthingArr,
		topologyArr
	);
}

} //namespace BR

#endif
