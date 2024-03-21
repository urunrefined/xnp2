#include "VKTexture.h"
#include "VKBuffers.h"
#include "VKCommandBuffer.h"
#include <string.h>

namespace BR {

static void transitionImageLayout(const VkDevice &device,
                                  const VkCommandPool &commandPool,
                                  const VkQueue &graphicsQueue, VkImage image,
                                  VkImageLayout oldLayout,
                                  VkImageLayout newLayout) {
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

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
        newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
               newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0,
                         nullptr, 0, nullptr, 1, &barrier);

    commandBuffer.submit();
}

static void copyBufferToImage(const VkDevice device,
                              const VkCommandPool &commandPool,
                              VkQueue &graphicsQueue, VkBuffer buffer,
                              VkImage image, uint32_t width, uint32_t height) {
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
    region.imageExtent = {width, height, 1};

    vkCmdCopyBufferToImage(commandBuffer, buffer, image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    commandBuffer.submit();
}

static bool lineHasData(const unsigned char *data, unsigned int width) {
    for (unsigned int x = 0; x < width * 4; x++) {
        if (data[x])
            return true;
    }

    return false;
}

static void doubleBlankLines(uint16_t width, uint16_t height,
                             const unsigned char *in, unsigned char *out) {
    // Always copy the first line
    memcpy(out, in, width * 4);

    for (uint16_t h = 1; h < height; h++) {
        if (!lineHasData(in + h * width * 4, width)) {

            memcpy(out + (h)*width * 4, in + (h - 1) * width * 4, width * 4);
        } else {
            memcpy(out + (h)*width * 4, in + (h)*width * 4, width * 4);
        }
    }
}

void VulkanTexture::update(DoubleLines doubleLines) {
    if (textureDirty) {
        void *data;
        vkMapMemory(device, stagingBuffer, 0, image.data.size(), 0, &data);
        if (doubleLines == DoubleLines::NO) {
            memcpy(data, image.data.data(), image.data.size());
        } else {
            doubleBlankLines(image.width, image.height, image.data.data(),
                             (unsigned char *)data);
        }

        vkUnmapMemory(device, stagingBuffer);

        transitionImageLayout(device, commandPool, graphicsQueue, texture,
                              VK_IMAGE_LAYOUT_UNDEFINED,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        copyBufferToImage(device, commandPool, graphicsQueue, stagingBuffer,
                          texture, image.width, image.height);
        transitionImageLayout(device, commandPool, graphicsQueue, texture,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    textureDirty = false;
}

} // namespace BR
