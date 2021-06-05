
#include "vk/VKEngine.h"
#include "vk/VKPhysicalDeviceEnumerations.h"

#include "np2.h"
#include "scrnmng.h"

namespace BR {

static VulkanPhysicalDevice glPhysicalDeviceSelection(VulkanContext& engine){
	BR::VulkanPhysicalDeviceEnumerations physicalDeviceEnums(engine.instance);

	for(BR::VulkanPhysicalDevice& physicalDevice : physicalDeviceEnums.physicalDevices){
		//Use the first one available

		if(physicalDevice.isDeviceSuitable(engine.surface)){
			return physicalDevice;
		}
		else {
			printf("Not suitable\n");
		}
	}

	printf("No graphics device suitable\n");
	throw -2;
}

static void glLoop(VulkanContext& engine, VulkanPhysicalDevice& physicalDevice){
	BR::VulkanScaler scaler(engine, physicalDevice);
	std::unique_ptr<BR::VulkanRenderBuffer> renderBuffer;

	while(scaler.getWindowState() != WindowState::SHOULDCLOSE){
		mainloop(&scaler);

		if(scaler.renderingComplete()){
			scaler.renderer.updateImage();

			renderBuffer = scaler.newRenderBuffer();
			scaler.pollWindowEvents();

			renderBuffer->begin(scaler.getRenderPass(), scaler.swapchain);
			//scaler.renderer.pipelineV->record(*renderBuffer, 6);
			scaler.renderer.pipelineTex->record(*renderBuffer, scaler.renderer.descriptorSet, 6);
			renderBuffer->end();

			if(scaler.drawAndPresent(*renderBuffer) == RenderState::NEEDSSWAPCHAINUPDATE){
				scaler.recreateSwapChain();
			}
		}
	}

	while(!scaler.renderingComplete()){
		usleep(1000);
	}
}

void loop(){
	VulkanContext engine;

	VulkanPhysicalDevice physicalDevice = glPhysicalDeviceSelection(engine);

	glLoop(engine, physicalDevice);
}

}

static SCRNSURF scrnsurf;

const SCRNSURF *
scrnmng_surflock(void *graphics){
	BR::VulkanScaler *scaler = (BR::VulkanScaler *)graphics;

	scrnsurf.ptr = (UINT8 *) scaler->renderer.image.data;
	scrnsurf.bpp = 4 * 8;
	scrnsurf.width = 640;
	scrnsurf.height = 400;
	scrnsurf.xalign = 4;
	scrnsurf.yalign = 640 * 4;

	return &scrnsurf;
}

void
scrnmng_surfunlock(void *graphics, const SCRNSURF *surf){
	(void) surf;

	BR::VulkanScaler *scaler = (BR::VulkanScaler *)graphics;
	scaler->renderer.updateImage();
}


