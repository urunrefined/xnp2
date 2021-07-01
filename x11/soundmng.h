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

/*incr decr / should be between 0 and 1*/
void soundmng_increaseVol(double incr);
void soundmng_decreaseVol(double decr);
void soundmng_toggleMute(void);

BRESULT soundmng_initialize(void);
void soundmng_deinitialize(void);


#ifdef __cplusplus
} //#ifdef __cplusplus
#endif

#endif	/* NP2_X11_SOUNGMNG_H__ */
