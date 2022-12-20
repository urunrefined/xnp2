#ifndef TEXT_H
#define TEXT_H

#include "vk/Displaylist.h"

namespace BR {

class Dimensions {
public:
	float x = 0;
	float y = 0;
};

VTXs getTriangleList(Vec2& ul, Vec2& ur, Vec2& ll, Vec2& lr);

Dimensions calculate(std::vector<TextDims>& elements, VulkanVtxBuffer& vtxBuffer, VulkanVtxBuffer& uvBuffer, size_t offset, Image& image);

class Text{
public:
	std::string str;
	VulkanTexture *texture;
	TextDims dims;
};

class Page {
public:
	std::vector<Text> texts;
	VulkanTexture texture;
	Pen pen;

	Page(const VkDevice& device, VulkanPhysicalDevice& physicalDevice,
		const VkQueue& graphicsQueue, int graphicsFamily, FreetypeFace& face, HarfbuzzFont& font);

	bool alloc(const char *str, Text& text);

	bool getText(const char *str, Text& text);
};



class TextCache {
	std::vector<std::unique_ptr<Page>> pages;
	const VkDevice& device;
	VulkanPhysicalDevice& physicalDevice;
	const VkQueue& graphicsQueue;
	int graphicsFamily;
	FreetypeFace& face;
	HarfbuzzFont& font;


public:
	TextCache (const VkDevice& device_, VulkanPhysicalDevice& physicalDevice_,
			   const VkQueue& graphicsQueue_, int graphicsFamily_,FreetypeFace& face_, HarfbuzzFont& font_);

	Text query(const char *text);

	void update();
};

}

#endif // TEXT_H
