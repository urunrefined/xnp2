#include "text.h"

namespace BR {

VTXs getTriangleList(Vec2 &ul, Vec2 &ur, Vec2 &ll, Vec2 &lr) {
    return VTXs{ll, ul, ur, ll, ur, lr};
}

Dimensions calculate(std::vector<TextDims> &elements,
                     VulkanVtxBuffer &vtxBuffer, VulkanVtxBuffer &uvBuffer,
                     size_t offset, Image &image) {
    Dimensions cDims = {0, 0};

    VTXs vtxs[elements.size()];
    UVs uvs[elements.size()];

    float top = 0;

    for (size_t i = 0; i < elements.size(); i++) {
        TextDims &dims = elements[i];
        VTXs &pos = vtxs[i];

        float t = top + dims.sizeY;
        float b = top;
        top = t;

        Vec2 ul = {0, t};
        Vec2 ur = {(float)dims.sizeX, t};
        Vec2 ll = {0, b};
        Vec2 lr = {(float)dims.sizeX, b};

        pos = getTriangleList(ul, ur, ll, lr);

        cDims.x = std::max(cDims.x, ul.x);
        cDims.x = std::max(cDims.x, ur.x);
        cDims.x = std::max(cDims.x, ll.x);
        cDims.x = std::max(cDims.x, lr.x);

        cDims.y = std::max(cDims.y, ul.y);
        cDims.y = std::max(cDims.y, ur.y);
        cDims.y = std::max(cDims.y, ll.y);
        cDims.y = std::max(cDims.y, lr.y);
    }

    for (size_t i = 0; i < elements.size(); i++) {
        TextDims &dims = elements[i];
        UVs &uv = uvs[i];
        uv = calculateUvs(dims, image);
    }

    printf("offset: %zu Update Size: %zu\n", offset * sizeof(VTXs),
           elements.size() * sizeof(VTXs));
    vtxBuffer.update((char *)vtxs, offset * sizeof(VTXs),
                     elements.size() * sizeof(VTXs));
    uvBuffer.update((char *)uvs, offset * sizeof(VTXs),
                    elements.size() * sizeof(UVs));

    return cDims;
}

Page::Page(const VkDevice &device, VulkanPhysicalDevice &physicalDevice,
           const VkQueue &graphicsQueue, int graphicsFamily, FreetypeFace &face,
           HarfbuzzFont &font)
    : texture(device, physicalDevice, graphicsQueue, graphicsFamily, 4096,
              4096),
      pen(texture.image, face, font) {}

bool Page::alloc(const char *str, Text &text) {
    TextDims textDims = pen.draw(str);
    texts.push_back({str, &texture, textDims});
    text = texts.back();
    return true;
}

bool Page::getText(const char *str, Text &text) {
    for (auto &cachedText : texts) {
        if (cachedText.str == str) {
            text = cachedText;
            return true;
        };
    }

    return false;
}

TextCache::TextCache(const VkDevice &device_,
                     VulkanPhysicalDevice &physicalDevice_,
                     const VkQueue &graphicsQueue_, int graphicsFamily_,
                     FreetypeFace &face_, HarfbuzzFont &font_)

    : device(device_), physicalDevice(physicalDevice_),
      graphicsQueue(graphicsQueue_), graphicsFamily(graphicsFamily_),
      face(face_), font(font_) {}

Text TextCache::query(const char *text) {
    std::string str;
    if (!text || text[0] == '\0') {
        str = "(empty)";
    } else {
        str = text;
    }

    for (auto &page : pages) {
        Text t1;
        if (page->getText(str.c_str(), t1)) {
            return t1;
        }

        if (page->alloc(str.c_str(), t1)) {
            return t1;
        }
    }

    {
        Text t1;

        pages.emplace_back(std::make_unique<Page>(
            device, physicalDevice, graphicsQueue, graphicsFamily, face, font));

        pages.back()->alloc(str.c_str(), t1);
        return t1;
    }
}

void TextCache::update() {
    for (auto &page : pages) {
        page->texture.update(DoubleLines::NO);
    }
}

} // namespace BR
