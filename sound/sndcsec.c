/**
 * @file	sndcsec.c
 * @brief	Implementation of the critical section for sound
 */

#include "compiler.h"
#include "sndcsec.h"

#if defined(SOUND_CRITICAL)
	pthread_mutex_t g_sndcsec;		/* = PTHREAD_MUTEX_INITIALIZER; */
#endif


