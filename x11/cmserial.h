#ifndef NP2_X11_CMSERIAL_H__
#define NP2_X11_CMSERIAL_H__

#include "commng.h"
#include "compiler.h"

// ---- com manager serial for unix

#ifdef __cplusplus
extern "C" {
#endif

extern const UINT32 cmserial_speed[10];

struct _commng* cmserial_create(UINT port, UINT8 param, UINT32 speed);

#if defined(SUPPORT_PC9861K)
#define MAX_SERIAL_PORT_NUM 3
#else
#define MAX_SERIAL_PORT_NUM 1
#endif

#ifdef __cplusplus
}
#endif

#endif /* NP2_X11_CMSERIAL_H__ */
