#ifndef VKSYNC_H
#define VKSYNC_H

#include <vulkan/vulkan.h>

namespace BR {

class VulkanSemaphore {
public:
	const VkDevice& device;
	VkSemaphore semaphore;

	VulkanSemaphore(const VkDevice& device_);

	~VulkanSemaphore();

	operator VkSemaphore& (){
		return semaphore;
	}

	operator VkSemaphore* (){
		return &semaphore;
	}
};

class VulkanFence {
public:
	const VkDevice& device;
	VkFence fence;

	VulkanFence(const VkDevice& device_);
	~VulkanFence();

	operator VkFence& (){
		return fence;
	}

	VkResult wait();
};

enum class SitterState {
	INIT,
	INACTIVE = 0,
	ACTIVE
};

class Sitter {
public:
	const VkDevice& device;
	VkFence fence;
	SitterState state = SitterState::INACTIVE;

	Sitter(const VkDevice& device_);
	~Sitter();

	void activate();
	void cancel();

	bool done ();

	void block(uint64_t nanoseconds);
	void block();
	void wait();

	operator VkFence& (){
		return fence;
	}
};

}

#endif // VKSYNC_H
