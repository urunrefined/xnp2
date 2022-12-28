#ifndef	NP2_X11_SOUNGMNG_H__
#define	NP2_X11_SOUNGMNG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "compiler.h"
#include "common.h"

void soundmng_sync(void *soundRef, int index, const SINT32 *pcm, size_t sz);
void soundmng_addStream(const void *soundRef, const char *name, int index);
UINT soundmng_getRate(void *soundRef);

/*incr decr / should be between 0 and 1*/
void soundmng_increaseVol(double incr);
void soundmng_decreaseVol(double decr);
void soundmng_toggleMute(void);

BRESULT soundmng_initialize(void);
void soundmng_deinitialize(void);
void soundmng_reset(void *soundRef);


#ifdef __cplusplus
} //#ifdef __cplusplus
#endif

#endif	/* NP2_X11_SOUNGMNG_H__ */
