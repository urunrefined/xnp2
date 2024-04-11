#include "cmamidi.h"
#include "cmmidi.h"
#include "cmserial.h"
#include "compiler.h"

#include "cmjasts.h"
#include "commng.h"
#include "global.h"
#include "np2.h"

#include <stdio.h>

// ---- non connect

static UINT ncread(struct _commng *self, UINT8 *data) {
    (void)self;
    (void)data;
    return 0;
}

static UINT ncwrite(struct _commng *self, UINT8 data) {
    (void)self;
    (void)data;

    return 0;
}

static UINT8 ncgetstat(struct _commng *self) {
    (void)self;

    return 0xf0;
}

static INTPTR ncmsg(struct _commng *self, UINT msg, INTPTR param) {
    (void)self;
    (void)msg;
    (void)param;

    return 0;
}

static void ncrelease(struct _commng *self) {
    (void)self;
    /* Nothing to do */
}

static struct _commng com_nc = {COMCONNECT_OFF, ncread, ncwrite,
                                ncgetstat,      ncmsg,  ncrelease};

// ----

void commng_initialize(void) { cmmidi_initialize(); }

static const char *getDeviceName(UINT device) {
    return device == COMCREATE_SERIAL     ? "COMCREATE_SERIAL"
           : device == COMCREATE_PC9861K1 ? "COMCREATE_PC9861K1"
           : device == COMCREATE_PC9861K2 ? "COMCREATE_PC9861K2"
           : device == COMCREATE_PRINTER  ? "COMCREATE_PRINTER"
           : device == COMCREATE_MPU98II  ? "COMCREATE_MPU98II"
                                          : "Unknown COMCREATE";
}

static const char *getComName(enum ComPort comPort) {
    return comPort == COMPORT_NONE   ? "COMPORT_NONE"
           : comPort == COMPORT_COM1 ? "COMPORT_COM1"
           : comPort == COMPORT_COM2 ? "COMPORT_COM2"
           : comPort == COMPORT_COM3 ? "COMPORT_COM3"
           : comPort == COMPORT_COM4 ? "COMPORT_COM4"
           : comPort == COMPORT_MIDI ? "COMPORT_MIDI"
                                     : "Unknown COMPORT";
}

struct _commng *commng_create(UINT device) {

    printf("create device (%s)\n", getDeviceName(device));

    // TODO: Investigate printer // Why cant it be opened as a simple tty?

    if (device == COMCREATE_PRINTER && np2oscfg.jastsnd) {
        printf("Try to create jasts device\n");
        return cmjasts_create();
    }

    // TODO: cmmidi is currently cometely disabled

    COMCFG *cfg = (device == COMCREATE_SERIAL     ? &np2oscfg.com[0]
                   : device == COMCREATE_PC9861K1 ? &np2oscfg.com[1]
                   : device == COMCREATE_PC9861K2 ? &np2oscfg.com[2]
                                                  : 0);

    if (!cfg) {
        printf("No config for device %s\n", getDeviceName(device));
        return &com_nc;
    }

    struct _commng *ret = 0;

    if (strcmp(cfg->type, "alsaraw") == 0) {
        ret = cmALSASerial_create(cfg->alsaRawHWName);
    } else if (strcmp(cfg->type, "tty") == 0) {
        ret = cmserial_create(cfg->ttyname);
    } else {
        printf("Invalid COM type %s\n", cfg->type);
    }

    printf("device(%u) result: %p\n", device, ret);

    if (!ret) {
        ret = &com_nc;
    }

    return ret;
}

void commng_destroy(struct _commng *hdl) {

    if (hdl) {
        hdl->release(hdl);
    }
}
