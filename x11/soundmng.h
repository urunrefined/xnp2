#ifndef	NP2_X11_SOUNGMNG_H__
#define	NP2_X11_SOUNGMNG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "compiler.h"
#include "common.h"

UINT soundmng_create(UINT rate, UINT ms);
void soundmng_destroy(void);
void soundmng_reset(void);
void soundmng_play(void);
void soundmng_stop(void);
void soundmng_sync(void);

BRESULT soundmng_initialize(void);
void soundmng_deinitialize(void);

#ifdef __cplusplus
} //#ifdef __cplusplus
#endif

#endif	/* NP2_X11_SOUNGMNG_H__ */
