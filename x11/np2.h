#ifndef NP2_X11_NP2_H__
#define NP2_X11_NP2_H__

#include "compiler.h"
#include <signal.h>

#ifdef __cplusplus
extern "C" {
#endif

UINT32 gettick(void);

typedef struct {
    char type[64];
    char ttyname[MAX_PATH];
    char alsaRawHWName[64];
} COMCFG;

typedef struct {
    UINT8 DISPCLK;

    UINT8 KEYBOARD;
    UINT8 F12KEY;

    UINT8 JOYPAD1;
    UINT8 JOYPAD2;
    char JOYDEV[2][MAX_PATH];

    COMCFG mpu;
    COMCFG com[3];

    UINT8 confirm;

    UINT8 statsave;
    UINT8 hostdrv_write;
    UINT8 jastsnd;

    char MIDIDEV[2][MAX_PATH];
    UINT32 MIDIWAIT;
} NP2OSCFG;

enum {
    SCREEN_WBASE = 80,
    SCREEN_HBASE = 50,
    SCREEN_DEFMUL = 8,
};

enum { MMXFLAG_DISABLE = 1, MMXFLAG_NOTSUPPORT = 2 };

void framereset();
void processwait(UINT cnt);
int mainloop(void *graphics, void *soundRef);

#ifdef __cplusplus
}
#endif

#endif /* NP2_X11_NP2_H__ */
