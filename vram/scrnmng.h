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

BRESULT scrnmng_create(UINT8 scrnmode);
SCRNSURF scrnmng_surflock(void *ptr);
void scrnmng_surfunlock(void *ptr);

void scrnmng_renewal();

#ifdef __cplusplus
}
#endif

#endif	/* NP2_X11_SCRNMNG_H__ */
