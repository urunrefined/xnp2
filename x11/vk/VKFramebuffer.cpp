#include "VKFramebuffer.h"

#include <stdexcept>

namespace BR {

VulkanFramebuffer::VulkanFramebuffer(const VkDevice &device_, VkExtent2D swapChainExtent, VkRenderPass &renderPass, std::array<VkImageView, 2> &attachments)
		: device(device_){
	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderPass;
	framebufferInfo.attachmentCount = (uint32_t)attachments.size();
	framebufferInfo.pAttachments = attachments.data();
	framebufferInfo.width = swapChainExtent.width;
	framebufferInfo.height = swapChainExtent.height;
	framebufferInfo.layers = 1;

	if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create framebuffer!");
	}
}

VulkanFramebuffer::VulkanFramebuffer(const VkDevice& device_, VkExtent2D extent, VkRenderPass& renderPass, VkImageView& attachment) : device(device_){
	VkFramebufferCreateInfo fb_info;
	fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	fb_info.pNext = NULL;
	fb_info.renderPass = renderPass;
	fb_info.attachmentCount = 1;
	fb_info.pAttachments = &attachment;
	fb_info.width = extent.width;
	fb_info.height = extent.height;
	fb_info.layers = 1;
	fb_info.flags = 0;

	vkCreateFramebuffer(device, &fb_info, NULL, &framebuffer);
}

VulkanFramebuffer::~VulkanFramebuffer(){
	vkDestroyFramebuffer(device, framebuffer, nullptr);
}

}
