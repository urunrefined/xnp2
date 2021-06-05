#ifndef VKGLFW_H_
#define VKGLFW_H_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Input.h"


namespace BR {
	
class GLFWInput : public Input {
public:
	GLFWwindow* window;
	bool shouldClose;
	
	void pollEvents(){
		glfwPollEvents();
		
		if(glfwWindowShouldClose(window)){
			shouldClose = true;
		}
	};

	GLFWInput() : window(nullptr), shouldClose(false){}
	virtual ~GLFWInput(){}
};

class GLFWContext {
public:
	uint32_t currentWidth;
	uint32_t currentHeight;
	
	uint32_t xOffset;
	uint32_t yOffset;
	
	uint32_t fWidth;
	uint32_t fheight;
	
	uint32_t requestedWindowWidth;
	uint32_t requestedWindowHeight;

	GLFWwindow* window;
	
	GLFWContext (uint32_t width_, uint32_t height_);
	~GLFWContext ();
	
	GLFWInput input;
	
	GLFWInput& getInput();
	
};

}

#endif
