#ifndef	NP2_X11_SOUNGMNG_H__
#define	NP2_X11_SOUNGMNG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "compiler.h"

enum {
	SOUND_PCMSEEK,
	SOUND_PCMSEEK1,
	SOUND_MAXPCM
};

enum {
	SNDDRV_NODRV,
	SNDDRV_SDL,
	SNDDRV_DRVMAX
};

UINT8 snddrv_drv2num(const char *);
const char *snddrv_num2drv(UINT8);

UINT soundmng_create(UINT rate, UINT ms);
void soundmng_destroy(void);
void soundmng_reset(void);
void soundmng_play(void);
void soundmng_stop(void);
void soundmng_sync(void);
void soundmng_setreverse(BOOL reverse);

BRESULT soundmng_pcmplay(UINT num, BOOL loop);
void soundmng_pcmstop(UINT num);

/* ---- for X11 */

BRESULT soundmng_initialize(void);
void soundmng_deinitialize(void);

BRESULT soundmng_pcmload(UINT num, const char *filename);
void soundmng_pcmvolume(UINT num, int volume);

extern int pcm_volume_default;

#ifdef __cplusplus
} //#ifdef __cplusplus
#endif

#endif	/* NP2_X11_SOUNGMNG_H__ */
