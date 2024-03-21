#ifndef BR_VULKANTEXTURE_H
#define BR_VULKANTEXTURE_H

#include "Image.h"
#include "VKBuffers.h"
#include "VKCommandPool.h"
#include "VKImage.h"
#include "VKImageMemory.h"
#include "VKImageView.h"
#include "enums.h"

#include <vector>

namespace BR {

class VulkanTexture {
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkQueue graphicsQueue;
    VulkanImage texture;
    VulkanImageMemory textureMemory;
    VulkanCommandPool commandPool;

  public:
    VulkanImageView textureView;
    bool textureDirty;
    Image image;

  private:
    VulkanBufferGeneric stagingBuffer;

  public:
    VulkanTexture(const VkDevice &device_,
                  const VkPhysicalDevice &physicalDevice_,
                  const VkQueue &graphicsQueue_, int graphicsFamily,
                  uint16_t width, uint16_t height)
        : device(device_), physicalDevice(physicalDevice_),
          graphicsQueue(graphicsQueue_),
          texture(device, width, height, VK_FORMAT_B8G8R8A8_UNORM,
                  VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT),
          textureMemory(physicalDevice, device, texture,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),

          commandPool(device, graphicsFamily),
          textureView(device, texture, VK_FORMAT_B8G8R8A8_UNORM,
                      VK_IMAGE_ASPECT_COLOR_BIT),
          textureDirty(true), image(width, height),
          stagingBuffer(device, physicalDevice, image.data.size(),
                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {}

    void update(DoubleLines doubleLines);
};

} // namespace BR

#endif // BR_VULKANTEXTURE_H
