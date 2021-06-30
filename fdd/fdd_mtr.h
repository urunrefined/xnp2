#pragma once

#include "common.h"
#include "fddfile.h"
#include "nevent.h"

typedef struct {
	int		busy;
	UINT8	head[4];
	UINT	nextevent;
	UINT8	curevent;
} _FDDMTR, *FDDMTR;


#ifdef __cplusplus
extern "C" {
#endif

extern	_FDDMTR		fddmtr;

void fdbiosout(NEVENTITEM item);

void fddmtr_initialize(void);
void fddmtr_callback(UINT time);
void fddmtr_seek(REG8 drv, REG8 c, UINT size);
void fddmtr_reset(void);

#define	fddmtrsnd_initialize(r)
#define	fddmtrsnd_bind()
#define	fddmtrsnd_deinitialize()

#ifdef __cplusplus
}
#endif

