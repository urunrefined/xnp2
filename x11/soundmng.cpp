/*
 * Copyright (c) 2001-2003, 2015 NONAKA Kimihiro
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "compiler.h"
#include "soundmng.h"
#include "np2.h"
#include "pccore.h"
#include "ini.h"
#include "dosio.h"
#include "parts.h"
#include "sound.h"
#include "_memory.h"
#include "sound/vermouth/vermouth.h"

#include <sys/stat.h>
#include <strings.h>

#include <SDL.h>


extern "C" MIDIMOD vermouth_module;
MIDIMOD vermouth_module = NULL;

static int audio_fd = -1;
static BOOL opened = FALSE;
static UINT opna_frame;
static double masterVolume = 1;
static bool mute = false;

/*
 * buffer
 */
#ifndef	NSOUNDBUFFER
#define	NSOUNDBUFFER	4
#endif
static struct sndbuf {
	struct sndbuf *next;
	char *buf;
	UINT size;
	UINT remain;
} sound_buffer[NSOUNDBUFFER];

static struct sndbuf *sndbuf_freelist;
#define	SNDBUF_FREELIST_FIRST()		(sndbuf_freelist)
#define	SNDBUF_FREELIST_INIT()						\
do {									\
	sndbuf_freelist = NULL;						\
} while (/*CONSTCOND*/0)
#define	SNDBUF_FREELIST_INSERT_HEAD(sndbuf)				\
do {									\
	(sndbuf)->next = sndbuf_freelist;				\
	sndbuf_freelist = (sndbuf);					\
} while (/*CONSTCOND*/0)
#define	SNDBUF_FREELIST_REMOVE_HEAD()					\
do {									\
	sndbuf_freelist = sndbuf_freelist->next;			\
} while (/*CONSTCOND*/0)

static struct {
	struct sndbuf *first;
	struct sndbuf **last;
} sndbuf_filled;
#define	SNDBUF_FILLED_QUEUE_FIRST()	(sndbuf_filled.first)
#define	SNDBUF_FILLED_QUEUE_INIT()					\
do {									\
	sndbuf_filled.first = NULL;					\
	sndbuf_filled.last = &sndbuf_filled.first;			\
} while (/*CONSTCOND*/0)
#define	SNDBUF_FILLED_QUEUE_INSERT_HEAD(sndbuf)				\
do {									\
	if (((sndbuf)->next = sndbuf_filled.first) == NULL)		\
		sndbuf_filled.last = &(sndbuf)->next;			\
	sndbuf_filled.first = (sndbuf);					\
} while (/*CONSTCOND*/0)
#define	SNDBUF_FILLED_QUEUE_INSERT_TAIL(sndbuf)				\
do {									\
	(sndbuf)->next = NULL;						\
	*sndbuf_filled.last = (sndbuf);					\
	sndbuf_filled.last = &(sndbuf)->next;				\
} while (/*CONSTCOND*/0)
#define	SNDBUF_FILLED_QUEUE_REMOVE_HEAD()				\
do {									\
	sndbuf_filled.first = sndbuf_filled.first->next;		\
	if (sndbuf_filled.first == NULL)				\
		sndbuf_filled.last = &sndbuf_filled.first;		\
} while (/*CONSTCOND*/0)

#define	sndbuf_lock()
#define	sndbuf_unlock()

static BRESULT buffer_init(void);
static void buffer_destroy(void);
static void buffer_clear(void);

static UINT
calc_blocksize(UINT size)
{
	UINT s = size;

	if (size & (size - 1))
		for (s = 32; s < size; s <<= 1)
			continue;
	return s;
}

static void sdlaudio_callback(void *, unsigned char *, int);

static UINT8 sound_silence;

static void
sdlaudio_callback(void *userdata, unsigned char *stream, int len)
{
	(void) userdata;

	struct sndbuf *sndbuf;


	/* SDL2 から SDL 側で stream を無音で初期化しなくなった */
	memset(stream, 0, len);

	int sdlVol = (unsigned int)((double)SDL_MIX_MAXVOLUME * masterVolume);

	sdlVol = std::max(0, sdlVol);
	sdlVol = std::min(SDL_MIX_MAXVOLUME, sdlVol);

	sndbuf_lock();

	sndbuf = SNDBUF_FILLED_QUEUE_FIRST();
	if (sndbuf == NULL)
		goto out;

	while (sndbuf->remain < len) {
		SNDBUF_FILLED_QUEUE_REMOVE_HEAD();
		sndbuf_unlock();

		SDL_MixAudio(stream,
			(uint8_t *)sndbuf->buf + (sndbuf->size - sndbuf->remain),
			sndbuf->remain, SDL_MIX_MAXVOLUME);
		stream += sndbuf->remain;
		len -= sndbuf->remain;
		sndbuf->remain = 0;

		sndbuf_lock();
		SNDBUF_FREELIST_INSERT_HEAD(sndbuf);
		sndbuf = SNDBUF_FILLED_QUEUE_FIRST();
		if (sndbuf == NULL)
			goto out;
	}

	if (sndbuf->remain == len) {
		SNDBUF_FILLED_QUEUE_REMOVE_HEAD();
		sndbuf_unlock();
	}

	if(!mute){
		SDL_MixAudio(stream, (uint8_t *)sndbuf->buf + (sndbuf->size - sndbuf->remain),
			len, sdlVol);
	}
	sndbuf->remain -= len;

	if (sndbuf->remain == 0) {
		sndbuf_lock();
		SNDBUF_FREELIST_INSERT_HEAD(sndbuf);
	}
 out:
	sndbuf_unlock();
}


static BRESULT
sdlaudio_init(UINT rate, UINT samples)
{
	static SDL_AudioSpec fmt;
	int rv;

	fmt.freq = rate;
	fmt.format = AUDIO_S16SYS;
	fmt.channels = 2;
	fmt.samples = samples;
	fmt.callback = sdlaudio_callback;
	fmt.userdata = UINT32_TO_PTR(samples * 2 * sizeof(SINT16));

	rv = SDL_InitSubSystem(SDL_INIT_AUDIO);
	if (rv < 0) {
		printf("sdlaudio_init: SDL_InitSubSystem(): %s\n",
			SDL_GetError());
		return FAILURE;
	}

	audio_fd = SDL_OpenAudio(&fmt, NULL);
	if (audio_fd < 0) {
		printf("sdlaudio_init: SDL_OpenAudio(): %s\n",
			SDL_GetError());
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
		return FAILURE;
	}

	sound_silence = fmt.silence;

	return SUCCESS;
}

static BRESULT
sdlaudio_term(void)
{

	SDL_PauseAudio(1);
	SDL_CloseAudio();

	return SUCCESS;
}

static void
sdlaudio_lock(void)
{

	SDL_LockAudio();
}

static void
sdlaudio_unlock(void)
{

	SDL_UnlockAudio();
}

static void
sdlaudio_play(void)
{

	SDL_PauseAudio(0);
}

static void
sdlaudio_stop(void)
{

	SDL_PauseAudio(1);
}

static void
sounddrv_lock(void)
{
	sdlaudio_lock();
}

static void
sounddrv_unlock(void)
{
	sdlaudio_unlock();
}

UINT
soundmng_create(UINT rate, UINT bufmsec)
{
	if (opened || ((rate != 11025) && (rate != 22050) && (rate != 44100))) {
		return 0;
	}

	if (bufmsec < 20)
		bufmsec = 20;
	else if (bufmsec > 1000)
		bufmsec = 1000;

	UINT samples = calc_blocksize((rate * bufmsec) / 1000 / 2);
	opna_frame = samples * 2 * sizeof(SINT16);

	if (sdlaudio_init(rate, samples) != SUCCESS) {
		//TODO: Error handling
		audio_fd = -1;
		return 0;
	}

	vermouth_module = midimod_create(rate);
	midimod_loadall(vermouth_module);

	buffer_init();

	opened = TRUE;

	return samples;
}

void
soundmng_reset(void)
{

	sounddrv_lock();
	buffer_clear();
	sounddrv_unlock();
}

void
soundmng_destroy(void)
{
	if (opened) {
		midimod_destroy(vermouth_module);
		vermouth_module = NULL;

		sdlaudio_stop();
		sdlaudio_term();

		audio_fd = -1;
		opened = FALSE;
	}

	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void
soundmng_play(void)
{
	sdlaudio_play();
}

void
soundmng_stop(void)
{
	sdlaudio_stop();
}

BRESULT
soundmng_initialize(void)
{
	SDL_InitSubSystem(SDL_INIT_AUDIO);
	return SUCCESS;
}

void
soundmng_deinitialize(void)
{
	soundmng_destroy();
	buffer_destroy();
}

void
soundmng_sync(void)
{

	if (opened) {
		sounddrv_lock();
		struct sndbuf *sndbuf = SNDBUF_FREELIST_FIRST();

		if (sndbuf != NULL) {
			SNDBUF_FREELIST_REMOVE_HEAD();
			sounddrv_unlock();

			const SINT32 *pcm = sound_pcmlock();

			//TODO: This does some sort of clipping and "transforms"
			//32 bit pcm samples into 16 bit ones. Why are the samples
			//obtained by sound_pcmlock... Well, I haven't dug that deep yet...
			//but i _think_ it is possible to register several audio "streams"
			//into the internal sound buffer, which then get added up. Because
			//these streams would clip at 16bit they are added together into
			//a 32bit stream as to not lose any data, and then finally they get
			//to this place. and need to be adjusted... The issue is that they
			//simply get clipped again... So i am not sure what this is supposed
			//to do.. I suppose here it only gets clipped once instead of several
			//times internally? In any case, more investigation is needed.

			//TODO: Also, saturation_s16x and saturation_s16 do the exact same thing
			//No clue what the difference is supposed to be.
			saturation_s16((SINT16 *)sndbuf->buf, pcm, opna_frame);
			//saturation_s16x((SINT16 *)sndbuf->buf, pcm, opna_frame);

			sound_pcmunlock(pcm);
			sndbuf->remain = sndbuf->size;

			sounddrv_lock();
			SNDBUF_FILLED_QUEUE_INSERT_TAIL(sndbuf);
		}
		sounddrv_unlock();
	}
}

void soundmng_increaseVol(double incr){
	masterVolume += incr;
	masterVolume = std::min(1.0, masterVolume);
}

void soundmng_decreaseVol(double decr){
	masterVolume -= decr;
	masterVolume = std::max(0.0, masterVolume);
}

void soundmng_toggleMute(){
	mute = !mute;
}

/*
 * sound buffer
 */
static BRESULT
buffer_init(void)
{
	BRESULT result = SUCCESS;
	int i;

	sounddrv_lock();
	for (i = 0; i < NSOUNDBUFFER; i++) {
		if (sound_buffer[i].buf != NULL) {
			_MFREE(sound_buffer[i].buf);
			sound_buffer[i].buf = NULL;
		}
		sound_buffer[i].buf = (char *)_MALLOC(opna_frame, "sound buffer");
		if (sound_buffer[i].buf == NULL) {
			printf("buffer_init: can't alloc memory\n");
			while (--i >= 0) {
				_MFREE(sound_buffer[i].buf);
				sound_buffer[i].buf = NULL;
			}
			result = FAILURE;
			goto out;
		}
	}
	buffer_clear();
 out:
	sounddrv_unlock();
	return result;
}

static void
buffer_clear(void)
{
	int i;

	SNDBUF_FREELIST_INIT();
	SNDBUF_FILLED_QUEUE_INIT();

	for (i = 0; i < NSOUNDBUFFER; i++) {
		sound_buffer[i].next = &sound_buffer[i + 1];
		if (sound_buffer[i].buf != NULL)
			memset(sound_buffer[i].buf, 0, opna_frame);
		sound_buffer[i].size = sound_buffer[i].remain = opna_frame;
	}
	sound_buffer[NSOUNDBUFFER - 1].next = NULL;

	sndbuf_freelist = sound_buffer;
}

static void
buffer_destroy(void)
{
	int i;

	sounddrv_lock();

	SNDBUF_FREELIST_INIT();
	SNDBUF_FILLED_QUEUE_INIT();

	for (i = 0; i < NSOUNDBUFFER; i++) {
		sound_buffer[i].next = NULL;
		if (sound_buffer[i].buf != NULL) {
			_MFREE(sound_buffer[i].buf);
			sound_buffer[i].buf = NULL;
		}
	}

	sounddrv_unlock();
}

