#include "VKSync.h"
#include <assert.h>
#include <stdexcept>

namespace BR {

VulkanSemaphore::VulkanSemaphore(const VkDevice &device_) : device(device_){
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS){
		throw std::runtime_error("failed to create semaphore!");
	}
}

VulkanSemaphore::~VulkanSemaphore(){
	vkDestroySemaphore(device, semaphore, nullptr);
}

VulkanFence::VulkanFence(const VkDevice &device_) : device(device_)
{
	VkFenceCreateInfo fenceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0
	};

	if(vkCreateFence(device, &fenceCreateInfo, nullptr, &fence) != VK_SUCCESS){
		throw std::runtime_error("failed to create fence!");
	}
}

VulkanFence::~VulkanFence(){
	vkDestroyFence(device, fence, nullptr);
}

VkResult VulkanFence::wait(){
	return vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
}

Sitter::Sitter(const VkDevice &device_) : device(device_)
{
	assert(state == SitterState::INACTIVE);

	VkFenceCreateInfo fenceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0
	};

	if(vkCreateFence(device, &fenceCreateInfo, nullptr, &fence) != VK_SUCCESS){
		throw std::runtime_error("failed to create fence!");
	}
}

void Sitter::activate(){
	assert(state != SitterState::ACTIVE);
	state = SitterState::ACTIVE;
}

void Sitter::cancel(){
	assert(state == SitterState::ACTIVE);
	state = SitterState::INACTIVE;
}

bool Sitter::done(){
	return state == SitterState::INACTIVE;
}

void Sitter::block(uint64_t nanoseconds){
	assert(state == SitterState::ACTIVE);

	VkResult result = vkWaitForFences(device, 1, &fence, VK_FALSE, nanoseconds);

	if(result == VK_SUCCESS){
		vkResetFences(device, 1, &fence);
		state = SitterState::INACTIVE;
	}
	else if(result == VK_TIMEOUT){
		//Nothing
	}
	else{
		//TODO: Something something something. What to do?
		printf("Error vkWaitForFences result %d\n", result);
		state = SitterState::INACTIVE;
	}
}

void Sitter::block(){
	assert(state == SitterState::ACTIVE);

	VkResult result = vkWaitForFences(device, 1, &fence, VK_FALSE, UINT64_MAX);

	assert(result != VK_TIMEOUT);

	if(result == VK_SUCCESS){
		vkResetFences(device, 1, &fence);
		state = SitterState::INACTIVE;
	}
	else{
		//TODO: Something something something. What to do?
		printf("Error vkWaitForFences result %d\n", result);
		state = SitterState::INACTIVE;
	}
}

void Sitter::wait(){
	assert(state == SitterState::ACTIVE);

	VkResult result = vkWaitForFences(device, 1, &fence, VK_FALSE, 0);

	if(result == VK_SUCCESS){
		vkResetFences(device, 1, &fence);
		state = SitterState::INACTIVE;
	}
	else if(result == VK_TIMEOUT){
		//Do nothing
	}
	else{
		//TODO: Something something something. What to do?
		state = SitterState::INACTIVE;
	}
}

Sitter::~Sitter(){
	if(state == SitterState::ACTIVE)
		vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
	vkDestroyFence(device, fence, nullptr);
}

}
