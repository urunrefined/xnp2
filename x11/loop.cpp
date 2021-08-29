
#include "vk/VKEngine.h"
#include "vk/VKPhysicalDeviceEnumerations.h"
#include "vk/FreeFont.h"

#include "np2.h"
#include "scrnmng.h"
#include "soundmng.h"
#include "keystat.h"
#include "loop.h"
#include "exception.h"
#include "pccore.h"


namespace BR {

static VulkanPhysicalDevice glPhysicalDeviceSelection(VulkanContext& engine){
	BR::VulkanPhysicalDeviceEnumerations physicalDeviceEnums(engine.instance);

	for(BR::VulkanPhysicalDevice& physicalDevice : physicalDeviceEnums.physicalDevices){
		//Use the first one available

		if(physicalDevice.isDeviceSuitable(engine.surface)){
			return physicalDevice;
		}
	}

	throw Exception("No graphics device suitable");
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
	{KeyButtons::KEY_ESC,          0x00},
	{KeyButtons::KEY_1,            0x01},
	{KeyButtons::KEY_2,            0x02},
	{KeyButtons::KEY_3,            0x03},
	{KeyButtons::KEY_4,            0x04},
	{KeyButtons::KEY_5,            0x05},
	{KeyButtons::KEY_6,            0x06},
	{KeyButtons::KEY_7,            0x07},
	{KeyButtons::KEY_8,            0x08},
	{KeyButtons::KEY_9,            0x09},
	{KeyButtons::KEY_0,            0x0a},
	{KeyButtons::KEY_ENTER,        0x1c},

	{KeyButtons::KEY_Q,            0x10},
	{KeyButtons::KEY_W,            0x11},
	{KeyButtons::KEY_E,            0x12},
	{KeyButtons::KEY_R,            0x13},
	{KeyButtons::KEY_T,            0x14},
	{KeyButtons::KEY_Y,            0x15},
	{KeyButtons::KEY_U,            0x16},
	{KeyButtons::KEY_I,            0x17},
	{KeyButtons::KEY_O,            0x18},
	{KeyButtons::KEY_P,            0x19},

	{KeyButtons::KEY_A,            0x1d},
	{KeyButtons::KEY_S,            0x1e},
	{KeyButtons::KEY_D,            0x1f},

	{KeyButtons::KEY_F,            0x20},
	{KeyButtons::KEY_G,            0x21},
	{KeyButtons::KEY_H,            0x22},
	{KeyButtons::KEY_J,            0x23},
	{KeyButtons::KEY_K,            0x24},
	{KeyButtons::KEY_L,            0x25},

	{KeyButtons::KEY_Z,            0x29},
	{KeyButtons::KEY_X,            0x2a},
	{KeyButtons::KEY_C,            0x2b},
	{KeyButtons::KEY_V,            0x2c},
	{KeyButtons::KEY_B,            0x2d},
	{KeyButtons::KEY_N,            0x2e},
	{KeyButtons::KEY_M,            0x2f},

	{KeyButtons::KEY_SPACE,	       0x34},
	{KeyButtons::KEY_RIGHT_ALT,    0x35},

	{KeyButtons::KEY_ARROW_UP,     0x3a},
	{KeyButtons::KEY_ARROW_LEFT,   0x3b},
	{KeyButtons::KEY_ARROW_RIGHT,  0x3c},
	{KeyButtons::KEY_ARROW_DOWN,   0x3d},

	{KeyButtons::KEY_NUMPAD_NUM,   0x3e},
	//HELP Key -- Apparently used to access the bios
	{KeyButtons::KEY_F11,          0x3f},
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
				//printf("res %d\n", mapping.res);
				keystat_keydown(mapping.res);
			}
			else {
				//printf("res %d\n", mapping.res);
				keystat_keyup(mapping.res);
			}
			break;
		}
	}
}

struct CallbackContext {
	VulkanTexture *texture;
	Input *input;
};

static const unsigned int pc98Width = 640;
static const unsigned int pc98Height = 400;

static void glLoop(
		SignalFD& sfd,
		VulkanContext& engine,
		VulkanPhysicalDevice& physicalDevice,
		HarfbuzzFont& hbfont,
		FreetypeFace& freetypeFace)
{
	VulkanScaler scaler(engine, physicalDevice);
	std::unique_ptr<VulkanRenderBuffer> renderBuffer;

	VulkanTexture mainTexture(
		scaler.device, physicalDevice, scaler.renderer.graphicsQueue,
		scaler.renderer.graphicsFamily, scaler.renderer.descriptorLayout,
		scaler.renderer.sampler, pc98Width, pc98Height
	);

	VulkanTexture logTexture(
		scaler.device, physicalDevice, scaler.renderer.graphicsQueue,
		scaler.renderer.graphicsFamily, scaler.renderer.descriptorLayout,
		scaler.renderer.sampler, 1024, 1024
	);

	Pen pen(logTexture.image, 0.2, freetypeFace, hbfont);

	{
		pen.draw("abc123 テスト");
	}

	{
		pen.draw("eat 食べる");
	}

	{
		pen.draw("pesto ペスト");
	}

	logTexture.textureDirty = true;

	enum class ViewPortMode : uint8_t {
		ASPECT = 0,
		STRETCH,
		INTEGER,
		END
	};

	int showlog = false;

	CallbackContext ctx{
		&mainTexture,
		&scaler.context.glfwCtx.input
	};

	ViewPortMode mode = ViewPortMode::INTEGER;

	while(scaler.getWindowState() != WindowState::SHOULDCLOSE && !sfd.isTriggered()){
		mainloop(&ctx);

		if(scaler.renderingComplete()){
			mainTexture.update();
			logTexture.update();

			renderBuffer = scaler.newRenderBuffer();
			scaler.pollWindowEvents();

			renderBuffer->begin(scaler.getRenderPass(), scaler.swapchain);
			//scaler.renderer.pipelineV->record(*renderBuffer, 6);

			if(showlog){
				scaler.renderer.pipelineAspect1to1->record(*renderBuffer, logTexture.descriptorSet, 6);
			}
			else {
				if(mode == ViewPortMode::ASPECT){
					scaler.renderer.pipelineAspect->record(*renderBuffer, mainTexture.descriptorSet, 6);
				}
				else if(mode == ViewPortMode::STRETCH){
					scaler.renderer.pipelineStretch->record(*renderBuffer, mainTexture.descriptorSet, 6);
				}
				else {
					scaler.renderer.pipelineInteger->record(*renderBuffer, mainTexture.descriptorSet, 6);
				}
			}

			renderBuffer->end();

			if(scaler.drawAndPresent(*renderBuffer) == RenderState::NEEDSSWAPCHAINUPDATE){
				scaler.recreateSwapChain();
			}
		}

		GLFWInput& input = engine.glfwCtx.getInput();

		if(input.getButton(KeyButtons::KEY_SUPER)){
			for(auto& keyEvent : input.keyEvents){
				if(keyEvent.key == KeyButtons::KEY_E && keyEvent.state == PRESSED){
					if(mode == ViewPortMode::ASPECT){
						mode = ViewPortMode::STRETCH;
					}else if(mode == ViewPortMode::STRETCH){
						mode = ViewPortMode::INTEGER;
					}else if(mode == ViewPortMode::INTEGER){
						mode = ViewPortMode::ASPECT;
					}
				}

				if(keyEvent.key == KeyButtons::KEY_NUMPAD_DIV && keyEvent.state == PRESSED){
					soundmng_decreaseVol(0.05);
				}

				if(keyEvent.key == KeyButtons::KEY_NUMPAD_MULT && keyEvent.state == PRESSED){
					soundmng_increaseVol(0.05);
				}

				if(keyEvent.key == KeyButtons::KEY_M && keyEvent.state == PRESSED){
					soundmng_toggleMute();
				}

				if(keyEvent.key == KeyButtons::KEY_I && keyEvent.state == PRESSED){
					pccore_reset();
				}

				if(keyEvent.key == KeyButtons::KEY_K && keyEvent.state == PRESSED){
					showlog = !showlog;
				}
			}
		}
		else {
			for(auto& keyEvent : input.keyEvents){
				mapAndSendKey(keyEvent);
				//kbdTest();
			}
		}

		input.reset();
	}

	while(!scaler.renderingComplete()){
		usleep(1000);
	}
}

void loop(SignalFD& sfd){
	FontconfigLib lib;
	FontList fontlist(lib);
	std::string fontfile = fontlist.getFirst();

	FreetypeLib freetypeLib;
	FreetypeFace freetypeFace(freetypeLib, fontfile.c_str(), 48);

	HarfbuzzBlob hbblob(fontfile.c_str());
	HarfbuzzFace hbface(hbblob.blob);
	HarfbuzzFont hbfont(hbface.face);

	VulkanContext engine;
	VulkanPhysicalDevice physicalDevice = glPhysicalDeviceSelection(engine);

	glLoop(sfd, engine, physicalDevice, hbfont, freetypeFace);
}

}

extern "C" UINT8 mousemng_getstat(void *inContext, short *x, short *y){

	BR::CallbackContext *context = (BR::CallbackContext *)inContext;

	auto& input = *(context->input);

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

SCRNSURF scrnmng_surflock(void *inContext){
	BR::CallbackContext *context = (BR::CallbackContext *)inContext;

	SCRNSURF scrnsurf;

	scrnsurf.ptr = (UINT8 *) context->texture->image.data.data();
	scrnsurf.bpp = 32;
	scrnsurf.width = context->texture->image.width;
	scrnsurf.height = context->texture->image.height;
	scrnsurf.xalign = 4;
	scrnsurf.yalign = context->texture->image.width * 4;

	return scrnsurf;
}

void scrnmng_surfunlock(void *inContext){
	BR::CallbackContext *context = (BR::CallbackContext *)inContext;
	context->texture->textureDirty = true;
}


