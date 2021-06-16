
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

//PC-9801 keyboard layout (more or less)
// STOP  COPY    F1  F2  F3  F4  F5  F6  F7  F8  F9  F10  (RU  RD)*
//
// ESC       1  2  3  4  5  6  7  8  9  0  -  ^  ¥  BS     INS  DEL    HOME HELP   -    /
//    TAB      Q  W  E  R  T  Y  U  I  O  P  @  [  ||||||| (       )*    7    8    9    *
// CTRL CAPS     A  S  D  F  G  H  J  K  L  ;  : ] |ENTER|     UP        4    5    6    +
//  --SHIFT--      Z  X  C  V  B  N  M  <  >  ? --  SHIFT   LT    RT     1    2    3    =
//        KANA  GRPH (**) ----------SPACE---------- XFER       DN        0    ,    .   RET
//
//
// * On some layouts Roll Up Down are replaced with vf1 vf2 vf3 vf4 vf5 and is instead
// placed under INS and DEL
// ** On some layouts there was also an NFER Key here.
// -- left of shift is actually the long '-'


class KeyMapping {
public:
	KeyButtons key;
	uint8_t res;
};

//NKEY_KANA			= 0x72,
//NKEY_GRPH			= 0x73,

// we are using keycodes here, not scancodes as we want to replicate
// the original keyboard (not completely possible)

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

	{KeyButtons::KEY_SPACE,	      0x34},
	{KeyButtons::KEY_RIGHT_ALT,   0x35},

	{KeyButtons::KEY_ARROW_UP,    0x3A},
	{KeyButtons::KEY_ARROW_LEFT,  0x3B},
	{KeyButtons::KEY_ARROW_RIGHT, 0x3C},
	{KeyButtons::KEY_ARROW_DOWN,  0x3D},

	{KeyButtons::KEY_NUMPAD_NUM,   0x3e},
	{KeyButtons::KEY_NUMPAD_MINUS, 0x40},
	{KeyButtons::KEY_NUMPAD_DIV,   0x41},
	{KeyButtons::KEY_NUM_7,        0x42},
	{KeyButtons::KEY_NUM_8,        0x43},
	{KeyButtons::KEY_NUM_9,        0x44},
	{KeyButtons::KEY_NUMPAD_MULT,  0x45},
	{KeyButtons::KEY_NUM_4,        0x46},
	{KeyButtons::KEY_NUM_5,        0x47},
	{KeyButtons::KEY_NUM_6,        0x48},
	{KeyButtons::KEY_NUMPAD_PLUS,  0x49},
	{KeyButtons::KEY_NUM_1,        0x4a},
	{KeyButtons::KEY_NUM_2,        0x4b},
	{KeyButtons::KEY_NUM_3,        0x4c},
	{KeyButtons::KEY_NUMPAD_ENTER, 0x4d},
	{KeyButtons::KEY_NUM_0,        0x4e},
	{KeyButtons::KEY_NUMPAD_COMMA, 0x4f},

	{KeyButtons::KEY_LEFT_SHIFT,   0x70},
	{KeyButtons::KEY_LEFT_CTRL,    0x74},
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
				printf("res %d\n", mapping.res);
				keystat_keydown(mapping.res);
			}
			else {
				printf("res %d\n", mapping.res);
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
	printf("mouse_running %d\n", flg);
}

extern "C" UINT8 mousemng_getstat(void *graphics, short *x, short *y){

	BR::VulkanScaler *scaler = (BR::VulkanScaler *)graphics;

	auto& input = scaler->context.glfwCtx.getInput();

	input.getMouseMove(*x, *y);
	input.mouse_move_x = 0;
	input.mouse_move_y = 0;

	UINT8 mouseButtonState = 0;

	//The entire logic here seems very strange....
	//Needs to be revisited
	if(input.getButton(BR::MouseButtons::BUTTON_LEFT)){
		mouseButtonState &= 0x7f;
	}
	else {
		mouseButtonState |= 0x80;
	}

	if(input.getButton(BR::MouseButtons::BUTTON_RIGHT)){
		mouseButtonState &= 0xdf;
	}
	else {
		mouseButtonState |= 0x20;
	}

	return mouseButtonState;
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


