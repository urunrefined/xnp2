#ifndef VKPHYSICALDEVICEENUMERATIONS_H
#define VKPHYSICALDEVICEENUMERATIONS_H

#include "VKPhysicalDevice.h"

namespace BR {

class VulkanPhysicalDeviceEnumerations {
  public:
    std::vector<VulkanPhysicalDevice> physicalDevices;

    VulkanPhysicalDeviceEnumerations(VkInstance &instance);
};

} // namespace BR

#endif // VKPHYSICALDEVICEENUMERATIONS_H
