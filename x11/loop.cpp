#include "vk/Displaylist.h"
#include "vk/FreeFont.h"
#include "vk/VKDescriptorPool.h"
#include "vk/VKEngine.h"
#include "vk/VKPhysicalDeviceEnumerations.h"

#include "pulse/PulseSoundEngine.h"

#include "Matrix4x4.h"
#include "Vertex.h"
#include "exception.h"
#include "inputhandling.h"
#include "loop.h"
#include "np2.h"
#include "pccore.h"
#include "pccorecallbacks.h"
#include "text.h"

namespace BR {

static VulkanPhysicalDevice glPhysicalDeviceSelection(VulkanContext &engine) {
    BR::VulkanPhysicalDeviceEnumerations physicalDeviceEnums(engine.instance);

    for (BR::VulkanPhysicalDevice &physicalDevice :
         physicalDeviceEnums.physicalDevices) {
        // Use the first one available

        if (physicalDevice.isDeviceSuitable(engine.surface)) {
            return physicalDevice;
        }
    }

    throw Exception("No graphics device suitable");
}

static const unsigned int pc98Width = 640;
static const unsigned int pc98Height = 400;

Vec2 posstatic[6]{
    {-1.0, 1.0},  // lower left
    {-1.0, -1.0}, // upper left
    {1.0, -1.0},  // upper right

    {-1.0, 1.0}, // lower left
    {1.0, -1.0}, // upper right
    {1.0, 1.0}   // lower right
};

class Column {
  public:
    std::vector<std::string> elements;

    Column() {}

    void add(const std::string &element) { elements.push_back(element); }
};

// Column

static float getFittingScale(const Dimensions &in, const Dimensions &limits) {

    float scaleX = (limits.x / in.x);
    float scaleY = (limits.y / in.y);

    if (scaleX < scaleY) {
        return scaleX;
    } else {
        return scaleY;
    }
}

static char hexmap[16]{'0', '1', '2', '3', '4', '5', '6', '7',
                       '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

template <size_t N>
static void toHex(unsigned char (&in)[N], char (&out)[N * 2]) {
    for (size_t i = 0; i < N; i++) {
        out[i * 2] = hexmap[(in[i] >> 4) & 0b00001111];
        out[i * 2 + 1] = hexmap[in[i] & 0b00001111];
    }
}

class Menu {
  public:
    Column left, right;

    Menu() {}

    template <size_t N>
    void addHex(const char *leftString, unsigned char (&arr)[N]) {
        char buf[N * 2 + 1];
        toHex(arr, (char(&)[N * 2]) buf);
        buf[N * 2] = '\0';

        left.add(leftString);
        right.add(buf);
    }

    // automatic promotion
    void add(const char *leftString, UINT64 val) {
        char buf[std::numeric_limits<decltype(val)>::digits + 1];

        snprintf(buf, sizeof(buf), "%ld", val);

        left.add(leftString);
        right.add(buf);
    }

    void add(const char *leftString, const char *rightString) {
        left.add(leftString);
        right.add(rightString);
    }
};

class TextBuffer {
    TextPrep getTextPrep(float &top, TextCache &textCache,
                         const std::string &element) {
        Text text = textCache.query(element.c_str());
        auto &dims = text.dims;

        float t = top + dims.sizeY;
        float b = top;
        top = t;

        Vec2 ul = {0, t};
        Vec2 ur = {(float)dims.sizeX, t};
        Vec2 ll = {0, b};
        Vec2 lr = {(float)dims.sizeX, b};

        return {text.texture, text.dims, getTriangleList(ul, ur, ll, lr)};
    }

  public:
    std::vector<TextPrepMat> buffer;

    void add(TextCache &textCache, Menu &menu) {
        std::vector<TextPrep> left;
        std::vector<TextPrep> right;

        float top = 0;

        for (auto &element : menu.left.elements) {
            left.push_back(getTextPrep(top, textCache, element.c_str()));
        }

        top = 0;

        for (auto &element : menu.right.elements) {
            right.push_back(getTextPrep(top, textCache, element.c_str()));
        }

        assert(menu.left.elements.size() == menu.right.elements.size());

        Dimensions dims;

        for (auto &e : left) {
            for (auto &vtx : e.vtxs.vtxs) {
                dims.x = std::max(dims.x, vtx.x);
                dims.y = std::max(dims.y, vtx.y);
            }
        }

        for (auto &e : right) {
            for (auto &vtx : e.vtxs.vtxs) {
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

static void glLoop(SignalFD &sfd, InputMapper &inputMapper,
                   VulkanContext &engine, VulkanPhysicalDevice &physicalDevice,
                   HarfbuzzFont &hbfont, FreetypeFace &freetypeFace,
                   Sfx::PulseSoundEngine &soundEngine, NP2CFG &cfg,
                   NP2OSCFG &oscfg) {
    VisualScreen visualScreen = VisualScreen::MAIN;
    DoubleLines doubleLines = DoubleLines::NO;

    VulkanScaler scaler(engine, physicalDevice);
    std::unique_ptr<VulkanRenderBuffer> renderBuffer;

    VulkanTexture mainTexture(
        scaler.device, physicalDevice, scaler.renderer.graphicsQueue,
        scaler.renderer.graphicsFamily, pc98Width, pc98Height);

    VulkanDescriptorPool descriptorPool(scaler.device, 1);

    VulkanDescriptorSet descriptorSetMain(
        scaler.device, mainTexture.textureView, scaler.renderer.sampler,
        descriptorPool, scaler.renderer.descriptorLayout);

    CallbackContext ctx{&mainTexture, &scaler.context.glfwCtx.input};

    TextCache textCache(scaler.device, physicalDevice,
                        scaler.device.graphicsQueue,
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
    menu.add("color16: ", cfg.color16);
    menu.add("BTN_MODE: ", cfg.BTN_MODE);
    menu.add("LCD_MODE: ", cfg.LCD_MODE);
    menu.add("KEY_MODE: ", cfg.KEY_MODE);

    menu.add("model: ", cfg.model);
    menu.add("BTN_RAPID: ", cfg.BTN_RAPID);
    menu.add("DISPSYNC: ", cfg.DISPSYNC);
    menu.add("EXTMEM: ", cfg.EXTMEM);
    menu.add("ITF_WORK: ", cfg.ITF_WORK);
    menu.add("MOTOR: ", cfg.MOTOR);
    menu.addHex("wait: ", cfg.wait);
    menu.add("PROTECTMEM: ", cfg.PROTECTMEM);
    menu.add("SOUND_SW: ", cfg.SOUND_SW);
    menu.add("vol_adpcm", cfg.vol_adpcm);
    menu.add("vol_fm", cfg.vol_fm);
    menu.add("vol_pcm", cfg.vol_pcm);
    menu.add("vol_rhythm", cfg.vol_rhythm);
    menu.add("vol_ssg", cfg.vol_ssg);
    menu.add("vol_ssg", cfg.vol_ssg);
    menu.addHex("vol14", cfg.vol14);
    menu.add("usefd144", cfg.usefd144);
    menu.add("uPD72020", cfg.uPD72020);
    menu.add("spbopt", cfg.spbopt);

    TextBuffer textBuffer;
    textBuffer.add(textCache, menu);

    TextDisplayTable displayTable(textBuffer.buffer);

    TextDisplayList displayList(
        scaler.device, physicalDevice, scaler.renderer.sampler,
        scaler.renderer.descriptorLayoutExt, displayTable.map);

    ViewPortMode mode = ViewPortMode::INTEGER;

    std::vector<VulkanCmbBuffer *> cmbBuffers;
    displayList.chainBuffers(cmbBuffers);
    textCache.update();

    while (scaler.getWindowState() != WindowState::SHOULDCLOSE &&
           !sfd.isTriggered()) {
        mainloop(&ctx, &soundEngine);

        if (scaler.renderingComplete()) {

            mainTexture.update(doubleLines);

            renderBuffer = scaler.newRenderBuffer();
            scaler.pollWindowEvents();

            renderBuffer->begin(scaler.getRenderPass(), scaler.swapchain);

            if (visualScreen == VisualScreen::CFG) {
                displayList.render(scaler.renderer, *renderBuffer);
            } else {
                if (mode == ViewPortMode::ASPECT) {
                    scaler.renderer.pipelineAspect->record(
                        *renderBuffer, descriptorSetMain, 6);
                } else if (mode == ViewPortMode::STRETCH) {
                    scaler.renderer.pipelineStretch->record(
                        *renderBuffer, descriptorSetMain, 6);
                } else {
                    scaler.renderer.pipelineInteger->record(
                        *renderBuffer, descriptorSetMain, 6);
                }
            }

            renderBuffer->end();

            if (scaler.drawAndPresent(*renderBuffer, cmbBuffers) ==
                RenderState::NEEDSSWAPCHAINUPDATE) {
                scaler.recreateSwapChain();
            }
        }

        GLFWInput &input = engine.glfwCtx.getInput();

        inputMapper.handleInput(input, mode, visualScreen, doubleLines,
                                soundEngine);

        input.reset();
    }

    while (!scaler.renderingComplete()) {
        usleep(1000);
    }

    vkDeviceWaitIdle(scaler.device);
}

void loop(SignalFD &sfd, InputMapper &inputMapper, NP2CFG &cfg, NP2OSCFG &oscfg,
          Sfx::PulseSoundEngine &soundEngine) {
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

    glLoop(sfd, inputMapper, engine, physicalDevice, hbfont, freetypeFace,
           soundEngine, cfg, oscfg);
}

} // namespace BR
