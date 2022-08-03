
#include "vk/VKEngine.h"
#include "vk/VKPhysicalDeviceEnumerations.h"
#include "vk/VKDescriptorPoolExt.h"
#include "vk/FreeFont.h"

#include "np2.h"
#include "soundmng.h"
#include "keystat.h"
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
	float x;
	float y;
};

class Column {
public:
	std::vector<TextDims> elements;

	Column(){}

	Dimensions calculate(VulkanVtxBuffer& vtxBuffer, VulkanVtxBuffer& uvBuffer, size_t offset, Image& image){
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

	size_t getRenderCount(){ return elements.size() * 6;}
	size_t getRenderSize(){ return elements.size() * sizeof(VTXs);}

	void add(TextDims element){
		elements.push_back(element);
	}
};

//Pen pen(drawArray.texture.image, freetypeFace, hbfont);

class Text{
public:
	std::string text;
	TextDims dims;
};

class TextCache {
	Pen pen;
	std::vector<Text> texts;

	TextDims alloc(const char *text){
		TextDims textDims = pen.draw(text);
		texts.push_back({text, textDims});
		return textDims;
	}

public:
	TextCache (VulkanTexture& texture, FreetypeFace& face, HarfbuzzFont& font) :
		pen(texture.image, face, font){
	}

	TextDims query(const char *text){
		std::string str;
		if(!text || text[0] == '\0'){
			str = "(empty)";
		}
		else {
			str = text;
		}

		for(auto& cachedText : texts){
			if(cachedText.text == str) return cachedText.dims;
		}

		return alloc(str.c_str());
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
	Column left, right;

	VulkanVtxBuffer vtxBuffer;
	VulkanVtxBuffer uvBuffer;
	VulkanDescriptorPoolExt descriptorPoolExt;
	VulkanDescriptorSetExt descriptorSetExt;

public:

	Menu(VulkanDevice& device, VulkanPhysicalDevice& physicalDevice,
		 VulkanTexture& textTexture, VulkanRenderer& renderer) :
		vtxBuffer(device, physicalDevice, 4096),
		uvBuffer(device, physicalDevice, 4096),
		descriptorPoolExt(device, 2),
		descriptorSetExt(device, physicalDevice, textTexture.textureView, renderer.sampler,
			descriptorPoolExt, renderer.descriptorLayoutExt, 2
		)
	{

	}

	template<size_t N>
	void addHex(TextCache& textCache, const char *leftString, unsigned char (&arr) [N]){
		char buf[N * 2 + 1];
		toHex(arr, (char (&) [N*2])buf);
		buf[N * 2] = '\0';

		left.add(textCache.query(leftString));
		right.add(textCache.query(buf));
	}

	//automatic promotion
	void add(TextCache& textCache, const char *leftString, UINT64 val){
		char buf[std::numeric_limits<decltype(val)>::digits + 1];

		snprintf(buf, sizeof(buf), "%ld", val);

		left.add(textCache.query(leftString));
		right.add(textCache.query(buf));
	}

	void add(TextCache& textCache, const char *leftString, const char *rightString){
		left.add(textCache.query(leftString));
		right.add(textCache.query(rightString));
	}

	void updateUniforms(float scale){
		Matrix4x4f model = Matrix4x4f::ident();

		model.sx() *= scale;
		model.sy() *= scale;

		descriptorSetExt.updateModelMatrix(model, 0);

		model.tx() = 0.5;

		descriptorSetExt.updateModelMatrix(model, 1);

	}

	void prepare(VulkanTexture& textTexture){

		Dimensions lDims = left.calculate(vtxBuffer, uvBuffer, 0, textTexture.image);
		float lScale = getFittingScale(lDims, {0.4, 1.0});

		Dimensions rDims = right.calculate(vtxBuffer, uvBuffer, left.elements.size(), textTexture.image);
		float rScale = getFittingScale(rDims, {0.4, 1.0});

		updateUniforms(std::min(lScale, rScale));

		Matrix4x4f world = Matrix4x4f::ortho1To1();
		descriptorSetExt.updateWorldMatrix(world);
	};

	void render(VulkanRenderer& renderer, VulkanRenderBuffer& renderBuffer){
		renderer.pipelineExt1to1->record(
			renderBuffer, descriptorSetExt.get(0),
			vtxBuffer, 0,
			uvBuffer, 0,
			left.getRenderCount()
		);

		renderer.pipelineExt1to1->record(
			renderBuffer, descriptorSetExt.get(1),
			vtxBuffer, left.getRenderSize(),
			uvBuffer, left.getRenderSize(),
			right.getRenderCount()
		);
	}

	void chainBuffers(std::vector<VulkanCmbBuffer *>& cmbBuffers){
		cmbBuffers.push_back(&vtxBuffer);
		cmbBuffers.push_back(&uvBuffer);
		cmbBuffers.push_back(&descriptorSetExt.uniformBuffer);
	};
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

	VulkanTexture textTexture(scaler.device, physicalDevice, scaler.device.graphicsQueue,
		scaler.device.graphicsFamily, 4096, 4096);

	TextCache textCache(textTexture, freetypeFace, hbfont);

	Menu menu(scaler.device, scaler.physicalDevice, textTexture, scaler.renderer);
	menu.add(textCache, "FDD 0: ", basename(cfg.fdd[0]));
	menu.add(textCache, "FDD 1: ", basename(cfg.fdd[1]));
	menu.add(textCache, "FDD 2: ", basename(cfg.fdd[2]));
	menu.add(textCache, "FDD 3: ", basename(cfg.fdd[3]));

	menu.add(textCache, "HDD 0: ", basename(cfg.sasihdd[0]));
	menu.add(textCache, "HDD 1: ", basename(cfg.sasihdd[1]));

	menu.add(textCache, "Font: ", basename(cfg.fontfile));

	menu.add(textCache, "BEEP_VOL: ", cfg.BEEP_VOL);
	menu.add(textCache, "BG_COLOR: ", cfg.BG_COLOR);
	menu.add(textCache, "FG_COLOR: ", cfg.FG_COLOR);
	menu.add(textCache, "color16: ",  cfg.color16);
	menu.add(textCache, "BTN_MODE: ", cfg.BTN_MODE);
	menu.add(textCache, "LCD_MODE: ", cfg.LCD_MODE);
	menu.add(textCache, "KEY_MODE: ", cfg.KEY_MODE);

	menu.add(textCache,    "model: ",       cfg.model);
	menu.add(textCache,    "BTN_RAPID: ",   cfg.BTN_RAPID);
	menu.add(textCache,    "DISPSYNC: ",    cfg.DISPSYNC);
	menu.add(textCache,    "EXTMEM: ",      cfg.EXTMEM);
	menu.add(textCache,    "ITF_WORK: ",    cfg.ITF_WORK);
	menu.add(textCache,    "MOTOR: ",       cfg.MOTOR);
	menu.addHex(textCache, "wait: ",        cfg.wait);
	menu.add(textCache,    "PROTECTMEM: ",  cfg.PROTECTMEM);
	menu.add(textCache,    "SOUND_SW: ",    cfg.SOUND_SW);
	menu.add(textCache,    "vol_adpcm",     cfg.vol_adpcm);
	menu.add(textCache,    "vol_fm",        cfg.vol_fm);
	menu.add(textCache,    "vol_pcm",       cfg.vol_pcm);
	menu.add(textCache,    "vol_rhythm",    cfg.vol_rhythm);
	menu.add(textCache,    "vol_ssg",       cfg.vol_ssg);
	menu.add(textCache,    "vol_ssg",       cfg.vol_ssg);
	menu.addHex(textCache, "vol14",         cfg.vol14);
	menu.add(textCache,    "usefd144",      cfg.usefd144);
	menu.add(textCache,    "uPD72020",      cfg.uPD72020);
	menu.add(textCache,    "spbopt",        cfg.spbopt);

	menu.prepare(textTexture);

	textTexture.textureDirty = true;

	ViewPortMode mode = ViewPortMode::INTEGER;

	std::vector<VulkanCmbBuffer *> cmbBuffers;
	menu.chainBuffers(cmbBuffers);

	while(scaler.getWindowState() != WindowState::SHOULDCLOSE && !sfd.isTriggered()){
		mainloop(&ctx);

		if(scaler.renderingComplete()){
			mainTexture.update();
			textTexture.update();

			renderBuffer = scaler.newRenderBuffer();
			scaler.pollWindowEvents();

			renderBuffer->begin(scaler.getRenderPass(), scaler.swapchain);

			if(visualScreen == VisualScreen::CFG){
				menu.render(scaler.renderer, *renderBuffer);
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



