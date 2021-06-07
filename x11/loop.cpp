
#include "vk/VKEngine.h"
#include "vk/VKPhysicalDeviceEnumerations.h"

#include "np2.h"
#include "scrnmng.h"
#include "keystat.h"

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

class KeyMapping {
public:
	KeyButtons key;
	uint8_t res;
};

static KeyMapping keyTable [] {
	{KeyButtons::KEY_ESC,         0x00},
	{KeyButtons::KEY_1,           0x01},
	{KeyButtons::KEY_2,           0x02},
	{KeyButtons::KEY_3,           0x03},
	{KeyButtons::KEY_4,           0x04},
	{KeyButtons::KEY_5,           0x05},
	{KeyButtons::KEY_6,           0x06},
	{KeyButtons::KEY_7,           0x07},
	{KeyButtons::KEY_8,           0x08},
	{KeyButtons::KEY_9,           0x09},
	{KeyButtons::KEY_0,           0x0A},
	{KeyButtons::KEY_ENTER,       0x1C},
	{KeyButtons::KEY_ARROW_UP,    0x3A},
	{KeyButtons::KEY_ARROW_LEFT,  0x3B},
	{KeyButtons::KEY_ARROW_RIGHT, 0x3C},
	{KeyButtons::KEY_ARROW_DOWN,  0x3D},
};

//static void kbdTest(GLFWInput& input){
//	for(auto& keyEvent : input.keyEvents){
//		static int i = 1;
//
//		if(keyEvent.key == KeyButtons::KEY_ENTER && keyEvent.state == PRESSED){
//			keystat_keydown(i);
//		}
//		else if(keyEvent.key == KeyButtons::KEY_ENTER && keyEvent.state == RELEASED){
//			keystat_keyup(i);
//		}
//
//		i++;
//	}
//}

static void mapAndSendKey(KeyEvent& keyEvent){
	for(KeyMapping& mapping : keyTable){
		if(mapping.key == keyEvent.key){
			if(keyEvent.state== PRESSED){
				keystat_keydown(mapping.res);
			}
			else {
				keystat_keyup(mapping.res);
			}
			break;
		}
	}
}

static void glLoop(VulkanContext& engine, VulkanPhysicalDevice& physicalDevice){
	BR::VulkanScaler scaler(engine, physicalDevice);
	std::unique_ptr<BR::VulkanRenderBuffer> renderBuffer;


	enum class ViewPortMode : uint8_t {
		ASPECT = 0,
		STRETCH,
		INTEGER,
		END
	};

	ViewPortMode mode = ViewPortMode::INTEGER;

	while(scaler.getWindowState() != WindowState::SHOULDCLOSE){
		mainloop(&scaler);

		if(scaler.renderingComplete()){
			scaler.renderer.updateImage();

			renderBuffer = scaler.newRenderBuffer();
			scaler.pollWindowEvents();

			renderBuffer->begin(scaler.getRenderPass(), scaler.swapchain);
			//scaler.renderer.pipelineV->record(*renderBuffer, 6);

			if(mode == ViewPortMode::ASPECT){
				scaler.renderer.pipelineAspect->record(*renderBuffer, scaler.renderer.descriptorSet, 6);
			}
			else if(mode == ViewPortMode::STRETCH){
				scaler.renderer.pipelineStretch->record(*renderBuffer, scaler.renderer.descriptorSet, 6);
			}
			else {
				scaler.renderer.pipelineInteger->record(*renderBuffer, scaler.renderer.descriptorSet, 6);
			}

			renderBuffer->end();

			if(scaler.drawAndPresent(*renderBuffer) == RenderState::NEEDSSWAPCHAINUPDATE){
				scaler.recreateSwapChain();
			}
		}

		GLFWInput& input = engine.glfwCtx.getInput();

		for(auto& keyEvent : input.keyEvents){
			mapAndSendKey(keyEvent);

			if(keyEvent.key == KeyButtons::KEY_E && keyEvent.state == PRESSED &&
					input.getButton(KeyButtons::KEY_SUPER)){
				if(mode == ViewPortMode::ASPECT){
					mode = ViewPortMode::STRETCH;
				}else if(mode == ViewPortMode::STRETCH){
					mode = ViewPortMode::INTEGER;
				}else if(mode == ViewPortMode::INTEGER){
					mode = ViewPortMode::ASPECT;
				}
			}

			//kbdTest();
		}

		input.reset();
		//input.resetFrame();
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

//compat to gtk version -- Remove eventually
extern "C" void mouse_running (UINT8 flg){
	//printf("mouse_running\n");

	(void) flg;
}

extern "C" UINT8 mousemng_getstat(short *x, short *y, int clear){
	//printf("mouse_getstat\n");

	*x = 0;
	*y = 0;

	(void) clear;

	return 0;
}

extern "C" void mousemng_callback(void){

}

//TODO: Remove
static SCRNMNG scrnmng {
	0,32,0
};

extern "C" SCRNMNG *scrnmngp = &scrnmng;

//TODO: Remove
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


