#ifndef VKPIPELINETEXEXT_H
#define VKPIPELINETEXEXT_H

#include "VKRenderBuffer.h"
#include "VKShaders.h"

namespace BR {

class PipelineTexExt {

  public:
    const VkDevice &device;

    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;

    PipelineTexExt(const VkDevice &device_, ShaderStore &shader3D,
                   const VkRect2D &scissor, const VkRenderPass &renderPass,
                   const VkDescriptorSetLayout &desciptorSetLayout);

    ~PipelineTexExt();

    VkPipeline &getPipeline();

    void record(VulkanRenderBuffer &renderBuffer,
                const VkDescriptorSet &descriptorSet, VkBuffer vertices,
                VkDeviceSize verticesOffset, VkBuffer uvs,
                VkDeviceSize uvOffset, size_t drawCount);
};

} // namespace BR

#endif // VKPIPELINETEXEXT_H
