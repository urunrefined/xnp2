#include "Displaylist.h"

namespace BR {

UVs calculateUvs(TextDims &dims, Image &image) {
    Vec2 ll{(float)dims.startX / (float)image.width,
            ((float)dims.startY + (float)dims.sizeY) / (float)image.height};

    Vec2 ul{(float)dims.startX / (float)image.width,
            (float)dims.startY / (float)image.height};

    Vec2 ur{((float)dims.startX + (float)dims.sizeX) / (float)image.width,
            (float)dims.startY / (float)image.height};

    Vec2 lr{((float)dims.startX + (float)dims.sizeX) / (float)image.width,
            ((float)dims.startY + (float)dims.sizeY) / (float)image.height};

    return UVs{ll, ul, ur, ll, ur, lr};
}

TextPrep::TextPrep(VulkanTexture *texture, TextDims dims, const VTXs &vtxs_)
    : vtxs(vtxs_), uvs(calculateUvs(dims, texture->image)), texture(texture) {}

TextPrepMat::TextPrepMat(std::vector<TextPrep> buffer_, Matrix4x4f model_)
    : model(model_), buffer(buffer_) {}

Mapping::Mapping(VulkanTexture *texture_, Matrix4x4f matrix_)
    : texture(texture_), matrix(matrix_) {}

bool Mapping::operator==(const Mapping &mapping) {
    return texture == mapping.texture &&
           memcmp(matrix.getData(), mapping.matrix.getData(),
                  sizeof(float) * 16) == 0;
}

MappingAssoc::MappingAssoc(const Mapping &mapping_,
                           const TextPrepMat &textPreps_)
    : mapping(mapping_) {
    add(textPreps_);
}

bool MappingAssoc::operator==(const Mapping &mapping_) {
    return mapping == mapping_;
}

void MappingAssoc::add(const TextPrepMat &mat) { textPreps.push_back(mat); }

TextDisplayTable::TextDisplayTable(std::vector<TextPrepMat> &buffer) {
    for (auto &e : buffer) {
        for (auto &e2 : e.buffer) {
            Mapping index{e2.texture, e.model};
            const auto &it = std::find(map.begin(), map.end(), index);

            if (it == map.end()) {
                map.push_back({{e2.texture, e.model}, e});
            } else {
                it->add(e);
            }
        }
    }
}

RenderResource::RenderResource(VulkanDevice &device,
                               VulkanPhysicalDevice &physicalDevice,
                               const std::vector<Vec2> &vtxs_,
                               const std::vector<Vec2> &uvs_,
                               VulkanDescriptorSetExt &descriptor_, int setIdx_)
    : vtxs(device, physicalDevice, vtxs_.size() * sizeof(Vec2)),
      uvs(device, physicalDevice, uvs_.size() * sizeof(Vec2)),
      descriptor(descriptor_), setIdx(setIdx_), renderCount(vtxs_.size()) {
    vtxs.update((char *)vtxs_.data(), 0, vtxs_.size() * sizeof(Vec2));
    uvs.update((char *)uvs_.data(), 0, uvs_.size() * sizeof(Vec2));
}

void RenderResource::render(VulkanRenderer &renderer,
                            VulkanRenderBuffer &renderBuffer) {

    renderer.pipelineExt1to1->record(renderBuffer, descriptor.get(setIdx), vtxs,
                                     0, uvs, 0, renderCount);
}

TextDisplayList::TextDisplayList(VulkanDevice &device,
                                 VulkanPhysicalDevice &physicalDevice,
                                 VulkanSampler &sampler,
                                 VulkanDescriptorLayoutExt &layout,
                                 std::vector<MappingAssoc> &map)
    : pool(device, map.size()) {
    printf("sz %zu\n", map.size());

    for (size_t i = 0; i < map.size(); i++) {
        auto &e = map[i];

        descriptorSetExts.push_back(std::make_unique<VulkanDescriptorSetExt>(
            device, physicalDevice, e.mapping.texture->textureView, sampler,
            pool, layout, 1));

        auto &set = descriptorSetExts.back();
        set->updateModelMatrix(e.mapping.matrix, 0);
        set->updateWorldMatrix(Matrix4x4f::ortho1To1());

        std::vector<Vec2> vtxs;
        std::vector<Vec2> uvs;

        for (size_t j = 0; j < e.textPreps.size(); j++) {
            for (auto &res : e.textPreps[j].buffer) {
                for (auto &vtxs_ : res.vtxs.vtxs) {
                    vtxs.push_back(vtxs_);
                }

                for (auto &uvs_ : res.uvs.uvs) {
                    uvs.push_back(uvs_);
                }
            }
        }

        resources.push_back(std::make_unique<RenderResource>(
            device, physicalDevice, vtxs, uvs, *set, 0));
    }
}

void TextDisplayList::chainBuffers(std::vector<VulkanCmbBuffer *> &cmbBuffers) {
    for (auto &res : resources) {
        cmbBuffers.push_back(&(res->uvs));
        cmbBuffers.push_back(&(res->vtxs));
    }

    for (auto &des : descriptorSetExts) {
        cmbBuffers.push_back(&(des->uniformBuffer));
    }
}

void TextDisplayList::render(VulkanRenderer &renderer,
                             VulkanRenderBuffer &renderBuffer) {
    for (auto &res : resources) {
        res->render(renderer, renderBuffer);
    }
}

} // namespace BR
