#pragma once

#include "compiler.h"
#include "pccore.h"

typedef struct {
	UINT8	port0439;
} _NECIO, *NECIO;

#ifdef __cplusplus
extern "C" {
#endif

void necio_reset(const NP2CFG *pConfig);
void necio_bind(void);

#ifdef __cplusplus
}
#endif

