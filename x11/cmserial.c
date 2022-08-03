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

#include "np2.h"
#include "commng.h"
#include "trace.h"
#include "_memory.h"
#include "global.h"
#include "cmserial.h"

#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>


typedef struct {
	int		hdl;

	struct termios	tio;
} _CMSER, *CMSER;

const UINT32 cmserial_speed[10] = {
	110, 300, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200
};


static UINT
serialread(COMMNG self, UINT8 *data)
{
	CMSER serial = (CMSER)(self + 1);
	size_t size;
	int bytes;
	int rv;

	rv = ioctl(serial->hdl, FIONREAD, &bytes);
	if (rv == 0 && bytes > 0) {
		VERBOSE(("serialread: bytes = %d", bytes));
		size = read(serial->hdl, data, 1);
		if (size == 1) {
			VERBOSE(("serialread: data = %02x", *data));
			return 1;
		}
		VERBOSE(("serialread: read failure (%s)", strerror(errno)));
	}
	return 0;
}

static UINT
serialwrite(COMMNG self, UINT8 data)
{
	CMSER serial = (CMSER)(self + 1);
	size_t size;

	size = write(serial->hdl, &data, 1);
	if (size == 1) {
		VERBOSE(("serialwrite: data = %02x", data));
		return 1;
	}
	VERBOSE(("serialwrite: write failure (%s)", strerror(errno)));
	return 0;
}

static UINT8
serialgetstat(COMMNG self)
{
	CMSER serial = (CMSER)(self + 1);
	int status;
	int rv;

	rv = ioctl(serial->hdl, TIOCMGET, &status);
	if (rv < 0) {
		VERBOSE(("serialgetstat: ioctl: %s", strerror(errno)));
		return 0x20;
	}
	if (!(status & TIOCM_DSR)) {
		VERBOSE(("serialgetstat: DSR is disable"));
		return 0x20;
	}
	VERBOSE(("serialgetstat: DSR is enable"));
	return 0x00;
}

static INTPTR
serialmsg(COMMNG self, UINT msg, INTPTR param)
{

	(void)self;
	(void)msg;
	(void)param;

	return 0;
}

static void
serialrelease(COMMNG self)
{
	CMSER serial = (CMSER)(self + 1);

	tcsetattr(serial->hdl, TCSANOW, &serial->tio);
	close(serial->hdl);
	_MFREE(self);
}



COMMNG
cmserial_create(UINT port, UINT8 param, UINT32 speed)
{
	static const int cmserial_cflag[10] = {
		B110, B300, B1200, B2400, B4800,
		B9600, B19200, B38400, B57600, B115200
	};
	static const int csize[] = { CS5, CS6, CS7, CS8 };
	struct termios options, origopt;
	COMMNG ret;
	CMSER serial;
	int hdl;
	UINT i;

	VERBOSE(("cmserial_create: port = %d, param = %02x, speed = %d", port, param, speed));

	if (port == 0 || port > MAX_SERIAL_PORT_NUM) {
		VERBOSE(("cmserial_create: port is invalid"));
		goto cscre_failure;
	}

	port--;
	if (np2oscfg.com[port].mout[0] == '\0') {
		VERBOSE(("cmserial_create: com device file is disable"));
		goto cscre_failure;
	}

	hdl = open(np2oscfg.com[port].mout, O_RDWR | O_NOCTTY | O_NDELAY);
	if (hdl == -1) {
		VERBOSE(("cmserial_create: open failure %s, %s", np2oscfg.com[port].mout, strerror(errno)));
		goto cscre_failure;
	}

	if (!isatty(hdl)) {
		VERBOSE(("cmserial_create: not terminal file descriptor (%s)", strerror(errno)));
		goto cscre_close;
	}

	/* get current options for the port */
	tcgetattr(hdl, &options);
	origopt = options;

	/* baud rates */
	for (i = 0; i < NELEMENTS(cmserial_speed); i++) {
		if (cmserial_speed[i] >= speed) {
			VERBOSE(("cmserial_create: spped = %d", cmserial_speed[i]));
			break;
		}
	}
	if (i >= NELEMENTS(cmserial_speed)) {
		VERBOSE(("cmserial_create: speed is invaild"));
		goto cscre_close;
	}
	cfsetispeed(&options, cmserial_cflag[i]);
	cfsetospeed(&options, cmserial_cflag[i]);

	/* character size bits */
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= csize[(param >> 2) & 3];
	VERBOSE(("cmserial_create: charactor size = %d", csize[(param >> 2) & 3]));

	/* parity check */
	switch (param & 0x30) {
	case 0x10:
		VERBOSE(("cmserial_create: odd parity"));
		options.c_cflag |= PARENB | PARODD;
		options.c_iflag |= INPCK | ISTRIP;
		break;

	case 0x30:
		VERBOSE(("cmserial_create: even parity"));
		options.c_cflag |= PARENB;
		options.c_cflag &= ~PARODD;
		options.c_iflag |= INPCK | ISTRIP;
		break;

	default:
		VERBOSE(("cmserial_create: non parity"));
		options.c_cflag &= ~PARENB;
		options.c_iflag &= ~(INPCK | ISTRIP);
		break;
	}

	/* stop bits */
	switch (param & 0xc0) {
	case 0x80:
		VERBOSE(("cmserial_create: stop bits: 1.5"));
		break;

	case 0xc0:
		VERBOSE(("cmserial_create: stop bits: 2"));
		options.c_cflag |= CSTOPB;
		break;

	default:
		VERBOSE(("cmserial_create: stop bits: 1"));
		options.c_cflag &= ~CSTOPB;
		break;
	}

	/* set misc flag */
	cfmakeraw(&options);
	options.c_cflag |= CLOCAL | CREAD;
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

	ret = (COMMNG)_MALLOC(sizeof(_COMMNG) + sizeof(_CMSER), "SERIAL");
	if (ret == NULL) {
		VERBOSE(("cmserial_create: memory alloc failure"));
		goto cscre_close;
	}

	/* set the new options for the port */
	tcsetattr(hdl, TCSANOW, &options);

#if 1
	ret->connect = COMCONNECT_MIDI;
#else
	ret->connect = COMCONNECT_SERIAL;
#endif
	ret->read = serialread;
	ret->write = serialwrite;
	ret->getstat = serialgetstat;
	ret->msg = serialmsg;
	ret->release = serialrelease;
	serial = (CMSER)(ret + 1);
	serial->hdl = hdl;
	serial->tio = origopt;
	return ret;

cscre_close:
	close(hdl);
cscre_failure:
	return NULL;
}
