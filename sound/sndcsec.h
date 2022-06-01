/**
 * @file	sndcsec.h
 * @brief	Interface of the critical section for sound
 */

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "compiler.h"
#include <pthread.h>

#if defined(SOUND_CRITICAL)

extern pthread_mutex_t g_sndcsec;

#define	SNDCSEC_INIT	pthread_mutex_init(&g_sndcsec, NULL)
#define	SNDCSEC_TERM	pthread_mutex_destroy(&g_sndcsec)
#define	SNDCSEC_ENTER	pthread_mutex_lock(&g_sndcsec)
#define	SNDCSEC_LEAVE	pthread_mutex_unlock(&g_sndcsec)

#elif defined(_SDL_mutex_h)

extern SDL_mutex* g_sndcsec;

#define	SNDCSEC_INIT	g_sndcsec = SDL_CreateMutex()
#define	SNDCSEC_TERM	SDL_DestroyMutex(g_sndcsec)
#define	SNDCSEC_ENTER	SDL_LockMutex(g_sndcsec)
#define	SNDCSEC_LEAVE	SDL_UnlockMutex(g_sndcsec)

#else	/* defined(SOUND_CRITICAL) */

#define	SNDCSEC_INIT		/*!< Initialize */
#define	SNDCSEC_TERM		/*!< Deinitialize */
#define	SNDCSEC_ENTER		/*!< Enter critical section */
#define	SNDCSEC_LEAVE		/*!< Leave critical section */

#endif	/* defined(SOUND_CRITICAL) */

#ifdef __cplusplus
}
#endif
