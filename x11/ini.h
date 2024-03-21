#ifndef NP2_X11_INI_H__
#define NP2_X11_INI_H__

#include "compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    INITYPE_STR = 0,
    INITYPE_BOOL,
    INITYPE_BITMAP,
    INITYPE_ARGS16,
    INITYPE_ARGH8,
    INITYPE_SINT8,
    INITYPE_SINT16,
    INITYPE_SINT32,
    INITYPE_UINT8,
    INITYPE_UINT16,
    INITYPE_UINT32,
    INITYPE_HEX8,
    INITYPE_HEX16,
    INITYPE_HEX32,
    INITYPE_BYTE3,
    INITYPE_KB,
    INITYPE_USER,
    INITYPE_MASK = 0xff,

    INIFLAG_RO = 0x0100,
    INIFLAG_MAX = 0x0200,
    INIFLAG_AND = 0x0400,
};

enum {
    INIRO_STR = INIFLAG_RO | INITYPE_STR,
    INIRO_BOOL = INIFLAG_RO | INITYPE_BOOL,
    INIRO_BITMAP = INIFLAG_RO | INITYPE_BITMAP,
    INIRO_UINT8 = INIFLAG_RO | INITYPE_UINT8,
    INIMAX_UINT8 = INIFLAG_MAX | INITYPE_UINT8,
    INIAND_UINT8 = INIFLAG_AND | INITYPE_UINT8,
    INIROMAX_SINT32 = INIFLAG_RO | INIFLAG_MAX | INITYPE_SINT32,
    INIROAND_HEX32 = INIFLAG_RO | INIFLAG_AND | INITYPE_HEX32,

    INIRO_BYTE3 = INIFLAG_RO | INITYPE_BYTE3,
    INIRO_KB = INIFLAG_RO | INITYPE_KB
};

typedef struct {
    char item[10];
    UINT16 itemtype;
    void *value;
    UINT32 arg;
} INITBL;

void initload(const char *path, const char *iniTitle, INITBL *tbl, size_t size);
void initsave(const char *path, const char *iniTitle, INITBL *tbl, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* NP2_X11_INI_H__ */
