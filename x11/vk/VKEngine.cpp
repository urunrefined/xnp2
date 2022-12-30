#include "VKEngine.h"
#include <assert.h>

namespace BR {

VulkanContext::VulkanContext(bool enableValidationLayers) :
	glfwCtx(640, 400),
	instance(enableValidationLayers),
	callback(instance, enableValidationLayers),
	surface(glfwCtx, instance)
{

}

static void copyBuffer(const std::vector<Range>& ranges, VkCommandBuffer& commandBuffer, VulkanCmbBuffer& gBuffer){
	if(!ranges.empty()){
		VkBufferCopy buffers[ranges.size()];

		for(size_t i = 0; i< ranges.size(); i++){
			buffers[i] = {ranges[i].start, ranges[i].start, ranges[i].size};
		}

		vkCmdCopyBuffer(
			commandBuffer,
			gBuffer.stagingBuffer,
			gBuffer.bufferCard,
			(uint32_t)ranges.size(),
			buffers
		);
	}
}

static RenderState aquireImage(
		VkDevice& device,
		VkSwapchainKHR& swapchainImages,
		Sitter& imageSitter,
		VulkanSemaphore& imageAvailableSemaphore,
		uint32_t& imageIndex)
{
	imageSitter.activate();

	VkResult resultImage = vkAcquireNextImageKHR(device, swapchainImages, 0, imageAvailableSemaphore, imageSitter, &imageIndex);

	if(resultImage == VK_NOT_READY || resultImage == VK_TIMEOUT){
		imageSitter.cancel();
		return RenderState::WAITING;
	}else if (resultImage == VK_ERROR_OUT_OF_DATE_KHR) {
		printf("VK_ERROR_OUT_OF_DATE_KHR\n");
		imageSitter.cancel();
		return RenderState::NEEDSSWAPCHAINUPDATE;
	}
	else if (resultImage == VK_SUBOPTIMAL_KHR){
		//BUG: Image was aquired, but we don't want to use it
		//1) Using this image later in the function crashes the mesa driver
		//   for some reason
		//2) In most cases waiting for the fence is fine,
		//   but there are some circumstances in which
		//   the fence will never be signaled (Therefore this
		//   will block forver)
		printf("VK_SUBOPTIMAL_KHR\n");
		imageSitter.block();

		return RenderState::NEEDSSWAPCHAINUPDATE;
	}
	else if (resultImage != VK_SUCCESS) {
		imageSitter.cancel();
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	return RenderState::OK;
}

static RenderState present(
	VulkanSemaphore& renderFinishedSemaphore,
	VkSwapchainKHR& swapchainImages,
	uint32_t& imageIndex,
	VkQueue& presentQueue)
{
	//printf("Aquir 3 "); (Time() - ref).print();

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = renderFinishedSemaphore;

	VkSwapchainKHR swapChains[] = {swapchainImages};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	VkResult result = vkQueuePresentKHR(presentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		//Nothing. Will be handled the next time we update
	} else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}

	return RenderState::OK;
}

RenderState VulkanScaler::drawAndPresent(
		VulkanRenderBuffer& renderBuffer, std::vector<VulkanCmbBuffer *>& cmbBuffers){

	assert(queueSitter.done());
	assert(imageSitter.done());

	RenderState renderState = aquireImage(device, swapchainImages, imageSitter,
		renderSemaphores->imageAvailableSemaphore, imageIndex);

	if(renderState != RenderState::OK) return renderState;

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = pool;
	allocInfo.commandBufferCount = 1;

	//TODO: ERROR HANDLING
	if(commandBuffer == VK_NULL_HANDLE){
		vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
	}
	else{
		vkFreeCommandBuffers(device, pool, 1, &commandBuffer);
		vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
	}

	if(commandBuffer == VK_NULL_HANDLE){
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	for(VulkanCmbBuffer *cmbBuffer : cmbBuffers){
		copyBuffer(cmbBuffer->ranges.ranges, commandBuffer, *cmbBuffer);
		cmbBuffer->ranges.ranges.clear();
	}

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo [2] = {};
	VkPipelineStageFlags flagsWaitForImage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

	{
		VkSubmitInfo& updateSubmitInfo = submitInfo[0];
		updateSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		updateSubmitInfo.commandBufferCount = 1;
		updateSubmitInfo.pCommandBuffers = &commandBuffer;

		updateSubmitInfo.waitSemaphoreCount = 1;
		updateSubmitInfo.pWaitSemaphores = renderSemaphores->imageAvailableSemaphore;
		updateSubmitInfo.pWaitDstStageMask = &flagsWaitForImage;

		updateSubmitInfo.signalSemaphoreCount = 1;
		updateSubmitInfo.pSignalSemaphores = renderSemaphores->vboUpdatedSemaphore;
	}

	VkPipelineStageFlags flagsWaitForVBOUpdate = VK_PIPELINE_STAGE_TRANSFER_BIT;

	{
		VkSubmitInfo& drawSubmitInfo = submitInfo[1];
		drawSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;


		drawSubmitInfo.waitSemaphoreCount = 1;
		drawSubmitInfo.pWaitSemaphores = renderSemaphores->vboUpdatedSemaphore;
		drawSubmitInfo.pWaitDstStageMask = &flagsWaitForVBOUpdate;

		drawSubmitInfo.commandBufferCount = 1;
		std::vector<VkCommandBuffer>& commandBuffers = renderBuffer.commandBuffers;
		drawSubmitInfo.pCommandBuffers = &commandBuffers[imageIndex];

		drawSubmitInfo.signalSemaphoreCount = 1;
		drawSubmitInfo.pSignalSemaphores = renderSemaphores->renderFinishedSemaphore;

	}

	// Need to make sure that the command-buffers are never deleted
	// while they are still being used. This is why
	// we need to create fence which lets us know when
	// we can recreate the buffer

	//printf("Aquir 2 "); (Time() - ref).print();

	queueSitter.activate();

	if (vkQueueSubmit(device.graphicsQueue, 2, submitInfo, queueSitter) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	//printf("Aquir 3 "); (Time() - ref).print();

	present(renderSemaphores->renderFinishedSemaphore, swapchainImages, imageIndex, device.presentQueue);

	imageSitter.block();
	return RenderState::OK;
}

}
