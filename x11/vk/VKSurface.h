#ifndef VKSURFACE_H
#define VKSURFACE_H

#include "VKGLFW.h"

namespace BR {

class VulkanSurface {
  public:
    VkInstance &instance;
    VkSurfaceKHR surface;

    VulkanSurface(GLFWContext &context, VkInstance &instance_);
    ~VulkanSurface();

    operator VkSurfaceKHR &() { return surface; }
};

} // namespace BR

#endif // VKSURFACE_H
