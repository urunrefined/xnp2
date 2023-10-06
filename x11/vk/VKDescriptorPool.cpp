#include "VKDescriptorPool.h"

#include <stdexcept>

namespace BR {

VulkanDescriptorPool::VulkanDescriptorPool(const VkDevice &device_,
                                           uint32_t descriptorCount)
    : device(device_) {
    VkDescriptorPoolSize poolSize;

    poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize.descriptorCount = descriptorCount;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = descriptorCount;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

VulkanDescriptorPool::~VulkanDescriptorPool() {
    vkDestroyDescriptorPool(device, descriptorPool, 0);
}

} // namespace BR
