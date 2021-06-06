#include "Input.h"
#include "VKGLFW.h"

#include <math.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>

namespace BR {

static bool isFocused = false;

static void onWindowResized(GLFWwindow* window, unsigned int width, unsigned int height) {
	GLFWContext *ctx = (GLFWContext *)glfwGetWindowUserPointer(window);

	if(height == 0) height = 1;

	ctx->currentHeight = height;
	ctx->currentWidth = width;
}

static void onWindowResized(GLFWwindow* window, int width, int height) {
	onWindowResized(window, (unsigned int)width, (unsigned int)height);
}

struct KeyMapping {
	int glfwKey;
	KeyButtons inputKey;
};

struct MouseMapping {
	int glfwKey;
	MouseButtons inputKey;
};

static KeyMapping keymap[] {
	{GLFW_KEY_LEFT_SHIFT, KeyButtons::KEY_LEFT_SHIFT}, {GLFW_KEY_LEFT_CONTROL, KeyButtons::KEY_LEFT_CTRL}, {GLFW_KEY_LEFT_ALT, KeyButtons::KEY_LEFT_ALT},
	{GLFW_KEY_TAB, KeyButtons::KEY_TAB}, {GLFW_KEY_ENTER, KeyButtons::KEY_ENTER},

	{GLFW_KEY_A, KeyButtons::KEY_A}, {GLFW_KEY_B, KeyButtons::KEY_B}, {GLFW_KEY_C, KeyButtons::KEY_C}, {GLFW_KEY_D, KeyButtons::KEY_D}, {GLFW_KEY_E, KeyButtons::KEY_E},
	{GLFW_KEY_F, KeyButtons::KEY_F}, {GLFW_KEY_G, KeyButtons::KEY_G}, {GLFW_KEY_H, KeyButtons::KEY_H}, {GLFW_KEY_I, KeyButtons::KEY_I}, {GLFW_KEY_J, KeyButtons::KEY_J},
	{GLFW_KEY_K, KeyButtons::KEY_K}, {GLFW_KEY_L, KeyButtons::KEY_L}, {GLFW_KEY_M, KeyButtons::KEY_M}, {GLFW_KEY_N, KeyButtons::KEY_N}, {GLFW_KEY_O, KeyButtons::KEY_O},
	{GLFW_KEY_P, KeyButtons::KEY_P}, {GLFW_KEY_Q, KeyButtons::KEY_Q}, {GLFW_KEY_R, KeyButtons::KEY_R}, {GLFW_KEY_S, KeyButtons::KEY_S}, {GLFW_KEY_T, KeyButtons::KEY_T},
	{GLFW_KEY_U, KeyButtons::KEY_U}, {GLFW_KEY_V, KeyButtons::KEY_V}, {GLFW_KEY_W, KeyButtons::KEY_W}, {GLFW_KEY_X, KeyButtons::KEY_X}, {GLFW_KEY_Y, KeyButtons::KEY_Y},
	{GLFW_KEY_Z, KeyButtons::KEY_Z},

	{GLFW_KEY_0, KeyButtons::KEY_0}, {GLFW_KEY_1, KeyButtons::KEY_1}, {GLFW_KEY_2, KeyButtons::KEY_2}, {GLFW_KEY_3, KeyButtons::KEY_3}, {GLFW_KEY_4, KeyButtons::KEY_4},
	{GLFW_KEY_5, KeyButtons::KEY_5}, {GLFW_KEY_6, KeyButtons::KEY_6}, {GLFW_KEY_7, KeyButtons::KEY_7}, {GLFW_KEY_8, KeyButtons::KEY_8}, {GLFW_KEY_9, KeyButtons::KEY_9},

	{GLFW_KEY_KP_0, KeyButtons::KEY_NUM_0}, {GLFW_KEY_KP_1, KeyButtons::KEY_NUM_1}, {GLFW_KEY_KP_2, KeyButtons::KEY_NUM_2}, {GLFW_KEY_KP_3, KeyButtons::KEY_NUM_3}, {GLFW_KEY_KP_4, KeyButtons::KEY_NUM_4},
	{GLFW_KEY_KP_5, KeyButtons::KEY_NUM_5}, {GLFW_KEY_KP_6, KeyButtons::KEY_NUM_6}, {GLFW_KEY_KP_7, KeyButtons::KEY_NUM_7}, {GLFW_KEY_KP_8, KeyButtons::KEY_NUM_8}, {GLFW_KEY_KP_9, KeyButtons::KEY_NUM_9},

	{GLFW_KEY_F1, KeyButtons::KEY_F1}, {GLFW_KEY_F2,  KeyButtons::KEY_F2},  {GLFW_KEY_F3,  KeyButtons::KEY_F3},  {GLFW_KEY_F4,  KeyButtons::KEY_F4},
	{GLFW_KEY_F5, KeyButtons::KEY_F5}, {GLFW_KEY_F6,  KeyButtons::KEY_F6},  {GLFW_KEY_F7,  KeyButtons::KEY_F7},  {GLFW_KEY_F8,  KeyButtons::KEY_F8},
	{GLFW_KEY_F9, KeyButtons::KEY_F9}, {GLFW_KEY_F10, KeyButtons::KEY_F10}, {GLFW_KEY_F11, KeyButtons::KEY_F11}, {GLFW_KEY_F12, KeyButtons::KEY_F12},

	{GLFW_KEY_SPACE, KeyButtons::KEY_SPACE}, {GLFW_KEY_BACKSPACE, KeyButtons::KEY_BACKSPACE}, {GLFW_KEY_DELETE, KeyButtons::KEY_DELETE},
	{GLFW_KEY_HOME, KeyButtons::KEY_HOME},   {GLFW_KEY_END, KeyButtons::KEY_END},             {GLFW_KEY_INSERT, KeyButtons::KEY_INSERT},

	{GLFW_KEY_KP_ADD, KeyButtons::KEY_NUMPAD_PLUS}, {GLFW_KEY_KP_SUBTRACT, KeyButtons::KEY_NUMPAD_MINUS},
	{GLFW_KEY_LEFT, KeyButtons::KEY_ARROW_LEFT},    {GLFW_KEY_UP, KeyButtons::KEY_ARROW_UP},
	{GLFW_KEY_RIGHT, KeyButtons::KEY_ARROW_RIGHT},  {GLFW_KEY_DOWN, KeyButtons::KEY_ARROW_DOWN},
	{GLFW_KEY_EQUAL, KeyButtons::KEY_EQUAL}, {GLFW_KEY_MINUS, KeyButtons::KEY_MINUS}, {GLFW_KEY_PERIOD, KeyButtons::KEY_PERIOD},
	{GLFW_KEY_ESCAPE, KeyButtons::KEY_ESC}
};

static MouseMapping mousemap[] {
	{GLFW_MOUSE_BUTTON_LEFT, MouseButtons::BUTTON_LEFT}, {GLFW_MOUSE_BUTTON_RIGHT, MouseButtons::BUTTON_RIGHT}, {GLFW_MOUSE_BUTTON_MIDDLE, MouseButtons::BUTTON_MIDDLE}
};


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
	(void) scancode;
	(void) window;
	(void) mods;

	if(!isFocused) return;

	GLFWContext *ctx = (GLFWContext *)glfwGetWindowUserPointer(window);
	GLFWInput& input = ctx->getInput();

	if(action == GLFW_PRESS){

		for(KeyMapping& keyPair : keymap){
			if(keyPair.glfwKey == key){
				input.pressKey(keyPair.inputKey);
				break;
			}
		}

		/*TODO: Move outside of this module into an upper layer*/

		if (key == GLFW_KEY_F12) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			isFocused = false;
		}

		if (key == GLFW_KEY_F4 && mods == GLFW_MOD_ALT){
			input.shouldClose = true;
		}

		if(key == GLFW_KEY_ENTER && mods == GLFW_MOD_ALT){

			//glfwGetWindowMonitor returns a pointer if the window is
			//fullscreen on a monitor
			if(glfwGetWindowMonitor(window)){
				//Fullscreen -> Windowed
				glfwSetWindowMonitor(window, nullptr, 0, 0, 1600, 900, GLFW_DONT_CARE);
			}else{
				//Windowed -> Fullscreen
				GLFWmonitor *monitor = glfwGetPrimaryMonitor();
				const GLFWvidmode* mode = glfwGetVideoMode(monitor);
				glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);

			}
		}

	}
	else if(action == GLFW_RELEASE){
		for(KeyMapping& keyPair : keymap){
			if(keyPair.glfwKey == key){
				input.letgoKey(keyPair.inputKey);
				break;
			}
		}
	}
}

static void mouse_move_callback(GLFWwindow* window, double x, double y){
	(void) window;

	if(!isFocused) return;

	GLFWContext *ctx = (GLFWContext *)glfwGetWindowUserPointer(window);
	Input& input = ctx->getInput();

	input.updateMouse((unsigned int)x,(unsigned int)y);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
	(void) mods;

	GLFWContext *ctx = (GLFWContext *)glfwGetWindowUserPointer(window);
	Input& input = ctx->getInput();

	if(isFocused){

		if(action == GLFW_PRESS){
			for(MouseMapping& buttonPair : mousemap){
				if(buttonPair.glfwKey == button){
					input.pressButton(buttonPair.inputKey);
					break;
				}
			}
		}else if(action == GLFW_RELEASE){
			for(MouseMapping& buttonPair : mousemap){
				if(buttonPair.glfwKey == button){
					input.letgoButton(buttonPair.inputKey);
					break;
				}
			}
		}
	}
	else{
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			isFocused = true;
		}
	}


}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
	(void) window;
	(void) xoffset;

	GLFWContext *ctx = (GLFWContext *)glfwGetWindowUserPointer(window);
	Input& input = ctx->getInput();

	if((float)yoffset > 0.0f){
		input.pressButton(MouseButtons::SCROLL_UP);
	}
	else if((float)yoffset < 0.0f){
		input.pressButton(MouseButtons::SCROLL_DOWN);
	}
}

GLFWContext::GLFWContext (uint32_t surfaceWidth, uint32_t surfaceHeight) : currentWidth(surfaceWidth), currentHeight(surfaceHeight){
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	assert((int) surfaceHeight > 0);
	assert((int) surfaceWidth > 0);

	window = glfwCreateWindow((int)surfaceWidth, (int)surfaceHeight, "Vulkan", nullptr, nullptr);

	if(!window){
		throw std::runtime_error("Could not create GLFW window!");
	}

	glfwSetWindowUserPointer(window, this);

	glfwSetWindowSizeCallback(window, onWindowResized);
	glfwSetCursorPosCallback(window, mouse_move_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);

	onWindowResized(window, surfaceWidth, surfaceHeight);

	input.window = window;
}

GLFWContext::~GLFWContext (){
	glfwDestroyWindow(window);
	glfwTerminate();
}

GLFWInput& GLFWContext::getInput (){
	return input;
}

}
