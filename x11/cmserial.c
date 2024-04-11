/*
 * Copyright (c) 2004 NONAKA Kimihiro
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "compiler.h"

#include "_memory.h"
#include "cmserial.h"
#include "commng.h"
#include "global.h"
#include "np2.h"
#include "trace.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

struct SerialCom {
    struct _commng commng;
    int fd;
};

static UINT serialread(struct _commng *self, UINT8 *data) {
    struct SerialCom *serial = (struct SerialCom *)self;
    size_t size;
    int bytes;
    int rv;

    rv = ioctl(serial->fd, FIONREAD, &bytes);
    if (rv == 0 && bytes > 0) {
        VERBOSE(("serialread: bytes = %d", bytes));
        size = read(serial->fd, data, 1);
        if (size == 1) {
            VERBOSE(("serialread: data = %02x", *data));
            return 1;
        }
        VERBOSE(("serialread: read failure (%s)", strerror(errno)));
    }
    return 0;
}

// TODO: Handle eagain
static UINT serialwrite(struct _commng *self, UINT8 data) {
    struct SerialCom *serial = (struct SerialCom *)self;
    size_t size;

    size = write(serial->fd, &data, 1);
    if (size == 1) {
        VERBOSE(("serialwrite: data = %02x", data));
        return 1;
    }

    VERBOSE(("serialwrite: write failure (%s)", strerror(errno)));
    return 0;
}

static UINT8 serialgetstat(struct _commng *self) {
    struct SerialCom *serial = (struct SerialCom *)self;

    int status;
    int rv = ioctl(serial->fd, TIOCMGET, &status);

    if (rv < 0) {
        VERBOSE(("serialgetstat: ioctl: %s", strerror(errno)));
        return 0x20;
    }

    if (!(status & TIOCM_DSR)) {
        VERBOSE(("serialgetstat: DSR is disabled"));
        return 0x20;
    }

    VERBOSE(("serialgetstat: DSR is enabled"));
    return 0x00;
}

static INTPTR serialmsg(struct _commng *self, UINT msg, INTPTR param) {

    (void)self;
    (void)msg;
    (void)param;

    return 0;
}

static void serialrelease(struct _commng *self) {
    struct SerialCom *serial = (struct SerialCom *)self;
    printf("Release serial %p, fd %d\n", serial, serial->fd);
    close(serial->fd);
}

struct _commng *cmserial_create(const char *tty) {
    printf("cmserial_create: (terminal %s)\n", tty ? "NULL" : tty);

    if (!tty || tty[0] == '\0') {
        printf("cmserial_create: com device file is disabled\n");
        printf("Check config, set mmap if this port should be used\n");
        return NULL;
    }

    int fd = open(tty, O_RDWR | O_NOCTTY | O_NDELAY);

    if (fd == -1) {
        printf("Open failure %s, %s\n", tty, strerror(errno));
        return 0;
    }

    if (!isatty(fd)) {
        printf("cmserial_create: not terminal file descriptor\n");
        close(fd);
        return 0;
    }

    struct SerialCom *serialCom =
        (struct SerialCom *)malloc(sizeof(struct SerialCom));

    if (!serialCom) {
        printf("cmserial_create: memory alloc failure\n");
        close(fd);
    }

    memset(serialCom, 0, sizeof(struct SerialCom));

    serialCom->fd = fd;

    //    serialCom->commng.connect = COMCONNECT_MIDI;
    serialCom->commng.connect = COMCONNECT_SERIAL;

    serialCom->commng.read = serialread;
    serialCom->commng.write = serialwrite;
    serialCom->commng.getstat = serialgetstat;
    serialCom->commng.msg = serialmsg;
    serialCom->commng.release = serialrelease;

    printf("cmsercial created real serialCom %p\n", serialCom);
    printf("cmgetstat %p\n", serialgetstat);

    return (struct _commng *)serialCom;
}
