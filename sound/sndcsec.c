/**
 * @file	sndcsec.c
 * @brief	Implementation of the critical section for sound
 */

#include "compiler.h"
#include "sndcsec.h"

#if defined(SOUND_CRITICAL)

#if defined(MACOS)

	MPCriticalRegionID g_sndcsec;

#elif defined(X11)

	pthread_mutex_t g_sndcsec;		/* = PTHREAD_MUTEX_INITIALIZER; */

#elif defined(_SDL_mutex_h)

	SDL_mutex* g_sndcsec;

#endif

#endif	/* defined(SOUND_CRITICAL) */
