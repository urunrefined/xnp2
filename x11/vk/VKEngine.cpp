#include "VKEngine.h"

namespace BR {

VulkanContext::VulkanContext(bool enableValidationLayers) :
	glfwCtx(640, 400),
	instance(enableValidationLayers),
	callback(instance, enableValidationLayers),
	surface(glfwCtx, instance)
{

}

RenderState VulkanScaler::drawAndPresent(
		VulkanRenderBuffer& buffer){

	assert(queueSitter.done());
	assert(imageSitter.done());

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

		imageSitter.block();

		return RenderState::NEEDSSWAPCHAINUPDATE;
	}
	else if (resultImage != VK_SUCCESS) {
		imageSitter.cancel();
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	//Time ref;
	std::vector<VkCommandBuffer>& commandBuffers = buffer.commandBuffers;

	VkSubmitInfo submitInfo [1] = {};

	VkSubmitInfo& drawSubmitInfo = submitInfo[0];
	drawSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkPipelineStageFlags flagsWaitForUniform = VK_PIPELINE_STAGE_TRANSFER_BIT;

	drawSubmitInfo.waitSemaphoreCount = 1;
	drawSubmitInfo.pWaitSemaphores = imageAvailableSemaphore;
	drawSubmitInfo.pWaitDstStageMask = &flagsWaitForUniform;

	drawSubmitInfo.commandBufferCount = 1;
	drawSubmitInfo.pCommandBuffers = &commandBuffers[imageIndex];

	drawSubmitInfo.signalSemaphoreCount = 1;
	drawSubmitInfo.pSignalSemaphores = renderFinishedSemaphore;

	// Need to make sure that the command-buffers are never deleted
	// while they are still being used. This is why
	// we need to create fence which lets us know when
	// we can recreate the buffer

	//printf("Aquir 2 "); (Time() - ref).print();

	queueSitter.activate();

	if (vkQueueSubmit(device.graphicsQueue, 1, submitInfo, queueSitter) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	//printf("Aquir 3 "); (Time() - ref).print();

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = renderFinishedSemaphore;

	VkSwapchainKHR swapChains[] = {swapchainImages};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	VkResult result = vkQueuePresentKHR(device.presentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		//Nothing. Will be handled the next time we update
	} else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}

	imageSitter.block();
	return RenderState::OK;
}

}
