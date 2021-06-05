#ifndef VKINSTANCE_H
#define VKINSTANCE_H

#include <vulkan/vulkan.h>

namespace BR {

class VulkanInstance {
public:
	VkInstance instance;

	VulkanInstance(bool enableValidationLayers);

	~VulkanInstance();

	operator VkInstance& (){
		return instance;
	}
};

}

#endif // VKINSTANCE_H
