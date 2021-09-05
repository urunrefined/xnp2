#ifndef VKDESCRIPTORPOOLEXT_H
#define VKDESCRIPTORPOOLEXT_H

#include <vulkan/vulkan.h>

namespace BR {

class VKDescriptorPoolExt
{
	const VkDevice& device;
	VkDescriptorPool descriptorPool;

public:
	VKDescriptorPoolExt(const VkDevice& device_);

	~VKDescriptorPoolExt();

	operator const VkDescriptorPool& () const{
		return descriptorPool;
	}
};

}

#endif // VKDESCRIPTORPOOLEXT_H
