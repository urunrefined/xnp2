#ifndef	NP2_X11_SCRNMNG_H__
#define	NP2_X11_SCRNMNG_H__

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
	RGB24_B	= 2,
	RGB24_G	= 1,
	RGB24_R	= 0
};

typedef struct {
	UINT8	*ptr;
	int		xalign;
	int		yalign;
	int		width;
	int		height;
	UINT	bpp;
} SCRNSURF;

enum {
	SCRNMODE_HIGHCOLOR	= 0x02
};

enum {
	SCRNFLAG_FULLSCREEN	= 0x01,
	SCRNFLAG_HAVEEXTEND	= 0x02,
};

enum ScaleMode {
	SCALEMODE_INTEGER        = 0x01,
	SCALEMODE_STRETCH        = 0x02,
	SCALEMODE_STRETCH_ASPECT = 0x03
};


typedef struct {
	UINT8	flag;
	UINT8	bpp;
	UINT8	palchanged;
} SCRNMNG;

extern SCRNMNG *scrnmngp;

void scrnmng_initialize(void);
BRESULT scrnmng_create(UINT8 scrnmode);
void scrnmng_destroy(void);

void scrnmng_setmultiple(int multiple);
void scrnmng_setScaleMode(enum ScaleMode scale_mode);

const SCRNSURF* scrnmng_surflock(void *ptr);
void scrnmng_surfunlock(void *ptr, const SCRNSURF *surf);

void scrnmng_renewal();

#define	scrnmng_haveextend()	(scrnmngp->flag & SCRNFLAG_HAVEEXTEND)
#define	scrnmng_getbpp()	(scrnmngp->bpp)
#define	scrnmng_palchanged()	do { scrnmngp->palchanged = TRUE; } while (0)

#ifdef __cplusplus
}
#endif

#endif	/* NP2_X11_SCRNMNG_H__ */
