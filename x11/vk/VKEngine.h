#ifndef VKENGINE_H
#define VKENGINE_H

#include "VKGLFW.h"
#include "VKInstance.h"
#include "VKCommandPool.h"
#include "VKSurface.h"
#include "VKPhysicalDevice.h"
#include "VKDevice.h"
#include "VKSwapchain.h"
#include "VKRenderPass.h"
#include "VKDebug.h"
#include "VKSwapchainImages.h"
#include "VKSwapchain.h"
#include "VKShaders.h"
#include "VKRenderBuffer.h"
#include "VKRenderer.h"
#include "VKSync.h"
#include "VKImage.h"
#include "VKImageMemory.h"
#include "VKSampler.h"
#include "VKBuffers.h"
#include "VKDescriptorSet.h"

#include <memory>

namespace BR {

class VulkanContext {
public:
	GLFWContext glfwCtx;
	VulkanInstance instance;
	VulkanDebugCallback callback;
	VulkanSurface surface;

	VulkanContext(bool enableValidationLayers);
};

enum class WindowState {
		SHOULDCLOSE = 0,
		MINIMIZED,
		FOCUSED,
};


enum class RenderState {
	// OK :)
	OK,

	NEEDSSWAPCHAINUPDATE,
	//Can't do anything right now. Don't do anything.
	WAITING
};

class VulkanScaler {
public:
	VulkanContext& context;
	VulkanPhysicalDevice physicalDevice;
	VulkanDevice device;

	VulkanSwapchainImages swapchainImages;
	VulkanRenderPass renderpass;
	VulkanCommandPool commandPool;
	VulkanSwapchain swapchain;
	ShaderStore shaderStore;
	VulkanRenderer renderer;
	VkCommandBuffer commandBuffer;

	Sitter imageSitter;
	Sitter queueSitter;

	VulkanSemaphore imageAvailableSemaphore;
	VulkanSemaphore renderFinishedSemaphore;

	uint32_t imageIndex;

	VulkanScaler(VulkanContext& context_, VulkanPhysicalDevice physicalDevice_) :
		context(context_), physicalDevice(physicalDevice_),
		device(true, physicalDevice, context.surface, physicalDevice.queueFamilies),
		swapchainImages(physicalDevice, device, context.surface,
			context.glfwCtx.currentWidth, context.glfwCtx.currentHeight,
			device.graphicsFamily, device.presentFamily),
		renderpass(device, physicalDevice, swapchainImages.swapChainImageFormat),
		commandPool(device, device.graphicsFamily),
		swapchain(context.instance, physicalDevice, device, commandPool, device.graphicsQueue,
			device.presentQueue, renderpass, swapchainImages.swapChainImages,
			swapchainImages.swapChainImageFormat, swapchainImages.swapChainExtent),
		shaderStore(device),
		renderer(physicalDevice, device, device.graphicsFamily, device.graphicsQueue, shaderStore, swapchainImages.swapChainImageFormat, swapchainImages.swapChainExtent),

		commandBuffer(VK_NULL_HANDLE),

		imageSitter(device),
		queueSitter(device),
		imageAvailableSemaphore(device),
		renderFinishedSemaphore(device)

	{

	}

	VulkanRenderer& getRenderer() {
		return renderer;
	}

	VulkanRenderPass& getRenderPass() {
		return renderer.renderPass;
	}

	std::unique_ptr<VulkanRenderBuffer> newRenderBuffer () {
		return std::make_unique<VulkanRenderBuffer>(device, device.graphicsFamily);
	}

	WindowState getWindowState() {
		if(glfwWindowShouldClose(context.glfwCtx.window)) return WindowState::SHOULDCLOSE;
		return WindowState::FOCUSED;
	}

	bool renderingComplete(){
		if(!queueSitter.done()){
			queueSitter.wait();
		}

		return queueSitter.done();
	}

	void pollWindowEvents(){
		glfwPollEvents();
	}

	bool recreateSwapChain() {

		auto width = context.glfwCtx.currentWidth;
		auto height = context.glfwCtx.currentHeight;

		if(width && height){
			vkDeviceWaitIdle(device);
			vkQueueWaitIdle(device.graphicsQueue);
			vkQueueWaitIdle(device.presentQueue);

			swapchainImages.reCreate(width, height);
			swapchain.reCreate(swapchainImages.swapChainImages, swapchainImages.swapChainImageFormat, swapchainImages.swapChainExtent);
			renderer.reCreatePipeline(swapchainImages.swapChainExtent);

			return true;
		}

		return false;
	}

	RenderState drawAndPresent(VulkanRenderBuffer& buffer);


};

}

#endif // VKENGINE_H
