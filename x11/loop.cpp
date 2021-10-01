
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

static TextDims draw(Pen &pen, const std::string& base, const char *add){

	std::string str = base;

	if(add && add[0] != 0){
		str += add;
	}
	else {
		str += "<empty>";
	}

	return pen.draw(str.c_str());
}

static TextDims drawFilename(Pen &pen, const std::string& base, const char *path){
	if(path){
		const char *filename = strrchr(path, '/');

		if(!filename){
			return draw(pen, base, path);
		} else {
			return draw(pen, base, filename + 1);
		}
	}
	else {
		return draw(pen, base, path);
	}
}

class UVs {
public:
	Vec2 uvs[6];
};

Vec2 uv344[6] {
	{0.0, 1.0},
	{0.0, 0.0},
	{1.0, 0.0},
	{0.0, 1.0},
	{1.0, 0.0},
	{1.0, 1.0}
};

UVs calculateUvs(TextDims& dims, Image& image){
	Vec2 ll{(float)dims.startX / (float)image.width,
			((float)dims.startY + (float)dims.sizeY) / (float)image.height};

	Vec2 ul{(float)dims.startX / (float)image.width,
			(float)dims.startY / (float)image.height};

	Vec2 ur{((float)dims.startX + (float)dims.sizeX) / (float)image.width,
			(float)dims.startY / (float)image.height};

	Vec2 lr{((float)dims.startX + (float)dims.sizeX) / (float)image.width,
			((float)dims.startY + (float)dims.sizeY) / (float)image.height};

	printf("ul %f %f\n", ul.x, ul.y);
	printf("ll %f %f\n", ll.x, ll.y);
	printf("ur %f %f\n", ur.x, ur.y);
	printf("lr %f %f\n", lr.x, lr.y);

	//Vec2 uv344[6] {
	//	{0.0, 1.0},
	//	{0.0, 0.0},
	//	{1.0, 0.0},
	//	{0.0, 1.0},
	//	{1.0, 0.0},
	//	{1.0, 1.0}
	//};

	//return UVs{ll, ul, ur, ll, ur, lr};

	return UVs {uv344[0], uv344[1], uv344[2], uv344[3], uv344[4], uv344[5]};
}

static void glLoop(
		SignalFD& sfd,
		VulkanContext& engine,
		VulkanPhysicalDevice& physicalDevice,
		HarfbuzzFont& hbfont,
		FreetypeFace& freetypeFace,
		NP2CFG& cfg,
		NP2OSCFG& oscfg)
{
	VulkanScaler scaler(engine, physicalDevice);
	std::unique_ptr<VulkanRenderBuffer> renderBuffer;

	VulkanTexture mainTexture(
		scaler.device, physicalDevice, scaler.renderer.graphicsQueue,
		scaler.renderer.graphicsFamily, pc98Width, pc98Height
	);

	VulkanTexture logTexture(
		scaler.device, physicalDevice, scaler.renderer.graphicsQueue,
		scaler.renderer.graphicsFamily, 1024, 1024
	);

	VulkanDescriptorPool descriptorPool(scaler.device, 4);

	VulkanDescriptorSet descriptorSetMain(
		scaler.device,
		mainTexture.textureView,
		scaler.renderer.sampler,
		descriptorPool,
		scaler.renderer.descriptorLayout
	);

	VulkanDescriptorSet descriptorSetLog(
		scaler.device,
		logTexture.textureView,
		scaler.renderer.sampler,
		descriptorPool,
		scaler.renderer.descriptorLayout
	);

	VulkanDescriptorPoolExt descriptorPoolExt(scaler.device, 12);

	VulkanDescriptorSetExt descriptorSetExt(
		scaler.device,
		physicalDevice,
		logTexture.textureView,
		scaler.renderer.sampler,
		descriptorPoolExt,
		scaler.renderer.descriptorLayoutExt
	);

	Pen pen(logTexture.image, freetypeFace, hbfont);

	std::vector<TextDims> texts;

	TextDims dims = drawFilename(pen, "FDD 0: ", cfg.fdd[0]);
	TextDims dims2 = drawFilename(pen, "FDD 1: ", cfg.fdd[1]);

	TextDims dims3 = drawFilename(pen, "FDD 2: ", cfg.fdd[2]);
	drawFilename(pen, "FDD 3: ", cfg.fdd[3]);

	drawFilename(pen, "HDD 0: ", cfg.sasihdd[0]);
	drawFilename(pen, "HDD 1: ", cfg.sasihdd[1]);
	drawFilename(pen, "Font: ",  cfg.fontfile);

	logTexture.textureDirty = true;
	bool showLog = false;

	CallbackContext ctx{
		&mainTexture,
		&scaler.context.glfwCtx.input
	};

	VulkanVtxBuffer vtxBuffer(scaler.device, physicalDevice, 1024);
	VulkanVtxBuffer uvBuffer(scaler.device, physicalDevice, 1024);

	Vec2 pos[6] {
		{-1.0, 1.0},  // lower left
		{-1.0,-1.0},  // upper left
		{ 1.0,-1.0},  // upper right

		{-1.0, 1.0},  // lower left
		{ 1.0,-1.0},  // upper right
		{ 1.0, 1.0}   // lower right
	};

	UVs uvs = calculateUvs(dims2, logTexture.image);

	vtxBuffer.update((const char *)pos, 0, sizeof(pos));
	uvBuffer.update((const char *)uvs.uvs, 0, sizeof(uvs));

	Matrix4x4f model = Matrix4x4f::ident();
	model.sx() /= (16.f / 9.f);

	descriptorSetExt.updateWorldMatrix(Matrix4x4f::ident());
	descriptorSetExt.updateModelMatrix(0, model);

	ViewPortMode mode = ViewPortMode::INTEGER;

	std::vector<VulkanCmbBuffer *> cmbBuffers;
	cmbBuffers.push_back(&vtxBuffer);
	cmbBuffers.push_back(&uvBuffer);
	cmbBuffers.push_back(&descriptorSetExt.uniformBuffer);

	while(scaler.getWindowState() != WindowState::SHOULDCLOSE && !sfd.isTriggered()){
		mainloop(&ctx);

		if(scaler.renderingComplete()){
			mainTexture.update();
			logTexture.update();

			renderBuffer = scaler.newRenderBuffer();
			scaler.pollWindowEvents();

			renderBuffer->begin(scaler.getRenderPass(), scaler.swapchain);
			//scaler.renderer.pipelineV->record(*renderBuffer, 6);

			if(showLog){
				//scaler.renderer.pipelineAspect1to1->record(*renderBuffer, descriptorSetLog, 6);
				scaler.renderer.pipelineStretchExt->record(
					*renderBuffer, descriptorSetExt,
					vtxBuffer, 0,
					uvBuffer, 0,
					6);
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

		handleInput(input, mode, showLog);

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



