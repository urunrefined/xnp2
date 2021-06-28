/**
 * @file	pcmmix.c
 * @brief	Implementation of the pcm sound
 */

#include "compiler.h"
#include "pcmmix.h"
#include "dosio.h"
#include "_memory.h"

BRESULT pcmmix_regist(PMIXDAT *dat, void *datptr, UINT datsize, UINT rate) {
	return(FAILURE);
}

BRESULT pcmmix_regfile(PMIXDAT *dat, const OEMCHAR *fname, UINT rate) {
	return(FAILURE);
}

void SOUNDCALL pcmmix_getpcm(PCMMIX hdl, SINT32 *pcm, UINT count) {

}
