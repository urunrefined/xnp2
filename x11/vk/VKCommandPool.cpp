#include "VKCommandPool.h"

#include <stdexcept>

namespace BR {

VulkanCommandPool::VulkanCommandPool(const VkDevice &device_,
                                     uint32_t graphicsFamily)
    : device(device_) {
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = graphicsFamily;

    if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics command pool!");
    }
}

VulkanCommandPool::~VulkanCommandPool() {
    vkDestroyCommandPool(device, commandPool, nullptr);
}

} // namespace BR
