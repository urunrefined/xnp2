#include "global.h"

NP2CFG	np2cfg = {
			0, 1, 0, 32, 0, 0, 0x40,
			0, 0, 0, 0,
			{0x3e, 0x73, 0x7b}, 0,
			0, 0, {1, 1, 6, 1, 8, 1},

			OEMTEXT("VX"), PCBASECLOCK25, PCBASEMULTIPLE,
			{0x48, 0x05, 0x04, 0x00, 0x01, 0x00, 0x00, 0x6e},
			1, 1, 2, 1, 0x000000, 0xffffff,
			44100, 250, 4, 0,
			{0, 0, 0}, 0xd1, 0x7f, 0xd1, 0, 0, 1,
			3, {0x0c, 0x0c, 0x08, 0x06, 0x03, 0x0c}, 64, 64, 64, 64, 64,
			1, 0x82,
			0, {0x17, 0x04, 0x1f}, {0x0c, 0x0c, 0x02, 0x10, 0x3f, 0x3f},
			3, 1, 80, 0,
			{OEMTEXT(""), OEMTEXT("")},
			{OEMTEXT(""), OEMTEXT(""), OEMTEXT(""), OEMTEXT("")},
#if defined(SUPPORT_SCSI)
			{OEMTEXT(""), OEMTEXT(""), OEMTEXT(""), OEMTEXT("")},
#endif
			OEMTEXT(""), OEMTEXT("")};
