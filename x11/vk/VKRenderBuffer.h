#ifndef VKRENDERBUFFER2_H
#define VKRENDERBUFFER2_H

#include "VKCommandPool.h"
#include "VKDevice.h"
#include "VKRenderPass.h"
#include "VKSwapchain.h"

namespace BR {

class VulkanRenderBuffer {
  public:
    const VulkanDevice &device;

    VulkanCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    VulkanRenderBuffer(const VulkanDevice &device_, uint32_t graphicsFamily)
        : device(device_), commandPool(device, graphicsFamily) {}

    virtual void begin(VulkanRenderPass &renderPass,
                       VulkanSwapchain &swapChain);
    virtual void end();
};

} // namespace BR

#endif // VKRENDERBUFFER_H
