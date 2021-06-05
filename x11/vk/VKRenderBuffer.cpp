#include "VKRenderBuffer.h"
#include "VKSwapchain.h"

#include <stdexcept>

namespace BR {

static void beginVulkanRendering(const VulkanDevice& device, VulkanCommandPool& commandPool, std::vector<std::unique_ptr<VulkanFramebuffer>>& framebuffers, std::vector<VkCommandBuffer>& commandBuffers,
		VkExtent2D extent, VulkanRenderPass& renderPass)
{
	if (commandBuffers.size() > 0) {
		vkFreeCommandBuffers(device, commandPool, (uint32_t)commandBuffers.size(), commandBuffers.data());
	}

	commandBuffers.resize(framebuffers.size());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

	if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

	for (size_t i = 0; i < commandBuffers.size(); i++) {
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		vkBeginCommandBuffer(commandBuffers[i], &beginInfo);

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = (*framebuffers[i]);
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = extent;

		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
		clearValues[1].depthStencil = {1.0f, 0};

		renderPassInfo.clearValueCount = (uint32_t)clearValues.size();
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}
}

static void endVulkanRendering(std::vector<VkCommandBuffer>& commandBuffers){
	for (size_t i = 0; i < commandBuffers.size(); i++) {
		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}

void VulkanRenderBuffer::begin(VulkanRenderPass& renderPass, VulkanSwapchain& swapChain){
	beginVulkanRendering(device, commandPool, swapChain.swapChainFramebuffers, commandBuffers, swapChain.extent, renderPass);
}

void VulkanRenderBuffer::end(){
	endVulkanRendering(commandBuffers);
}

}

