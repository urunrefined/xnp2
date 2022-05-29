INC=-I.  -I./.. -I./../x11/vk -I./../x11/debug -I./../common -I./../cbus -I./../generic -I./../io -I./../mem -I./../sound -I./../vram
INCEXT=-I/usr/include/SDL2 -I/usr/include/fontconfig -I/usr/include/freetype2 -I/usr/include/harfbuzz

LDLIBS = -pthread -lSDL2 -lm -lglfw -lvulkan -lfontconfig -lharfbuzz -lfreetype

#General flags
CXXFLAGS = -Wall -Wextra -std=gnu++17 -pthread -DNDEBUG -fsigned-char -fno-strict-aliasing -g -O2
CFLAGS   = -Wall -Wextra -std=gnu99   -pthread -DNDEBUG -fsigned-char -fno-strict-aliasing -g -O2

SRC_C = \
		./../x11/np2.c                                      \
		./../x11/dosio.c                                    \
		./../x11/ini.c                                      \
		./../x11/trace.c                                    \
		./../x11/cmmidi.c                                   \
		./../x11/cmserial.c                                 \
		./../x11/drawmng.c                                  \
		./../x11/fontmng.c                                  \
		./../x11/commng.c                                   \
		./../x11/timemng.c                                  \
		./../pccore.c                                       \
		./../global.c                                       \
		./../nevent.c                                       \
		./../calendar.c                                     \
		./../timing.c                                       \
		./../statsave.c                                     \
		./../keystat.c                                      \
		./../common/strres.c                                \
		./../common/milstr.c                                \
		./../common/_memory.c                               \
		./../common/textfile.c                              \
		./../common/profile.c                               \
		./../common/lstarray.c                              \
		./../common/bmpdata.c                               \
		./../common/mimpidef.c                              \
		./../common/parts.c                                 \
		./../common/wavefile.c                              \
		./../codecnv/sjisucs2.c                             \
		./../codecnv/ucs2sjis.c                             \
		./../bios/bios.c                                    \
		./../bios/bios09.c                                  \
		./../bios/bios0c.c                                  \
		./../bios/bios12.c                                  \
		./../bios/bios13.c                                  \
		./../bios/bios18.c                                  \
		./../bios/bios19.c                                  \
		./../bios/bios1a.c                                  \
		./../bios/bios1b.c                                  \
		./../bios/bios1c.c                                  \
		./../bios/bios1f.c                                  \
		./../bios/sxsibios.c                                \
		./../cbus/cbuscore.c                                \
		./../cbus/pc9861k.c                                 \
		./../cbus/mpu98ii.c                                 \
		./../cbus/amd98.c                                   \
		./../cbus/board118.c                                \
		./../cbus/board14.c                                 \
		./../cbus/board26k.c                                \
		./../cbus/board86.c                                 \
		./../cbus/boardso.c                                 \
		./../cbus/boardspb.c                                \
		./../cbus/boardx2.c                                 \
		./../cbus/cs4231io.c                                \
		./../cbus/pcm86io.c                                 \
		./../cbus/sasiio.c                                  \
		./../cbus/scsiio.c                                  \
		./../cbus/scsicmd.c                                 \
		./../cbus/ideio.c                                   \
		./../cbus/atapicmd.c                                \
		./../fdd/diskdrv.c                                  \
		./../fdd/newdisk.c                                  \
		./../fdd/fddfile.c                                  \
		./../fdd/fdd_xdf.c                                  \
		./../fdd/fdd_d88.c                                  \
		./../fdd/fdd_mtr.c                                  \
		./../fdd/sxsi.c                                     \
		./../fdd/sxsihdd.c                                  \
		./../fdd/sxsicd.c                                   \
		./../font/font.c                                    \
		./../font/fontdata.c                                \
		./../font/fontmake.c                                \
		./../font/fontpc88.c                                \
		./../font/fontpc98.c                                \
		./../font/fontv98.c                                 \
		./../font/fontfm7.c                                 \
		./../font/fontx1.c                                  \
		./../font/fontx68k.c                                \
		./../generic/cmjasts.c                              \
		./../generic/hostdrv.c                              \
		./../generic/hostdrvs.c                             \
		./../generic/unasm.c                                \
		./../generic/np2info.c                              \
		./../generic/memdbg32.c                             \
		./../io/iocore.c                                    \
		./../io/artic.c                                     \
		./../io/cgrom.c                                     \
		./../io/cpuio.c                                     \
		./../io/crtc.c                                      \
		./../io/dipsw.c                                     \
		./../io/dmac.c                                      \
		./../io/egc.c                                       \
		./../io/emsio.c                                     \
		./../io/fdc.c                                       \
		./../io/fdd320.c                                    \
		./../io/gdc.c                                       \
		./../io/gdc_sub.c                                   \
		./../io/gdc_pset.c                                  \
		./../io/mouseif.c                                   \
		./../io/nmiio.c                                     \
		./../io/np2sysp.c                                   \
		./../io/pic.c                                       \
		./../io/pit.c                                       \
		./../io/printif.c                                   \
		./../io/serial.c                                    \
		./../io/sysport.c                                   \
		./../io/upd4990.c                                   \
		./../io/necio.c                                     \
		./../io/epsonio.c                                   \
		./../io/pcidev.c                                    \
		./../lio/lio.c                                      \
		./../lio/gscreen.c                                  \
		./../lio/gpset.c                                    \
		./../lio/gline.c                                    \
		./../lio/gcircle.c                                  \
		./../lio/gput1.c                                    \
		./../mem/dmav30.c                                   \
		./../mem/dmax86.c                                   \
		./../mem/memegc.c                                   \
		./../mem/memems.c                                   \
		./../mem/memepp.c                                   \
		./../mem/memtram.c                                  \
		./../mem/memvga.c                                   \
		./../mem/memvram.c                                  \
		./../sound/sound.c                                  \
		./../sound/soundrom.c                               \
		./../sound/s98.c                                    \
		./../sound/adpcmc.c                                 \
		./../sound/adpcmg.c                                 \
		./../sound/beepc.c                                  \
		./../sound/beepg.c                                  \
		./../sound/cs4231c.c                                \
		./../sound/cs4231g.c                                \
		./../sound/fmboard.c                                \
		./../sound/oplgenc.c                                \
		./../sound/oplgeng.c                                \
		./../sound/opngenc.c                                \
		./../sound/opngeng.c                                \
		./../sound/opntimer.c                               \
		./../sound/pcm86c.c                                 \
		./../sound/pcm86g.c                                 \
		./../sound/pcmmix.c                                 \
		./../sound/psggenc.c                                \
		./../sound/psggeng.c                                \
		./../sound/rhythmc.c                                \
		./../sound/sndcsec.c                                \
		./../sound/tms3631c.c                               \
		./../sound/tms3631g.c                               \
		./../sound/vermouth/midiout.c                       \
		./../sound/vermouth/midimod.c                       \
		./../sound/vermouth/midinst.c                       \
		./../sound/vermouth/midvoice.c                      \
		./../sound/vermouth/midtable.c                      \
		./../vram/vram.c                                    \
		./../vram/scrndraw.c                                \
		./../vram/sdraw.c                                   \
		./../vram/dispsync.c                                \
		./../vram/palettes.c                                \
		./../vram/maketext.c                                \
		./../vram/maketgrp.c                                \
		./../vram/makegrph.c                                \
		./../vram/makegrex.c                                \
		./../debugsub.c

SRC_CPP = \
                ./../x11/main.cpp                                   \
		./../x11/loop.cpp                                   \
		./../x11/inicfg.cpp                                 \
		./../x11/exception.cpp                              \
		./../x11/pccorecallbacks.cpp                        \
		./../x11/inputhandling.cpp                          \
		./../x11/signalFD.cpp                               \
		./../x11/soundmng.cpp                               \
		./../x11/misc/threadbase.cpp                        \
		./../x11/misc/tty.cpp                               \
		./../x11/misc/usbdev.cpp                            \
		./../x11/ext/opl3.cpp                               \
		./../x11/ext/opna.cpp                               \
		./../x11/ext/c86ctl/c86ctlc86box.cpp                \
		./../x11/ext/c86ctl/c86ctlgimic.cpp                 \
		./../x11/ext/c86ctl/c86ctlif.cpp                    \
		./../x11/ext/c86ctl/c86ctlrealchipbase.cpp          \
		./../x11/ext/scci/scciif.cpp                        \
		./../x11/ext/scci/sccisoundchip.cpp                 \
		./../x11/ext/scci/sccisoundinterface.cpp            \
		./../x11/ext/scci/sccisoundinterfacemanager.cpp     \
		./../x11/ext/scci/sccispfmlight.cpp                 \
		./../x11/ext/externalchipmanager.cpp                \
		./../x11/ext/externalopl3.cpp                       \
		./../x11/ext/externalopm.cpp                        \
		./../x11/ext/externalopna.cpp                       \
		./../x11/ext/externalpsg.cpp                        \
		./../x11/vk/Input.cpp                               \
		./../x11/vk/VKGLFW.cpp                              \
		./../x11/vk/VKInstance.cpp                          \
		./../x11/vk/VKDebug.cpp                             \
		./../x11/vk/VKPhysicalDevice.cpp                    \
		./../x11/vk/VKCommandPool.cpp                       \
		./../x11/vk/VKEngine.cpp                            \
		./../x11/vk/VKUtil.cpp                              \
		./../x11/vk/VKSurface.cpp                           \
		./../x11/vk/VKDevice.cpp                            \
		./../x11/vk/VKImage.cpp                             \
		./../x11/vk/VKImageView.cpp                         \
		./../x11/vk/VKImageMemory.cpp                       \
		./../x11/vk/VKFramebuffer.cpp                       \
		./../x11/vk/VKSwapchain.cpp                         \
		./../x11/vk/VKRenderPass.cpp                        \
		./../x11/vk/VKSync.cpp                              \
		./../x11/vk/VKPhysicalDeviceEnumerations.cpp        \
		./../x11/vk/VKSwapchainImages.cpp                   \
		./../x11/vk/VKRenderBuffer.cpp                      \
		./../x11/vk/VKShaders.cpp                           \
		./../x11/vk/VKPipelineV.cpp                         \
		./../x11/vk/VKPipelineTex.cpp                       \
		./../x11/vk/VKPipelineTexExt.cpp                    \
		./../x11/vk/VKBuffers.cpp                           \
		./../x11/vk/VKSampler.cpp                           \
		./../x11/vk/VKRenderer.cpp                          \
		./../x11/vk/VKDescriptorSet.cpp                     \
		./../x11/vk/VKDescriptorSetExt.cpp                  \
		./../x11/vk/VKDescriptorPool.cpp                    \
		./../x11/vk/VKDescriptorPoolExt.cpp                 \
		./../x11/vk/VKCommandBuffer.cpp                     \
		./../x11/vk/VKTexture.cpp                           \
		./../x11/vk/FreeFont.cpp                            \
		./../x11/vk/Image.cpp                               \
		./../x11/vk/Vertex.cpp                              \
		./../x11/vk/Matrix4x4.cpp                           \
		./../x11/vk/Range.cpp
		
cputype?=i286

ifeq ($(cputype),i286)
SRC_C += \
                ./../i286c/i286c.c                                  \
		./../i286c/cpumem.c                                 \
		./../i286c/i286c_mn.c                               \
		./../i286c/i286c_ea.c                               \
		./../i286c/i286c_0f.c                               \
		./../i286c/i286c_8x.c                               \
		./../i286c/i286c_sf.c                               \
		./../i286c/i286c_f6.c                               \
		./../i286c/i286c_fe.c                               \
		./../i286c/i286c_rp.c                               \
		./../i286c/v30patch.c

INC += -I./../i286c

else
CXXFLAGS += -DCPUCORE_IA32
CFLAGS += -DCPUCORE_IA32

SRC_C += \
                ./../i386c/ia32/task.c                              \
		./../i386c/ia32/debug.c                             \
		./../i386c/ia32/inst_table.c                        \
		./../i386c/ia32/cpu.c                               \
		./../i386c/ia32/instructions/fpu.c                  \
		./../i386c/ia32/instructions/system_inst.c          \
		./../i386c/ia32/instructions/flag_ctrl.c            \
		./../i386c/ia32/instructions/seg_reg.c              \
		./../i386c/ia32/instructions/bin_arith.c            \
		./../i386c/ia32/instructions/misc_inst.c            \
		./../i386c/ia32/instructions/data_trans.c           \
		./../i386c/ia32/instructions/string_inst.c          \
		./../i386c/ia32/instructions/shift_rotate.c         \
		./../i386c/ia32/instructions/logic_arith.c          \
		./../i386c/ia32/instructions/bit_byte.c             \
		./../i386c/ia32/instructions/dec_arith.c            \
		./../i386c/ia32/instructions/ctrl_trans.c           \
		./../i386c/ia32/instructions/fpu/fpemul.c           \
		./../i386c/ia32/disasm.c                            \
		./../i386c/ia32/resolve.c                           \
		./../i386c/ia32/ia32.c                              \
		./../i386c/ia32/ctrlxfer.c                          \
		./../i386c/ia32/exception.c                         \
		./../i386c/ia32/interface.c                         \
		./../i386c/ia32/cpu_mem.c                           \
		./../i386c/ia32/groups.c                            \
		./../i386c/ia32/cpu_io.c                            \
		./../i386c/ia32/paging.c                            \
		./../i386c/ia32/segments.c                          \
		./../i386c/cpucore.c                                \
		./../i386c/cpumem.c

INC += -I./../i386c

endif

ifeq ($(VKDEBUG),1)
    CXXFLAGS += -DVKDEBUG=1
    CFLAGS += -DVKDEBUG=1
endif

OBJ = $(SRC_C:.c=.o) $(SRC_CPP:.cpp=.o)
DEP = $(SRC_C:.c=.d) $(SRC_CPP:.cpp=.d)

FRAGSHADERSRC = $(wildcard shader/*.frag)
VERTSHADERSRC = $(wildcard shader/*.vert)

VERTSHADEROBJ = $(VERTSHADERSRC:.vert=.spv)
FRAGSHADEROBJ = $(FRAGSHADERSRC:.frag=.spv)

BIN = xnp2

default: all

all: $(BIN) $(FRAGSHADEROBJ) $(VERTSHADEROBJ)

$(BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDLIBS) $^ -o $@

%.o: %.cpp
	$(CXX) -c $< $(CXXFLAGS) $(INC) $(INCEXT) -MMD -MP -o $@

%.o: %.c
	$(CC) -c $< $(CFLAGS) $(INC) $(INCEXT) -MMD -MP -o $@
	
%.spv: %.vert
	glslangValidator -V $< -o $@

%.spv: %.frag
	glslangValidator -V $< -o $@

.PHONY: clean

clean:
	$(RM) $(OBJ) $(DEP) $(BIN) $(FRAGSHADEROBJ) $(VERTSHADEROBJ)

-include $(DEP)
