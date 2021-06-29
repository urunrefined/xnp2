#pragma once

#if defined(SUPPORT_SCSI)

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "compiler.h"

REG8 scsicmd_negate(REG8 id);
REG8 scsicmd_select(REG8 id);
REG8 scsicmd_transfer(REG8 id, UINT8 *cdb);
BRESULT scsicmd_send(void);

void scsicmd_bios(void);

#ifdef __cplusplus
}
#endif

#endif

