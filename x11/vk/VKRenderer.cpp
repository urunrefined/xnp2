#include "VKRenderer.h"
#include "VKBuffers.h"
#include "VKCommandBuffer.h"

#include <string.h>

namespace BR {

void transitionImageLayout(const VkDevice& device, const VkCommandPool& commandPool, const VkQueue& graphicsQueue, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout) {
	VulkanCommandBuffer commandBuffer(device, commandPool, graphicsQueue);

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	} else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	} else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	commandBuffer.submit();
}

void copyBufferToImage(const VkDevice device, const VkCommandPool& commandPool, VkQueue& graphicsQueue, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
	VulkanCommandBuffer commandBuffer(device, commandPool, graphicsQueue);

	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = {0, 0, 0};
	region.imageExtent = {
		width,
		height,
		1
	};

	vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	commandBuffer.submit();
}

void VulkanRenderer::updateImage(){
	VulkanBufferGeneric stagingBuffer(device, physicalDevice, sizeof(image.data), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* data;
	vkMapMemory(device, stagingBuffer, 0, sizeof(image.data), 0, &data);
		memcpy(data, image.data, sizeof(image.data));
	vkUnmapMemory(device, stagingBuffer);

	transitionImageLayout(device, commandPool, graphicsQueue, texture, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(device, commandPool, graphicsQueue, stagingBuffer, texture, 640, 400);
	transitionImageLayout(device, commandPool, graphicsQueue, texture, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

VulkanRenderer::VulkanRenderer(
		const VulkanPhysicalDevice& physicalDevice_, const VulkanDevice& device_,
		uint32_t graphicsFamily_, VkQueue& graphicsQueue_, ShaderStore& shaderStore_,
		VkFormat swapChainFormat, VkExtent2D swapChainExtent, VkImage& texture_)
	: physicalDevice(physicalDevice_), device(device_), shaderStore(shaderStore_),
		renderPass(device, physicalDevice, swapChainFormat),
		//descriptorCommandPool(device, device.graphicsFamily)
		texture(texture_),
		graphicsFamily(graphicsFamily_),
		graphicsQueue(graphicsQueue_),

		commandPool(device, graphicsFamily),

		textureView(device, texture, VK_FORMAT_B8G8R8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT),
		sampler(physicalDevice, device),

		descriptorLayout(device),
		descriptorPool(device),
		descriptorSet(device, textureView, sampler, descriptorPool, descriptorLayout){
	
	reCreatePipeline(swapChainExtent);

	updateImage();
}
	
} //namespace D5