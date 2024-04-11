#ifndef NP2_X11_CMSERIAL_H__
#define NP2_X11_CMSERIAL_H__

#include "commng.h"

// ---- com manager serial for unix

#ifdef __cplusplus
extern "C" {
#endif

struct _commng *cmserial_create(const char *tty);

#if defined(SUPPORT_PC9861K)
#define MAX_SERIAL_PORT_NUM 3
#else
#define MAX_SERIAL_PORT_NUM 1
#endif

#ifdef __cplusplus
}
#endif

#endif /* NP2_X11_CMSERIAL_H__ */
