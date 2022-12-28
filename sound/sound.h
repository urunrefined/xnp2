/**
 * @file	sound.h
 * @brief	Interface of the sound
 */

#pragma once

#include "common.h"

#ifndef SOUNDCALL
#define	SOUNDCALL
#endif

#if !defined(DISABLE_SOUND)

typedef void (SOUNDCALL * SOUNDCB)(void *hdl, SINT32 *pcm, UINT count);

typedef struct {
	UINT	rate;
	UINT32	hzbase;
	UINT32	clockbase;
	UINT32	minclock;
	UINT32	lastclock;
	void    *soundRef;
} SOUNDCFG;


#ifdef __cplusplus
extern "C" {
#endif

extern	SOUNDCFG	soundcfg;

BRESULT sound_create(void *soundRef);
void sound_destroy(void);

void sound_reset(void);
void sound_changeclock(void);
void sound_streamregist(void *hdl, SOUNDCB cbfn);

void sound_sync(void);

#if defined(SUPPORT_WAVEREC)
BRESULT sound_recstart(const OEMCHAR *filename);
void sound_recstop(void);
BOOL sound_isrecording(void);
#endif

#ifdef __cplusplus
}
#endif

#else

#define sound_pcmlock()		(NULL)
#define sound_pcmunlock(h)
#define sound_reset()
#define sound_changeclock()
#define sound_sync()

#endif

