#ifndef VKDEVICE_H
#define VKDEVICE_H

#include <vulkan/vulkan.h>

#include <vector>

namespace BR {

class VulkanDevice {
    VulkanDevice(VulkanDevice &) = delete;

  public:
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;

    uint32_t graphicsFamily;
    uint32_t presentFamily;

    VulkanDevice(bool enableValidationLayer,
                 const VkPhysicalDevice &physicalDevice, VkSurfaceKHR &surface,
                 const std::vector<VkQueueFamilyProperties> &queueFamilies);
    ~VulkanDevice();

    operator VkDevice &() { return device; }

    operator const VkDevice &() const { return device; }
};

} // namespace BR

#endif // VKDEVICE_H
