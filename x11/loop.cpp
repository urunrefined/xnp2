#include "vk/VKEngine.h"
#include "vk/VKPhysicalDeviceEnumerations.h"
#include "vk/VKDescriptorPoolExt.h"
#include "vk/FreeFont.h"

#include "np2.h"
#include "loop.h"
#include "exception.h"
#include "pccore.h"
#include "Vertex.h"
#include "Matrix4x4.h"
#include "pccorecallbacks.h"
#include "inputhandling.h"

namespace BR {

static VulkanPhysicalDevice glPhysicalDeviceSelection(VulkanContext& engine){
	BR::VulkanPhysicalDeviceEnumerations physicalDeviceEnums(engine.instance);

	for(BR::VulkanPhysicalDevice& physicalDevice : physicalDeviceEnums.physicalDevices){
		//Use the first one available

		if(physicalDevice.isDeviceSuitable(engine.surface)){
			return physicalDevice;
		}
	}

	throw Exception("No graphics device suitable");
}

static const unsigned int pc98Width = 640;
static const unsigned int pc98Height = 400;

struct UVs {
	Vec2 uvs[6];
};

struct VTXs{
	Vec2 vtxs[6];
};

//TODO Change ll to bl // --
UVs calculateUvs(TextDims& dims, Image& image){
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

VTXs getTriangleList(Vec2& ul, Vec2& ur, Vec2& ll, Vec2& lr){
	return VTXs {ll, ul, ur, ll, ur, lr};
}

Vec2 posstatic[6] {
	{-1.0, 1.0},  // lower left
	{-1.0,-1.0},  // upper left
	{ 1.0,-1.0},  // upper right

	{-1.0, 1.0},  // lower left
	{ 1.0,-1.0},  // upper right
	{ 1.0, 1.0}   // lower right
};

class Dimensions {
public:
	float x = 0;
	float y = 0;
};

class Column {
public:
	std::vector<std::string> elements;

	Column(){}

	void add(const std::string& element){
		elements.push_back(element);
	}
};

//Column

Dimensions calculate(std::vector<TextDims>& elements, VulkanVtxBuffer& vtxBuffer, VulkanVtxBuffer& uvBuffer, size_t offset, Image& image){
	Dimensions cDims = {0,0};

	VTXs vtxs[elements.size()];
	UVs uvs[elements.size()];

	float top = 0;

	for(size_t i = 0 ; i < elements.size(); i++){
		TextDims& dims = elements[i];
		VTXs& pos = vtxs[i];

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

	for(size_t i = 0 ; i < elements.size(); i++){
		TextDims& dims = elements[i];
		UVs& uv = uvs[i];
		uv = calculateUvs(dims, image);
	}

	printf("offset: %zu Update Size: %zu\n", offset * sizeof(VTXs), elements.size() * sizeof(VTXs));
	vtxBuffer.update((char *)vtxs, offset * sizeof(VTXs), elements.size() * sizeof(VTXs));
	uvBuffer.update((char *)uvs, offset * sizeof(VTXs), elements.size() * sizeof(UVs));

	return cDims;
}

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
		const VkQueue& graphicsQueue, int graphicsFamily, FreetypeFace& face, HarfbuzzFont& font)
			:
		texture(device, physicalDevice, graphicsQueue, graphicsFamily, 4096, 4096),
		pen(texture.image, face, font)
	{}

	bool alloc(const char *str, Text& text){
		TextDims textDims = pen.draw(str);
		texts.push_back({str, &texture, textDims});
		text = texts.back();
		return true;
	}

	bool getText(const char *str, Text& text){
		for(auto& cachedText : texts){
			if(cachedText.str == str){
				text = cachedText;
				return true;
			};
		}

		return false;
	}
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
			   const VkQueue& graphicsQueue_, int graphicsFamily_,FreetypeFace& face_, HarfbuzzFont& font_)

		: device(device_), physicalDevice(physicalDevice_), graphicsQueue(graphicsQueue_),
			graphicsFamily(graphicsFamily_), face(face_), font(font_)
	{}

	Text query(const char *text){
		std::string str;
		if(!text || text[0] == '\0'){
			str = "(empty)";
		}
		else {
			str = text;
		}

		for(auto& page : pages){
			Text t1;
			if(page->getText(str.c_str(), t1)){
				return t1;
			}

			if(page->alloc(str.c_str(), t1)){
				return t1;
			}
		}

		{
			Text t1;

			pages.emplace_back(std::make_unique<Page>(device, physicalDevice,
				graphicsQueue, graphicsFamily, face, font));

			pages.back()->alloc(str.c_str(), t1);
			return t1;
		}
	}

	void update(){
		for(auto& page : pages){
			page->texture.update();
		}
	}
};

static float getFittingScale(const Dimensions& in, const Dimensions& limits){

	float scaleX = (limits.x / in.x);
	float scaleY = (limits.y / in.y);

	if(scaleX < scaleY){
		return scaleX;
	}
	else{
		return scaleY;
	}
}

static char hexmap [16]{
	'0','1','2','3','4','5','6','7',
	'8','9','A','B','C','D','E','F'
};

template <size_t N>
static void toHex(unsigned char (&in) [N], char (&out)[N * 2]){
	for(size_t i=0; i < N; i++){
		out[i * 2    ] = hexmap[(in[i] >> 4)& 0b00001111];
		out[i * 2 + 1] = hexmap[ in[i]      & 0b00001111];
	}
}

class Menu {
public:
	Column left, right;

	Menu(){
	}

	template<size_t N>
	void addHex(const char *leftString, unsigned char (&arr) [N]){
		char buf[N * 2 + 1];
		toHex(arr, (char (&) [N*2])buf);
		buf[N * 2] = '\0';

		left.add(leftString);
		right.add(buf);
	}

	//automatic promotion
	void add(const char *leftString, UINT64 val){
		char buf[std::numeric_limits<decltype(val)>::digits + 1];

		snprintf(buf, sizeof(buf), "%ld", val);

		left.add(leftString);
		right.add(buf);
	}

	void add(const char *leftString, const char *rightString){
		left.add(leftString);
		right.add(rightString);
	}
};

class TextPrep {
public:
	VTXs vtxs;
	UVs uvs;
	VulkanTexture *texture;

	TextPrep(Text& text, const VTXs& vtxs_) :
		vtxs(vtxs_),
		uvs(calculateUvs(text.dims, text.texture->image)),
		texture(text.texture)
	{}
};

class TextPrepMat {
public:
	Matrix4x4f model;
	std::vector<TextPrep> buffer;

	TextPrepMat(std::vector<TextPrep> buffer_, Matrix4x4f model_) :
		model(model_), buffer(buffer_)
	{}
};

class TextBuffer{
	TextPrep getTextPrep(float& top, TextCache& textCache, const std::string& element){
		Text text = textCache.query(element.c_str());
		auto& dims = text.dims;

		float t = top + dims.sizeY;
		float b = top;
		top = t;

		Vec2 ul = {0, t};
		Vec2 ur = {(float)dims.sizeX, t};
		Vec2 ll = {0, b};
		Vec2 lr = {(float)dims.sizeX, b};

		return {
			text,
			getTriangleList(ul, ur, ll, lr)
		};
	}

public:
	std::vector<TextPrepMat> buffer;

	void add(TextCache& textCache, Menu& menu)
	{
		std::vector<TextPrep> left;
		std::vector<TextPrep> right;

		float top = 0;

		for(auto & element : menu.left.elements){
			left.push_back(getTextPrep(top, textCache, element.c_str()));
		}

		top = 0;

		for(auto & element : menu.right.elements){
			right.push_back(getTextPrep(top, textCache, element.c_str()));
		}

		assert(menu.left.elements.size() == menu.right.elements.size());

		Dimensions dims;

		for(auto& e : left)
		{
			for(auto& vtx : e.vtxs.vtxs){
				dims.x = std::max(dims.x, vtx.x);
				dims.y = std::max(dims.y, vtx.y);
			}
		}

		for(auto& e : right)
		{
			for(auto& vtx : e.vtxs.vtxs){
				dims.x = std::max(dims.x, vtx.x);
				dims.y = std::max(dims.y, vtx.y);
			}
		}


		{
			float scale = getFittingScale(dims, {0.4, 1.0});

			Matrix4x4f model = Matrix4x4f::ident();

			model.sx() *= scale;
			model.sy() *= scale;

			buffer.push_back({left, model});

			model.tx() += 0.4;

			buffer.push_back({right, model});
		}

	}
};

class Mapping {
public:
	VulkanTexture *texture;
	Matrix4x4f matrix;

	Mapping (VulkanTexture *texture_, Matrix4x4f matrix_)
		: texture (texture_), matrix(matrix_){
	}

	bool operator==(const Mapping& mapping){
		return texture == mapping.texture
			&& memcmp(matrix.getData(), mapping.matrix.getData(), sizeof(float) * 16) == 0;
	}
};


class MappingAssoc {
public:
	Mapping mapping;
	std::vector<TextPrepMat> textPreps;

	MappingAssoc(const Mapping& mapping_, const TextPrepMat& textPreps_)
		: mapping(mapping_)
	{
		add(textPreps_);
	}

	bool operator==(const Mapping& mapping_){
		return mapping == mapping_;
	}

	void add(const TextPrepMat& mat){
		textPreps.push_back(mat);
	}
};

class TextDisplayTable {
public:
	std::vector<MappingAssoc> map;

	TextDisplayTable(std::vector<TextPrepMat>& buffer){
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
};

class RenderResource {
public:
	VulkanVtxBuffer vtxs;
	VulkanVtxBuffer uvs;
	VulkanDescriptorSetExt& descriptor;
	int setIdx;
	int renderCount;

	RenderResource(VulkanDevice& device, VulkanPhysicalDevice& physicalDevice, const std::vector<Vec2>& vtxs_, const std::vector<Vec2>& uvs_, VulkanDescriptorSetExt& descriptor_, int setIdx_) :
		vtxs(device, physicalDevice, vtxs_.size() * sizeof(Vec2)),
		uvs(device, physicalDevice, uvs_.size() * sizeof(Vec2)),
		descriptor(descriptor_),
		setIdx(setIdx_),
		renderCount(vtxs_.size())
	{
		vtxs.update((char *)vtxs_.data(), 0, vtxs_.size() * sizeof(Vec2));
		uvs.update((char *)uvs_.data(), 0, uvs_.size() * sizeof(Vec2));
	}

	void render(VulkanRenderer& renderer, VulkanRenderBuffer& renderBuffer){


		renderer.pipelineExt1to1->record(
			renderBuffer, descriptor.get(setIdx),
			vtxs, 0,
			uvs, 0,
			renderCount
		);
	}
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

static void glLoop(
		SignalFD& sfd,
		VulkanContext& engine,
		VulkanPhysicalDevice& physicalDevice,
		HarfbuzzFont& hbfont,
		FreetypeFace& freetypeFace,
		NP2CFG& cfg,
		NP2OSCFG& oscfg)
{
	VisualScreen visualScreen = VisualScreen::MAIN;

	VulkanScaler scaler(engine, physicalDevice);
	std::unique_ptr<VulkanRenderBuffer> renderBuffer;

	VulkanTexture mainTexture(
		scaler.device, physicalDevice, scaler.renderer.graphicsQueue,
		scaler.renderer.graphicsFamily, pc98Width, pc98Height
	);

	VulkanDescriptorPool descriptorPool(scaler.device, 1);

	VulkanDescriptorSet descriptorSetMain(
		scaler.device,
		mainTexture.textureView,
		scaler.renderer.sampler,
		descriptorPool,
		scaler.renderer.descriptorLayout
	);

	CallbackContext ctx{
		&mainTexture,
		&scaler.context.glfwCtx.input
	};

	TextCache textCache(scaler.device, physicalDevice, scaler.device.graphicsQueue,
						scaler.device.graphicsFamily, freetypeFace, hbfont);

	Menu menu;

	menu.add("FDD 0: ", basename(cfg.fdd[0]));
	menu.add("FDD 1: ", basename(cfg.fdd[1]));
	menu.add("FDD 2: ", basename(cfg.fdd[2]));
	menu.add("FDD 3: ", basename(cfg.fdd[3]));

	menu.add("HDD 0: ", basename(cfg.sasihdd[0]));
	menu.add("HDD 1: ", basename(cfg.sasihdd[1]));

	menu.add("Font: ", basename(cfg.fontfile));

	menu.add("BEEP_VOL: ", cfg.BEEP_VOL);
	menu.add("BG_COLOR: ", cfg.BG_COLOR);
	menu.add("FG_COLOR: ", cfg.FG_COLOR);
	menu.add("color16: ",  cfg.color16);
	menu.add("BTN_MODE: ", cfg.BTN_MODE);
	menu.add("LCD_MODE: ", cfg.LCD_MODE);
	menu.add("KEY_MODE: ", cfg.KEY_MODE);

	menu.add("model: ",       cfg.model);
	menu.add("BTN_RAPID: ",   cfg.BTN_RAPID);
	menu.add("DISPSYNC: ",    cfg.DISPSYNC);
	menu.add("EXTMEM: ",      cfg.EXTMEM);
	menu.add("ITF_WORK: ",    cfg.ITF_WORK);
	menu.add("MOTOR: ",       cfg.MOTOR);
	menu.addHex("wait: ",     cfg.wait);
	menu.add("PROTECTMEM: ",  cfg.PROTECTMEM);
	menu.add("SOUND_SW: ",    cfg.SOUND_SW);
	menu.add("vol_adpcm",     cfg.vol_adpcm);
	menu.add("vol_fm",        cfg.vol_fm);
	menu.add("vol_pcm",       cfg.vol_pcm);
	menu.add("vol_rhythm",    cfg.vol_rhythm);
	menu.add("vol_ssg",       cfg.vol_ssg);
	menu.add("vol_ssg",       cfg.vol_ssg);
	menu.addHex("vol14",      cfg.vol14);
	menu.add("usefd144",      cfg.usefd144);
	menu.add("uPD72020",      cfg.uPD72020);
	menu.add("spbopt",        cfg.spbopt);

	TextBuffer textBuffer;
	textBuffer.add(textCache, menu);

	TextDisplayTable displayTable(textBuffer.buffer);

	TextDisplayList displayList(
		scaler.device, physicalDevice,
		scaler.renderer.sampler,
		scaler.renderer.descriptorLayoutExt,
		displayTable.map
	);

	ViewPortMode mode = ViewPortMode::INTEGER;

	std::vector<VulkanCmbBuffer *> cmbBuffers;
	displayList.chainBuffers(cmbBuffers);
	textCache.update();

	while(scaler.getWindowState() != WindowState::SHOULDCLOSE && !sfd.isTriggered()){
		mainloop(&ctx);

		if(scaler.renderingComplete()){
			mainTexture.update();

			renderBuffer = scaler.newRenderBuffer();
			scaler.pollWindowEvents();

			renderBuffer->begin(scaler.getRenderPass(), scaler.swapchain);

			if(visualScreen == VisualScreen::CFG){
				displayList.render(scaler.renderer, *renderBuffer);
			}
			else {
				if(mode == ViewPortMode::ASPECT){
					scaler.renderer.pipelineAspect->record(*renderBuffer, descriptorSetMain, 6);
				}
				else if(mode == ViewPortMode::STRETCH){
					scaler.renderer.pipelineStretch->record(*renderBuffer, descriptorSetMain, 6);
				}
				else {
					scaler.renderer.pipelineInteger->record(*renderBuffer, descriptorSetMain, 6);
				}
			}

			renderBuffer->end();

			if(scaler.drawAndPresent(*renderBuffer, cmbBuffers) == RenderState::NEEDSSWAPCHAINUPDATE){
				scaler.recreateSwapChain();
			}
		}

		GLFWInput& input = engine.glfwCtx.getInput();

		handleInput(input, mode, visualScreen);

		input.reset();
	}

	while(!scaler.renderingComplete()){
		usleep(1000);
	}
}

void loop(
		SignalFD& sfd,
		NP2CFG& cfg,
		NP2OSCFG& oscfg
){
	FontconfigLib lib;
	FontList fontlist(lib);
	std::string fontfile = fontlist.getFirst();

	FreetypeLib freetypeLib;
	FreetypeFace freetypeFace(freetypeLib, fontfile.c_str(), 32);

	HarfbuzzBlob hbblob(fontfile.c_str());
	HarfbuzzFace hbface(hbblob.blob);
	HarfbuzzFont hbfont(hbface.face);
#ifndef VKDEBUG
	VulkanContext engine(false);
#else
	VulkanContext engine(true);
#endif
	VulkanPhysicalDevice physicalDevice = glPhysicalDeviceSelection(engine);

	glLoop(sfd, engine, physicalDevice, hbfont, freetypeFace, cfg, oscfg);
}

}



