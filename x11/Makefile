INC=-I.  -I./.. -I./../x11 -I./../x11/gtk2 -I./../x11/debug -I./../common -I./../cbus -I./../generic -I./../io -I./../mem -I./../sound -I./../vram -I/usr/include/gtk-2.0 -I/usr/lib/gtk-2.0/include -I/usr/include/pango-1.0 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/harfbuzz -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/libmount -I/usr/include/blkid -I/usr/include/fribidi -I/usr/include/cairo -I/usr/include/lzo -I/usr/include/pixman-1 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/atk-1.0 -I/usr/include/SDL2 -I/usr/include/libusb-1.0 -I./../i286c

LDLIBS = -lgtk-x11-2.0 -lgdk-x11-2.0 -lpangocairo-1.0 -latk-1.0 -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lpangoft2-1.0 -lpango-1.0 -lgobject-2.0 -lglib-2.0 -lharfbuzz -lfontconfig -lfreetype  -pthread -lSDL2  -lSDL2_mixer -lusb-1.0 -lX11 -lXext -lSM -lICE -lXxf86vm -lm

#General flags
CXXFLAGS = -DHAVE_CONFIG_H -DSYSRESPATH=\""/usr/local/share/xnp2"\" -std=c++11 -pthread -DNDEBUG -fsigned-char -fno-strict-aliasing -g -O2
CFLAGS   = -DHAVE_CONFIG_H -DSYSRESPATH=\""/usr/local/share/xnp2"\"            -pthread -DNDEBUG -fsigned-char -fno-strict-aliasing -g -O2

SRC_C = \
		./../x11/main.c                                         \
		./../x11/main.c                                         \
		./../x11/np2.c                                          \
		./../x11/dosio.c                                        \
		./../x11/ini.c                                          \
		./../x11/oemtext.c                                      \
		./../x11/trace.c                                        \
		./../x11/kdispwin.c                                     \
		./../x11/skbdwin.c                                      \
		./../x11/cmmidi.c                                       \
		./../x11/cmserial.c                                     \
		./../x11/drawmng.c                                      \
		./../x11/fontmng.c                                      \
		./../x11/kbdmng.c                                       \
		./../x11/commng.c                                       \
		./../x11/joymng.c                                       \
		./../x11/sysmng.c                                       \
		./../x11/taskmng.c                                      \
		./../x11/timemng.c                                      \
		./../x11/gtk2/dialog_about.c                            \
		./../x11/gtk2/dialog_calendar.c                         \
		./../x11/gtk2/dialog_config.c                           \
		./../x11/gtk2/dialog_midi.c                             \
		./../x11/gtk2/dialog_newdisk.c                          \
		./../x11/gtk2/dialog_screen.c                           \
		./../x11/gtk2/dialog_sound.c                            \
		./../x11/gtk2/window_keydisp.c                          \
		./../x11/gtk2/window_softkbd.c                          \
		./../x11/gtk2/gtk_drawmng.c                             \
		./../x11/gtk2/gtk_screen.c                              \
		./../x11/gtk2/gtk_font.c                                \
		./../x11/gtk2/gtk_keyboard.c                            \
		./../x11/gtk2/gtk_menu.c                                \
		./../x11/gtk2/gtk_mouse.c                               \
		./../x11/gtk2/gtk_main.c                                \
		./../x11/gtk2/gtk_wrapper.c                             \
		./../x11/debug/viewmem.c                                \
		./../pccore.c                                           \
		./../nevent.c                                           \
		./../calendar.c                                         \
		./../timing.c                                           \
		./../statsave.c                                         \
		./../keystat.c                                          \
		./../common/strres.c                                    \
		./../common/milstr.c                                    \
		./../common/_memory.c                                   \
		./../common/textfile.c                                  \
		./../common/profile.c                                   \
		./../common/rect.c                                      \
		./../common/lstarray.c                                  \
		./../common/bmpdata.c                                   \
		./../common/mimpidef.c                                  \
		./../common/parts.c                                     \
		./../common/wavefile.c                                  \
		./../codecnv/eucsjis.c                                  \
		./../codecnv/eucucs2.c                                  \
		./../codecnv/sjiseuc.c                                  \
		./../codecnv/sjisucs2.c                                 \
		./../codecnv/tcswap16.c                                 \
		./../codecnv/tcswap32.c                                 \
		./../codecnv/textcnv.c                                  \
		./../codecnv/ucs2sjis.c                                 \
		./../codecnv/ucs2utf8.c                                 \
		./../codecnv/utf8ucs2.c                                 \
		./../bios/bios.c                                        \
		./../bios/bios09.c                                      \
		./../bios/bios0c.c                                      \
		./../bios/bios12.c                                      \
		./../bios/bios13.c                                      \
		./../bios/bios18.c                                      \
		./../bios/bios19.c                                      \
		./../bios/bios1a.c                                      \
		./../bios/bios1b.c                                      \
		./../bios/bios1c.c                                      \
		./../bios/bios1f.c                                      \
		./../bios/sxsibios.c                                    \
		./../cbus/cbuscore.c                                    \
		./../cbus/pc9861k.c                                     \
		./../cbus/mpu98ii.c                                     \
		./../cbus/amd98.c                                       \
		./../cbus/board118.c                                    \
		./../cbus/board14.c                                     \
		./../cbus/board26k.c                                    \
		./../cbus/board86.c                                     \
		./../cbus/boardso.c                                     \
		./../cbus/boardspb.c                                    \
		./../cbus/boardx2.c                                     \
		./../cbus/cs4231io.c                                    \
		./../cbus/pcm86io.c                                     \
		./../cbus/sasiio.c                                      \
		./../cbus/scsiio.c                                      \
		./../cbus/scsicmd.c                                     \
		./../cbus/ideio.c                                       \
		./../cbus/atapicmd.c                                    \
		./../fdd/diskdrv.c                                      \
		./../fdd/newdisk.c                                      \
		./../fdd/fddfile.c                                      \
		./../fdd/fdd_xdf.c                                      \
		./../fdd/fdd_d88.c                                      \
		./../fdd/fdd_mtr.c                                      \
		./../fdd/sxsi.c                                         \
		./../fdd/sxsihdd.c                                      \
		./../fdd/sxsicd.c                                       \
		./../font/font.c                                        \
		./../font/fontdata.c                                    \
		./../font/fontmake.c                                    \
		./../font/fontpc88.c                                    \
		./../font/fontpc98.c                                    \
		./../font/fontv98.c                                     \
		./../font/fontfm7.c                                     \
		./../font/fontx1.c                                      \
		./../font/fontx68k.c                                    \
		./../generic/cmndraw.c                                  \
		./../generic/keydisp.c                                  \
		./../generic/cmjasts.c                                  \
		./../generic/hostdrv.c                                  \
		./../generic/hostdrvs.c                                 \
		./../generic/unasm.c                                    \
		./../generic/np2info.c                                  \
		./../generic/memdbg32.c                                 \
		./../generic/softkbd.c                                  \
		./../io/iocore.c                                        \
		./../io/artic.c                                         \
		./../io/cgrom.c                                         \
		./../io/cpuio.c                                         \
		./../io/crtc.c                                          \
		./../io/dipsw.c                                         \
		./../io/dmac.c                                          \
		./../io/egc.c                                           \
		./../io/emsio.c                                         \
		./../io/fdc.c                                           \
		./../io/fdd320.c                                        \
		./../io/gdc.c                                           \
		./../io/gdc_sub.c                                       \
		./../io/gdc_pset.c                                      \
		./../io/mouseif.c                                       \
		./../io/nmiio.c                                         \
		./../io/np2sysp.c                                       \
		./../io/pic.c                                           \
		./../io/pit.c                                           \
		./../io/printif.c                                       \
		./../io/serial.c                                        \
		./../io/sysport.c                                       \
		./../io/upd4990.c                                       \
		./../io/necio.c                                         \
		./../io/epsonio.c                                       \
		./../io/pcidev.c                                        \
		./../lio/lio.c                                          \
		./../lio/gscreen.c                                      \
		./../lio/gpset.c                                        \
		./../lio/gline.c                                        \
		./../lio/gcircle.c                                      \
		./../lio/gput1.c                                        \
		./../mem/dmav30.c                                       \
		./../mem/dmax86.c                                       \
		./../mem/memegc.c                                       \
		./../mem/memems.c                                       \
		./../mem/memepp.c                                       \
		./../mem/memtram.c                                      \
		./../mem/memvga.c                                       \
		./../mem/memvram.c                                      \
		./../sound/sound.c                                      \
		./../sound/soundrom.c                                   \
		./../sound/s98.c                                        \
		./../sound/adpcmc.c                                     \
		./../sound/adpcmg.c                                     \
		./../sound/beepc.c                                      \
		./../sound/beepg.c                                      \
		./../sound/cs4231c.c                                    \
		./../sound/cs4231g.c                                    \
		./../sound/fmboard.c                                    \
		./../sound/oplgenc.c                                    \
		./../sound/oplgeng.c                                    \
		./../sound/opngenc.c                                    \
		./../sound/opngeng.c                                    \
		./../sound/opntimer.c                                   \
		./../sound/pcm86c.c                                     \
		./../sound/pcm86g.c                                     \
		./../sound/pcmmix.c                                     \
		./../sound/psggenc.c                                    \
		./../sound/psggeng.c                                    \
		./../sound/rhythmc.c                                    \
		./../sound/sndcsec.c                                    \
		./../sound/tms3631c.c                                   \
		./../sound/tms3631g.c                                   \
		./../sound/getsnd/getsnd.c                              \
		./../sound/getsnd/getsmix.c                             \
		./../sound/getsnd/getmp3.c                              \
		./../sound/getsnd/getogg.c                              \
		./../sound/getsnd/getwave.c                             \
		./../sound/vermouth/midiout.c                           \
		./../sound/vermouth/midimod.c                           \
		./../sound/vermouth/midinst.c                           \
		./../sound/vermouth/midvoice.c                          \
		./../sound/vermouth/midtable.c                          \
		./../vram/vram.c                                        \
		./../vram/scrndraw.c                                    \
		./../vram/sdraw.c                                       \
		./../vram/dispsync.c                                    \
		./../vram/palettes.c                                    \
		./../vram/maketext.c                                    \
		./../vram/maketgrp.c                                    \
		./../vram/makegrph.c                                    \
		./../vram/makegrex.c                                    \
		./../vram/scrnsave.c                                    \
		./../i286c/i286c.c                                      \
		./../i286c/cpumem.c                                     \
		./../i286c/i286c_mn.c                                   \
		./../i286c/i286c_ea.c                                   \
		./../i286c/i286c_0f.c                                   \
		./../i286c/i286c_8x.c                                   \
		./../i286c/i286c_sf.c                                   \
		./../i286c/i286c_f6.c                                   \
		./../i286c/i286c_fe.c                                   \
		./../i286c/i286c_rp.c                                   \
		./../i286c/v30patch.c                                   \
		./../debugsub.c                                         
      
SRC_CPP = \
		./../x11/soundmng.cpp                                   \
		./../x11/misc/threadbase.cpp                            \
		./../x11/misc/tty.cpp                                   \
		./../x11/misc/usbdev.cpp                                \
		./../x11/ext/opl3.cpp                                   \
		./../x11/ext/opna.cpp                                   \
		./../x11/ext/c86ctl/c86ctlc86box.cpp                    \
		./../x11/ext/c86ctl/c86ctlgimic.cpp                     \
		./../x11/ext/c86ctl/c86ctlif.cpp                        \
		./../x11/ext/c86ctl/c86ctlrealchipbase.cpp              \
		./../x11/ext/scci/scciif.cpp                            \
		./../x11/ext/scci/sccisoundchip.cpp                     \
		./../x11/ext/scci/sccisoundinterface.cpp                \
		./../x11/ext/scci/sccisoundinterfacemanager.cpp         \
		./../x11/ext/scci/sccispfmlight.cpp                     \
		./../x11/ext/externalchipmanager.cpp                    \
		./../x11/ext/externalopl3.cpp                           \
		./../x11/ext/externalopm.cpp                            \
		./../x11/ext/externalopna.cpp                           \
		./../x11/ext/externalpsg.cpp


OBJ = $(SRC_C:.c=.o) $(SRC_CPP:.cpp=.o)
DEP = $(SRC_C:.c=.d) $(SRC_CPP:.cpp=.d)

BIN = xnp2

default: $(BIN)

all: default

$(BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDLIBS) $^ -o $@

%.o: %.cpp
	$(CXX) -c $< $(CXXFLAGS) $(INC) -MMD -MP -o $@

%.o: %.c
	$(CC) -c $< $(CFLAGS) $(INC) -MMD -MP -o $@

.PHONY: clean

clean:
	$(RM) $(OBJ) $(DEP) $(BIN)

-include $(DEP)
