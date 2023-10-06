#include "compiler.h"

#include "dosio.h"
#include "ini.h"
#include "np2.h"
#include "pccore.h"
#include "profile.h"
#include "strres.h"

#include "commng.h"
#include "kbdmng.h"
#include "milstr.h"
#include "soundmng.h"

typedef struct {
    const char *title;
    INITBL *tbl;
    INITBL *tblterm;
    UINT count;
} _INIARG, *INIARG;

static BOOL inigetbmp(const UINT8 *ptr, UINT pos) {

    return ((ptr[pos >> 3] >> (pos & 7)) & 1);
}

static void inisetbmp(UINT8 *ptr, UINT pos, BOOL set) {
    UINT8 bit;

    ptr += (pos >> 3);
    bit = 1 << (pos & 7);
    if (set) {
        *ptr |= bit;
    } else {
        *ptr &= ~bit;
    }
}

static void inirdargs16(const char *src, INITBL *ini) {
    SINT16 *dst;
    int dsize;
    int i;
    char c;

    dst = (SINT16 *)ini->value;
    dsize = ini->arg;

    for (i = 0; i < dsize; i++) {
        while (*src == ' ') {
            src++;
        }
        if (*src == '\0') {
            break;
        }
        dst[i] = (SINT16)milstr_solveINT(src);
        while (*src != '\0') {
            c = *src++;
            if (c == ',') {
                break;
            }
        }
    }
}

static void inirdargh8(const char *src, INITBL *ini) {
    UINT8 *dst;
    int dsize;
    int i;
    UINT8 val;
    BOOL set;
    char c;

    dst = (UINT8 *)ini->value;
    dsize = ini->arg;

    for (i = 0; i < dsize; i++) {
        val = 0;
        set = FALSE;
        while (*src == ' ') {
            src++;
        }
        for (;;) {
            c = *src;
            if ((c == '\0') || (c == ' ')) {
                break;
            } else if ((c >= '0') && (c <= '9')) {
                val <<= 4;
                val += c - '0';
                set = TRUE;
            } else {
                c |= 0x20;
                if ((c >= 'a') && (c <= 'f')) {
                    val <<= 4;
                    val += c - 'a' + 10;
                    set = TRUE;
                }
            }
            src++;
        }
        if (set == FALSE) {
            break;
        }
        dst[i] = val;
    }
}

static void iniwrsetargh8(char *work, int size, INITBL *ini) {
    char tmp[8];
    const UINT8 *ptr;
    UINT arg;
    UINT i;

    ptr = (UINT8 *)(ini->value);
    arg = ini->arg;
    if (arg > 0) {
        snprintf(tmp, sizeof(tmp), "%.2x ", ptr[0]);
        milstr_ncpy(work, tmp, size);
    }
    for (i = 1; i < arg; i++) {
        snprintf(tmp, sizeof(tmp), "%.2x ", ptr[i]);
        milstr_ncat(work, tmp, size);
    }
}

/* ----- user */

static void inirdbyte3(const char *src, INITBL *ini) {
    UINT i;

    for (i = 0; i < 3; i++) {
        if (src[i] == '\0') {
            break;
        }
        if ((((src[i] - '0') & 0xff) < 9) || (((src[i] - 'A') & 0xdf) < 26)) {
            ((UINT8 *)ini->value)[i] = src[i];
        }
    }
}

static void inirdkb(const char *src, INITBL *ini) {

    if ((!milstr_extendcmp(src, "DOS")) || (!milstr_cmp(src, "JIS")) ||
        (!milstr_cmp(src, "106")) || (!milstr_cmp(src, "JP")) ||
        (!milstr_cmp(src, "PCAT")) || (!milstr_cmp(src, "AT"))) {
        *(UINT8 *)ini->value = KEY_KEY106;
    } else if ((!milstr_extendcmp(src, "KEY101")) ||
               (!milstr_cmp(src, "ASCII")) || (!milstr_cmp(src, "EN")) ||
               (!milstr_cmp(src, "US")) || (!milstr_cmp(src, "101"))) {
        *(UINT8 *)ini->value = KEY_KEY101;
    }
}

static BRESULT inireadcb(void *arg, const char *para, const char *key,
                         const char *data) {
    char work[512];
    INITBL *p;

    if (arg == NULL) {
        return (FAILURE);
    }
    if (milstr_cmp(para, ((INIARG)arg)->title)) {
        return (SUCCESS);
    }
    p = ((INIARG)arg)->tbl;
    while (p < ((INIARG)arg)->tblterm) {
        if (!milstr_cmp(key, p->item)) {
            switch (p->itemtype & INITYPE_MASK) {
            case INITYPE_STR:
                milstr_ncpy((char *)p->value, data, p->arg);
                break;

            case INITYPE_BOOL:
                *((UINT8 *)p->value) = (!milstr_cmp(data, str_true)) ? 1 : 0;
                break;

            case INITYPE_BITMAP:
                inisetbmp((UINT8 *)p->value, p->arg,
                          milstr_cmp(data, str_true) == 0);
                break;

            case INITYPE_ARGS16:
                milstr_ncpy(work, data, 512);
                inirdargs16(work, p);
                break;

            case INITYPE_ARGH8:
                milstr_ncpy(work, data, 512);
                inirdargh8(work, p);
                break;

            case INITYPE_SINT8:
            case INITYPE_UINT8:
                *((UINT8 *)p->value) = (UINT8)milstr_solveINT(data);
                break;

            case INITYPE_SINT16:
            case INITYPE_UINT16:
                *((UINT16 *)p->value) = (UINT16)milstr_solveINT(data);
                break;

            case INITYPE_SINT32:
            case INITYPE_UINT32:
                *((UINT32 *)p->value) = (UINT32)milstr_solveINT(data);
                break;

            case INITYPE_HEX8:
                *((UINT8 *)p->value) = (UINT8)milstr_solveHEX(data);
                break;

            case INITYPE_HEX16:
                *((UINT16 *)p->value) = (UINT16)milstr_solveHEX(data);
                break;

            case INITYPE_HEX32:
                *((UINT32 *)p->value) = (UINT32)milstr_solveHEX(data);
                break;

            case INITYPE_BYTE3:
                milstr_ncpy(work, data, 512);
                inirdbyte3(work, p);
                break;

            case INITYPE_KB:
                milstr_ncpy(work, data, 512);
                inirdkb(work, p);
                break;

            default:
                break;
            }
        }
        p++;
    }
    return (SUCCESS);
}

static void ini_read(const char *path, const char *title, INITBL *tbl,
                     UINT count) {
    _INIARG iniarg;

    if (path == NULL) {
        return;
    }
    iniarg.title = title;
    iniarg.tbl = tbl;
    iniarg.tblterm = tbl + count;
    profile_enum(path, &iniarg, inireadcb);
}

static void iniwrsetstr(char *work, int size, const char *ptr) {
    int i;
    char c;

    if (ptr[0] == ' ') {
        goto iwss_extend;
    }
    i = strlen(ptr);
    if ((i) && (ptr[i - 1] == ' ')) {
        goto iwss_extend;
    }
    while (i > 0) {
        i--;
        if (ptr[i] == '\"') {
            goto iwss_extend;
        }
    }
    milstr_ncpy(work, ptr, size);
    return;

iwss_extend:
    if (size > 3) {
        size -= 3;
        *work++ = '\"';
        while (size > 0) {
            size--;
            c = *ptr++;
            if (c == '\"') {
                if (size > 0) {
                    size--;
                    work[0] = c;
                    work[1] = c;
                    work += 2;
                }
            } else {
                *work++ = c;
            }
        }
        work[0] = '\"';
        work[1] = '\0';
    }
}

static void ini_write(const char *path, const char *title, INITBL *tbl,
                      UINT count, BOOL create) {
    char work[512];
    INITBL *p;
    INITBL *pterm;
    FILEH fh;
    BRESULT set;

    fh = FILEH_INVALID;
    if (!create) {
        fh = file_open(path);
        if (fh != FILEH_INVALID)
            file_seek(fh, 0L, FSEEK_END);
    }
    if (fh == FILEH_INVALID) {
        fh = file_create(path);
        if (fh == FILEH_INVALID)
            return;
    }

    milstr_ncpy(work, "[", sizeof(work));
    milstr_ncat(work, title, sizeof(work));
    milstr_ncat(work, "]\n", sizeof(work));
    file_write(fh, work, strlen(work));

    p = tbl;
    pterm = tbl + count;
    while (p < pterm) {
        if (!(p->itemtype & INIFLAG_RO)) {
            work[0] = '\0';
            set = SUCCESS;
            switch (p->itemtype & INITYPE_MASK) {
            case INITYPE_STR:
                iniwrsetstr(work, sizeof(work), (char *)p->value);
                break;

            case INITYPE_BOOL:
                milstr_ncpy(work, (*((UINT8 *)p->value)) ? str_true : str_false,
                            sizeof(work));
                break;

            case INITYPE_BITMAP:
                milstr_ncpy(work,
                            inigetbmp((UINT8 *)p->value, p->arg) ? str_true
                                                                 : str_false,
                            sizeof(work));
                break;

            case INITYPE_ARGH8:
                iniwrsetargh8(work, sizeof(work), p);
                break;

            case INITYPE_SINT8:
                snprintf(work, sizeof(work), "%d", *((char *)p->value));
                break;

            case INITYPE_SINT16:
                snprintf(work, sizeof(work), "%d", *((SINT16 *)p->value));
                break;

            case INITYPE_SINT32:
                snprintf(work, sizeof(work), "%d", *((SINT32 *)p->value));
                break;

            case INITYPE_UINT8:
                snprintf(work, sizeof(work), "%u", *((UINT8 *)p->value));
                break;

            case INITYPE_UINT16:
                snprintf(work, sizeof(work), "%u", *((UINT16 *)p->value));
                break;

            case INITYPE_UINT32:
                snprintf(work, sizeof(work), "%u", *((UINT32 *)p->value));
                break;

            case INITYPE_HEX8:
                snprintf(work, sizeof(work), "%x", *((UINT8 *)p->value));
                break;

            case INITYPE_HEX16:
                snprintf(work, sizeof(work), "%x", *((UINT16 *)p->value));
                break;

            case INITYPE_HEX32:
                snprintf(work, sizeof(work), "%x", *((UINT32 *)p->value));
                break;

            case INITYPE_KB:
                if (*(UINT8 *)p->value == KEY_KEY101)
                    milstr_ncpy(work, "101", sizeof(work));
                else
                    milstr_ncpy(work, "106", sizeof(work));
                break;

            default:
                set = FAILURE;
                break;
            }
            if (set == SUCCESS) {
                file_write(fh, p->item, strlen(p->item));
                file_write(fh, " = ", 3);
                file_write(fh, work, strlen(work));
                file_write(fh, "\n", 1);
            }
        }
        p++;
    }
    file_close(fh);
}

void initload(const char *path, const char *iniTitle, INITBL *tbl,
              size_t size) {
    ini_read(path, iniTitle, tbl, size);
}

void initsave(const char *path, const char *iniTitle, INITBL *tbl,
              size_t size) {
    ini_write(path, iniTitle, tbl, size, TRUE);
}
