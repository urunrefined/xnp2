#include "Displaylist.h"

namespace BR {

UVs calculateUvs(TextDims &dims, Image &image){
	Vec2 ll{(float)dims.startX / (float)image.width,
				((float)dims.startY + (float)dims.sizeY) / (float)image.height};

	Vec2 ul{(float)dims.startX / (float)image.width,
				(float)dims.startY / (float)image.height};

	Vec2 ur{((float)dims.startX + (float)dims.sizeX) / (float)image.width,
				(float)dims.startY / (float)image.height};

	Vec2 lr{((float)dims.startX + (float)dims.sizeX) / (float)image.width,
				((float)dims.startY + (float)dims.sizeY) / (float)image.height};

	return UVs {ll, ul, ur, ll, ur, lr};
}

TextPrep::TextPrep(VulkanTexture *texture, TextDims dims, const VTXs &vtxs_) :
	vtxs(vtxs_),
	uvs(calculateUvs(dims, texture->image)),
	texture(texture)
{}

TextPrepMat::TextPrepMat(std::vector<TextPrep> buffer_, Matrix4x4f model_) :
	model(model_), buffer(buffer_)
{}

Mapping::Mapping(VulkanTexture *texture_, Matrix4x4f matrix_)
	: texture (texture_), matrix(matrix_){
}

bool Mapping::operator==(const Mapping &mapping){
	return texture == mapping.texture
			&& memcmp(matrix.getData(), mapping.matrix.getData(), sizeof(float) * 16) == 0;
}

MappingAssoc::MappingAssoc(const Mapping &mapping_, const TextPrepMat &textPreps_)
	: mapping(mapping_)
{
	add(textPreps_);
}

bool MappingAssoc::operator==(const Mapping &mapping_){
	return mapping == mapping_;
}

void MappingAssoc::add(const TextPrepMat &mat){
	textPreps.push_back(mat);
}

TextDisplayTable::TextDisplayTable(std::vector<TextPrepMat> &buffer){
	for(auto& e : buffer){
		for(auto& e2 : e.buffer){
			Mapping index {e2.texture, e.model};
			const auto& it = std::find(map.begin(), map.end(), index);

			if(it == map.end()){
				map.push_back(
							{{e2.texture, e.model}, e}
							);
			}
			else{
				it->add(e);
			}
		}
	}
}

RenderResource::RenderResource(VulkanDevice &device, VulkanPhysicalDevice &physicalDevice, const std::vector<Vec2> &vtxs_, const std::vector<Vec2> &uvs_, VulkanDescriptorSetExt &descriptor_, int setIdx_) :
	vtxs(device, physicalDevice, vtxs_.size() * sizeof(Vec2)),
	uvs(device, physicalDevice, uvs_.size() * sizeof(Vec2)),
	descriptor(descriptor_),
	setIdx(setIdx_),
	renderCount(vtxs_.size())
{
	vtxs.update((char *)vtxs_.data(), 0, vtxs_.size() * sizeof(Vec2));
	uvs.update((char *)uvs_.data(), 0, uvs_.size() * sizeof(Vec2));
}

void RenderResource::render(VulkanRenderer &renderer, VulkanRenderBuffer &renderBuffer){


	renderer.pipelineExt1to1->record(
				renderBuffer, descriptor.get(setIdx),
				vtxs, 0,
				uvs, 0,
				renderCount
				);
}


}
