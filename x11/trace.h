#ifndef NP2_X11_TRACE_H__
#define NP2_X11_TRACE_H__

#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void trace_fmt(const char *fmt, ...);
void hexdump(const uint8_t *buffer, size_t sz);

#ifdef ENABLE_TRACE
#define TRACEOUT(arg) trace_fmt arg
#else
#define TRACEOUT(a)
#endif

#ifdef ENABLE_VERBOSE
#define VERBOSE(arg) trace_fmt arg
#else
#define VERBOSE(a)
#endif

#ifdef __cplusplus
}
#endif

#endif /* NP2_X11_TRACE_H__ */
