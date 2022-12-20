#ifndef DISPLAYLIST_H
#define DISPLAYLIST_H

#include "Matrix4x4.h"
#include "VKTexture.h"
#include "Vertex.h"
#include "vk/VKDescriptorPoolExt.h"
#include "vk/FreeFont.h"


//TODO: Remove this
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

struct VTXs{
	Vec2 vtxs[6];
};

//TODO Change ll to bl // --
UVs calculateUvs(TextDims& dims, Image& image);

class TextPrep {
public:
	VTXs vtxs;
	UVs uvs;
	VulkanTexture *texture;

	TextPrep(VulkanTexture *texture, TextDims dims, const VTXs& vtxs_);
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

	Mapping (VulkanTexture *texture_, Matrix4x4f matrix_);

	bool operator==(const Mapping& mapping);
};


class MappingAssoc {
public:
	Mapping mapping;
	std::vector<TextPrepMat> textPreps;

	MappingAssoc(const Mapping& mapping_, const TextPrepMat& textPreps_);

	bool operator==(const Mapping& mapping_);

	void add(const TextPrepMat& mat);
};

class TextDisplayTable {
public:
	std::vector<MappingAssoc> map;

	TextDisplayTable(std::vector<TextPrepMat>& buffer);
};

class RenderResource {
public:
	VulkanVtxBuffer vtxs;
	VulkanVtxBuffer uvs;
	VulkanDescriptorSetExt& descriptor;
	int setIdx;
	int renderCount;

	RenderResource(VulkanDevice& device, VulkanPhysicalDevice& physicalDevice, const std::vector<Vec2>& vtxs_, const std::vector<Vec2>& uvs_, VulkanDescriptorSetExt& descriptor_, int setIdx_);

	void render(VulkanRenderer& renderer, VulkanRenderBuffer& renderBuffer);
};

class TextDisplayList
{
	VulkanDescriptorPoolExt pool;
	std::vector<std::unique_ptr<VulkanDescriptorSetExt>> descriptorSetExts;
	std::vector<std::unique_ptr<RenderResource>> resources;

public:
	TextDisplayList(VulkanDevice& device, VulkanPhysicalDevice& physicalDevice,
		VulkanSampler& sampler, VulkanDescriptorLayoutExt& layout, std::vector<MappingAssoc>& map)
		: pool(device, map.size())
	{
		printf("sz %zu\n", map.size());

		for(size_t i = 0; i < map.size(); i++){
			auto& e = map[i];

			descriptorSetExts.push_back(std::make_unique<VulkanDescriptorSetExt>(
				device, physicalDevice, e.mapping.texture->textureView, sampler,
				pool, layout, 1
			));

			auto& set = descriptorSetExts.back();
			set->updateModelMatrix(e.mapping.matrix, 0);
			set->updateWorldMatrix(Matrix4x4f::ortho1To1());

			std::vector<Vec2> vtxs;
			std::vector<Vec2> uvs;

			for(size_t j = 0; j < e.textPreps.size(); j++){
				for(auto& res : e.textPreps[j].buffer){
					for (auto& vtxs_ : res.vtxs.vtxs) {
						vtxs.push_back(vtxs_);
					}

					for (auto& uvs_ : res.uvs.uvs) {
						uvs.push_back(uvs_);
					}
				}
			}

			resources.push_back(std::make_unique<RenderResource>(
				device, physicalDevice, vtxs, uvs, *set, 0
			));
		}
	}

	void chainBuffers(std::vector<VulkanCmbBuffer *>& cmbBuffers){
		for(auto & res : resources){
			cmbBuffers.push_back(&(res->uvs));
			cmbBuffers.push_back(&(res->vtxs));
		}

		for(auto & des : descriptorSetExts){
			cmbBuffers.push_back(&(des->uniformBuffer));
		}
	};

	void render(VulkanRenderer& renderer, VulkanRenderBuffer& renderBuffer){
		for(auto & res : resources){
			res->render(renderer, renderBuffer);
		}
	}
};


}

#endif // DISPLAYLIST_H
