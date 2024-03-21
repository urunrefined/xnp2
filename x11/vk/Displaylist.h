#ifndef DISPLAYLIST_H
#define DISPLAYLIST_H

#include "Matrix4x4.h"
#include "VKTexture.h"
#include "Vertex.h"
#include "vk/FreeFont.h"
#include "vk/VKDescriptorPoolExt.h"

// TODO: Remove this
#include "FreeFont.h"
#include "VKDescriptorSetExt.h"
#include "VKDevice.h"
#include "VKRenderer.h"

#include <algorithm>
#include <memory>

namespace BR {

struct UVs {
    Vec2 uvs[6];
};

struct VTXs {
    Vec2 vtxs[6];
};

// TODO Change ll to bl // --
UVs calculateUvs(TextDims &dims, Image &image);

class TextPrep {
  public:
    VTXs vtxs;
    UVs uvs;
    VulkanTexture *texture;

    TextPrep(VulkanTexture *texture, TextDims dims, const VTXs &vtxs_);
};

class TextPrepMat {
  public:
    Matrix4x4f model;
    std::vector<TextPrep> buffer;

    TextPrepMat(std::vector<TextPrep> buffer_, Matrix4x4f model_);
};

class Mapping {
  public:
    VulkanTexture *texture;
    Matrix4x4f matrix;

    Mapping(VulkanTexture *texture_, Matrix4x4f matrix_);

    bool operator==(const Mapping &mapping);
};

class MappingAssoc {
  public:
    Mapping mapping;
    std::vector<TextPrepMat> textPreps;

    MappingAssoc(const Mapping &mapping_, const TextPrepMat &textPreps_);

    bool operator==(const Mapping &mapping_);

    void add(const TextPrepMat &mat);
};

class TextDisplayTable {
  public:
    std::vector<MappingAssoc> map;

    TextDisplayTable(std::vector<TextPrepMat> &buffer);
};

class RenderResource {
  public:
    VulkanVtxBuffer vtxs;
    VulkanVtxBuffer uvs;
    VulkanDescriptorSetExt &descriptor;
    int setIdx;
    int renderCount;

    RenderResource(VulkanDevice &device, VulkanPhysicalDevice &physicalDevice,
                   const std::vector<Vec2> &vtxs_,
                   const std::vector<Vec2> &uvs_,
                   VulkanDescriptorSetExt &descriptor_, int setIdx_);

    void render(VulkanRenderer &renderer, VulkanRenderBuffer &renderBuffer);
};

class TextDisplayList {
    VulkanDescriptorPoolExt pool;
    std::vector<std::unique_ptr<VulkanDescriptorSetExt>> descriptorSetExts;
    std::vector<std::unique_ptr<RenderResource>> resources;

  public:
    TextDisplayList(VulkanDevice &device, VulkanPhysicalDevice &physicalDevice,
                    VulkanSampler &sampler, VulkanDescriptorLayoutExt &layout,
                    std::vector<MappingAssoc> &map);

    void chainBuffers(std::vector<VulkanCmbBuffer *> &cmbBuffers);
    ;

    void render(VulkanRenderer &renderer, VulkanRenderBuffer &renderBuffer);
};

} // namespace BR

#endif // DISPLAYLIST_H
