#pragma once

#include "commng.h"
#include "np2.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _commng *cmALSASerial_create(const char *hwdevice);

#ifdef __cplusplus
}
#endif
