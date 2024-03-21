#ifndef VKDESCRIPTORPOOLEXT_H
#define VKDESCRIPTORPOOLEXT_H

#include <vulkan/vulkan.h>

namespace BR {

class VulkanDescriptorPoolExt {
    const VkDevice &device;
    VkDescriptorPool descriptorPool;

  public:
    VulkanDescriptorPoolExt(const VkDevice &device_, uint32_t descriptorCount);

    ~VulkanDescriptorPoolExt();

    operator const VkDescriptorPool &() const { return descriptorPool; }
};

} // namespace BR

#endif // VKDESCRIPTORPOOLEXT_H
