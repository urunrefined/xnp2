/*
 * Copyright (c) 2003 NONAKA Kimihiro
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
#include "pccore.h"
#include "statsave.h"
#include "dosio.h"
#include "scrndraw.h"
#include "timing.h"

#include "commng.h"
#include "kbdmng.h"
#include "scrnmng.h"
#include "soundmng.h"

#include <unistd.h>
#include <sys/time.h>


NP2OSCFG np2oscfg = {
	0,					/* DISPCLK */

	KEY_KEY106,			/* KEYBOARD */
	0,					/* F12KEY */
	0,					/* JOYPAD1 */
	0,					/* JOYPAD2 */
	{ "", "" },					/* JOYDEV */

	{ COMPORT_MIDI, 0, 0x3e, 19200, "", "", "", "" },	/* mpu */
	{
		{ COMPORT_NONE, 0, 0x3e, 19200, "", "", "", "" },/* com1 */
		{ COMPORT_NONE, 0, 0x3e, 19200, "", "", "", "" },/* com2 */
		{ COMPORT_NONE, 0, 0x3e, 19200, "", "", "", "" },/* com3 */
	},

	0,					/* confirm */

	0,					/* statsave */
	0,					/* hostdrv_write */
	0,					/* jastsnd */

	{ "", "" },			/* MIDIDEV */
	0,					/* MIDIWAIT */
};

UINT framecnt = 0;
UINT waitcnt = 0;
UINT framemax = 1;

BOOL s98logging = FALSE;
int s98log_count = 0;

char modulefile[MAX_PATH];
char statpath[MAX_PATH];

const char np2flagext[] = "s%02d";

#ifndef FONTFACE
#define FONTFACE "-misc-fixed-%s-r-normal--%d-*-*-*-*-*-*-*"
#endif
char fontname[1024] = FONTFACE;

UINT32
gettick(void)
{
	struct timeval tv;

	gettimeofday(&tv, 0);
	return tv.tv_usec / 1000 + tv.tv_sec * 1000;
}

static void
getstatfilename(char* path, const char* ext, int size)
{

	/*
	 * default:
	 * e.g. resume:   "/home/user_name/.np2/sav/np2.sav"
	 *      statpath: "/home/user_name/.np2/sav/np2.s00"
	 *      config:   "/home/user_name/.np2/np2rc"
	 *
	 * --config option:
	 * e.g. resume:   "/config_file_path/sav/np2.sav"
	 *      statpath: "/config_file_path/sav/np2.s00"
	 *      config:   "/config_file_path/config_file_name"
	 */
	file_cpyname(path, statpath, size);
	file_catname(path, ".", size);
	file_catname(path, ext, size);
}

int
flagsave(const char* ext)
{
	char path[MAX_PATH];
	int ret;

	getstatfilename(path, ext, sizeof(path));
	soundmng_stop();
	ret = statsave_save(path);
	if (ret) {
		file_delete(path);
	}
	soundmng_play();

	return ret;
}

void
flagdelete(const char* ext)
{
	char path[MAX_PATH];

	getstatfilename(path, ext, sizeof(path));
	file_delete(path);
}

int
flagload(const char* ext, BOOL force)
{
	char path[MAX_PATH];
	char buf[1024];
	int ret;
	int rv = 0;

	getstatfilename(path, ext, sizeof(path));
	ret = statsave_check(path, buf, sizeof(buf));
	if (ret & (~STATFLAG_DISKCHG)) {
		rv = 1;
	} else if ((!force) && (ret & STATFLAG_DISKCHG)) {
		rv = 1;
	}
	if (rv == 0) {
		statsave_load(path);
	}

	return rv;
}

void
changescreen(void *graphics, UINT8 newmode)
{
	(void) newmode;

	soundmng_stop();
	scrndraw_redraw(graphics);
	soundmng_play();
}

void
framereset()
{
	framecnt = 0;
}

static BOOL
taskmng_sleep(UINT32 tick)
{
	UINT32 base;
	UINT32 now;

	base = gettick();
	while ((((now = gettick()) - base) < tick)) {
		usleep((tick - (now - base) / 2) * 1000);
	}

	return 1;
}

void
processwait(UINT cnt)
{

	if (timing_getcount() >= cnt) {
		timing_setcount(0);
		framereset();
	} else {
		taskmng_sleep(1);
	}
}

int
mainloop(void *graphics)
{
	/* auto skip */
	if (waitcnt == 0) {
		UINT cnt;
		pccore_exec(graphics, framecnt == 0);
		framecnt++;
		cnt = timing_getcount();
		if (framecnt > cnt) {
			waitcnt = framecnt;
			if (framemax > 1) {
				framemax--;
			}
		} else if (framecnt >= framemax) {
			if (framemax < 12) {
				framemax++;
			}
			if (cnt >= 12) {
				timing_reset();
			} else {
				timing_setcount(cnt - framecnt);
			}
			framereset();
		}
	} else {
		processwait(waitcnt);
		waitcnt = framecnt;
	}

	return TRUE;
}
