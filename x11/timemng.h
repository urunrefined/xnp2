#ifndef	NP2_X11_TIMEMNG_H__
#define	NP2_X11_TIMEMNG_H__

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

// Win32 SYSTEMTIME ストラクチャ

typedef struct {
	UINT16	year;
	UINT16	month;
	UINT16	week;
	UINT16	day;
	UINT16	hour;
	UINT16	minute;
	UINT16	second;
	UINT16	milli;
} _SYSTIME;

BRESULT timemng_gettime(_SYSTIME *systime);

#ifdef __cplusplus
}
#endif

#endif	/* NP2_X11_TIMEMNG_H__ */
