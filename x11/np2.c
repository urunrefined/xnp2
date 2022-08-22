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
#include "timing.h"

#include <unistd.h>
#include <sys/time.h>

static UINT framecnt = 0;
static UINT waitcnt = 0;
static UINT framemax = 1;

UINT32
gettick(void)
{
	struct timeval tv;

	gettimeofday(&tv, 0);
	return tv.tv_usec / 1000 + tv.tv_sec * 1000;
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
