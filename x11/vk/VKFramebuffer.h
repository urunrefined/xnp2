#ifndef VKFRAMEBUFFER_H
#define VKFRAMEBUFFER_H

#include <vulkan/vulkan.h>

#include <array>

namespace BR {

class VulkanFramebuffer {
    VkFramebuffer framebuffer;
    const VkDevice &device;

  public:
    VulkanFramebuffer(const VulkanFramebuffer &imageView) = delete;
    VulkanFramebuffer(const VkDevice &device_, VkExtent2D swapChainExtent,
                      VkRenderPass &renderPass,
                      std::array<VkImageView, 2> &attachments);
    VulkanFramebuffer(const VkDevice &device_, VkExtent2D extent,
                      VkRenderPass &renderPass, VkImageView &attachment);

    ~VulkanFramebuffer();

    operator VkFramebuffer &() { return framebuffer; }

    operator const VkFramebuffer &() const { return framebuffer; }
};

} // namespace BR

#endif // VKFRAMEBUFFER_H
