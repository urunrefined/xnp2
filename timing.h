#pragma once

#include "compiler.h"

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

#ifdef __cplusplus
extern "C" {
#endif

void timing_reset(void);
void timing_setrate(UINT lines, UINT crthz);
void timing_setcount(UINT value);
UINT timing_getcount(void);

#ifdef __cplusplus
}
#endif
