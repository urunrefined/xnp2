#include	"compiler.h"
#include	"soundmng.h"
#include	"pccore.h"
#include	"fdd_mtr.h"
#include	"np2.h"

// ----

enum {
	MOVE1TCK_MS		= 15,
	MOVEMOTOR1_MS	= 25,
	DISK1ROL_MS		= 166
};

	_FDDMTR		fddmtr;

static void fddmtr_event(void) {

	switch(fddmtr.curevent) {
		case 100:
			fddmtr.curevent = 0;
			break;

		default:
			fddmtr.curevent = 0;
			break;
	}
}

void fddmtr_initialize(void) {
	ZeroMemory(&fddmtr, sizeof(fddmtr));
	FillMemory(fddmtr.head, sizeof(fddmtr.head), 42);
}

void fddmtr_callback(UINT nowtime) {

	if ((fddmtr.curevent) && (nowtime >= fddmtr.nextevent)) {
		fddmtr_event();
	}
}

void fdbiosout(NEVENTITEM item) {

	fddmtr.busy = 0;
	(void)item;
}

void fddmtr_seek(REG8 drv, REG8 c, UINT size) {

	int		regmove;
	SINT32	waitcnt;

	drv &= 3;
	regmove = c - fddmtr.head[drv];
	fddmtr.head[drv] = c;

	if (!np2cfg.MOTOR) {
		if (size) {
			fddmtr.busy = 1;
			nevent_set(NEVENT_FDBIOSBUSY, size * pccore.multiple,
												fdbiosout, NEVENT_ABSOLUTE);
		}
		return;
	}

	waitcnt = (size * DISK1ROL_MS) / (1024 * 8);
	if (regmove < 0) {
		regmove = 0 - regmove;
	}
	if (regmove == 1) {
		if (fddmtr.curevent < 80) {
			fddmtr_event();
			fddmtr.curevent = 80;
			fddmtr.nextevent = gettick() + MOVEMOTOR1_MS;
		}
	}
	else if (regmove) {
		if (fddmtr.curevent < 100) {
			fddmtr_event();
			fddmtr.curevent = 100;
			fddmtr.nextevent = gettick() + (regmove * MOVE1TCK_MS);
		}
		if (regmove >= 32) {
			waitcnt += DISK1ROL_MS;
		}
	}
	if (waitcnt) {
		fddmtr.busy = 1;
		nevent_setbyms(NEVENT_FDBIOSBUSY,
										waitcnt, fdbiosout, NEVENT_ABSOLUTE);
	}
	(void)drv;
}

void fddmtr_reset(void) {

	fddmtr.busy = 0;
	nevent_reset(NEVENT_FDBIOSBUSY);
}

