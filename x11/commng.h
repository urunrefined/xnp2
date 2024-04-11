#pragma once

// ---- com manager interface

#include "compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

enum ConnectionType {
    COMCREATE_SERIAL = 0,
    COMCREATE_PC9861K1,
    COMCREATE_PC9861K2,
    COMCREATE_PRINTER,
    COMCREATE_MPU98II
};

enum ConnectionState {
    COMCONNECT_OFF = 0,
    COMCONNECT_SERIAL,
    COMCONNECT_MIDI,
    COMCONNECT_PARALLEL
};

enum { COMMSG_MIDIRESET = 0, COMMSG_SETFLAG, COMMSG_GETFLAG, COMMSG_USER };

struct _commng {
    UINT connect;

    UINT (*read)(struct _commng *self, UINT8 *data);
    UINT (*write)(struct _commng *self, UINT8 data);
    UINT8 (*getstat)(struct _commng *self);
    INTPTR (*msg)(struct _commng *self, UINT msg, INTPTR param);
    void (*release)(struct _commng *self);
};

typedef struct {
    UINT32 size;
    UINT32 sig;
    UINT32 ver;
    UINT32 param;
} _COMFLAG, *COMFLAG;

struct _commng *commng_create(UINT device);
void commng_destroy(struct _commng *hdl);

// ---- com manager for unix

enum ComPort {
    COMPORT_NONE = 0,
    COMPORT_COM1,
    COMPORT_COM2,
    COMPORT_COM3,
    COMPORT_COM4,
    COMPORT_MIDI
};

enum ComSig {
    COMSIG_COM1 = 0x314d4f43,
    COMSIG_COM2 = 0x324d4f43,
    COMSIG_COM3 = 0x334d4f43,
    COMSIG_COM4 = 0x344d4f43,
    COMSIG_MIDI = 0x4944494d
};

enum { COMMSG_MIMPIDEFFILE = COMMSG_USER, COMMSG_MIMPIDEFEN };

void commng_initialize(void);

#ifdef __cplusplus
}
#endif
