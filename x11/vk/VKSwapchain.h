#include <vulkan/vulkan.h>

#include "VKFramebuffer.h"
#include "VKImageView.h"

#include <memory>
#include <vector>

#ifndef VKSWAPCHAIN_H
#define VKSWAPCHAIN_H

namespace BR {

class VulkanSwapchain {
  public:
    VkInstance &instance;
    const VkPhysicalDevice &physicalDevice;
    VkDevice &device;
    VkCommandPool &commandPool;

    VkQueue &graphicsQueue;
    VkQueue &presentQueue;

    VkRenderPass &renderPass;

    std::vector<std::unique_ptr<VulkanImageView>> swapChainImageViews;
    std::vector<std::unique_ptr<VulkanFramebuffer>> swapChainFramebuffers;

    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;

    VkExtent2D extent;

    VulkanSwapchain(VkInstance &instance_,
                    const VkPhysicalDevice &physicalDevice_, VkDevice &device_,
                    VkCommandPool &commandPool_, VkQueue &graphicsQueue_,
                    VkQueue &presentQueue_, VkRenderPass &renderPass_,
                    std::vector<VkImage> &swapChainImages,
                    VkFormat swapChainImageFormat, VkExtent2D swapChainExtent);

    ~VulkanSwapchain();

    void create(std::vector<VkImage> &swapChainImages,
                VkFormat swapChainImageFormat, VkExtent2D swapChainExtent);

    void clean();

    void reCreate(std::vector<VkImage> &swapChainImages,
                  VkFormat swapChainImageFormat, VkExtent2D swapChainExtent);

    void createImageViews(std::vector<VkImage> &swapChainImages,
                          VkFormat swapChainImageFormat);

    void createFramebuffers(VkExtent2D swapChainExtent);

    void createDepthResources(VkExtent2D swapChainExtent);

    VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates,
                                 VkImageTiling tiling,
                                 VkFormatFeatureFlags features);

    VkFormat findDepthFormat();

    bool hasStencilComponent(VkFormat format);

    void createImage(uint32_t width, uint32_t height, VkFormat format,
                     VkImageTiling tiling, VkImageUsageFlags usage,
                     VkMemoryPropertyFlags properties, VkImage &image,
                     VkDeviceMemory &imageMemory);

    void transitionImageLayout(VkImage image, VkFormat format,
                               VkImageLayout oldLayout,
                               VkImageLayout newLayout);
};

} // namespace BR

#endif
