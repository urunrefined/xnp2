#include "VKPipelineTexExt.h"
#include "Core.h"
#include "Vertex.h"

namespace BR {

PipelineTexExt::PipelineTexExt(
	const VkDevice& device_,
	ShaderStore& shader3D,
	const VkRect2D& scissor,
	const VkRenderPass& renderPass,
	const VkDescriptorSetLayout& desciptorSetLayout)
	: device(device_)
{
	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = shader3D.vertTexExt;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = shader3D.fragTex;
	fragShaderStageInfo.pName = "main";

	std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages = {vertShaderStageInfo, fragShaderStageInfo};

	std::array<VkVertexInputBindingDescription, 2> bindingDescription = {};
	bindingDescription[0].binding = 0;
	bindingDescription[0].stride = sizeof(Vec2);
	bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	bindingDescription[1].binding = 1;
	bindingDescription[1].stride = sizeof(Vec2);
	bindingDescription[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[0].offset = 0;

	attributeDescriptions[1].binding = 1;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributeDescriptions[1].offset = 0;

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	vertexInputInfo.vertexBindingDescriptionCount = 2;
	vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)attributeDescriptions.size();
	vertexInputInfo.pVertexBindingDescriptions = bindingDescription.data();
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = (float) scissor.offset.x;
	viewport.y = (float) scissor.offset.y;
	viewport.width = (float) scissor.extent.width;
	viewport.height = (float) scissor.extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &desciptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0;

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("vkCreatePipelineLayout failed");
	}

	VkGraphicsPipelineCreateInfo pipelineInfo {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages.begin();
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	vertexInputInfo.vertexBindingDescriptionCount = 2;
	vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)attributeDescriptions.size();
	vertexInputInfo.pVertexBindingDescriptions = bindingDescription.data();
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
		throw std::runtime_error("vkCreateGraphicsPipelines failed");
	}
}

PipelineTexExt::~PipelineTexExt(){
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	vkDestroyPipeline(device, pipeline, nullptr);
}

VkPipeline &PipelineTexExt::getPipeline(){
	return pipeline;
}

void PipelineTexExt::record(
	VulkanRenderBuffer &renderBuffer,
	const VkDescriptorSet& descriptorSet,
	VkBuffer vertices,
	VkDeviceSize verticesOffset,
	VkBuffer uvs,
	VkDeviceSize uvOffset,
	size_t drawCount)
{
	if(!drawCount) return;

	auto& commandBuffers = renderBuffer.commandBuffers;

	for (size_t i = 0; i < commandBuffers.size(); i++) {

		VkPipeline& pipeline = getPipeline();

		VkBuffer buffers [2] = {vertices, uvs};
		VkDeviceSize bufferSizes [2] = {verticesOffset, uvOffset};

		vkCmdBindVertexBuffers(commandBuffers[i], 0, 2, buffers, bufferSizes);

		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

		vkCmdDraw(commandBuffers[i], (uint32_t)drawCount, 1, 0, 0);
	}
}

}
