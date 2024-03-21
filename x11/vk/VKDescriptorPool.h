#ifndef VKDESCRIPTORPOOL_H
#define VKDESCRIPTORPOOL_H

#include <vulkan/vulkan.h>

namespace BR {
class VulkanDescriptorPool {
    const VkDevice &device;
    VkDescriptorPool descriptorPool;

  public:
    VulkanDescriptorPool(const VkDevice &device_, uint32_t descriptorCount);
    ~VulkanDescriptorPool();

    operator const VkDescriptorPool &() const { return descriptorPool; }
};

} // namespace BR

#endif // VKDESCRIPTORPOOL_H
