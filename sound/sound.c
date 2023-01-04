/**
 * @file	sound.c
 * @brief	Implementation of the sound
 */

#include "compiler.h"
#include "sound.h"
#include "cpucore.h"
#include "pccore.h"
#include "iocore.h"
#include "beep.h"
#include "soundmng.h"
#include "trace.h"

SOUNDCFG	soundcfg;

#define	STREAM_CBMAX	16

/**
 * @brief Callback table
 */
struct tagSndCallback
{
	void *hdl;			/*!< instance */
	SOUNDCB cbfn;		/*!< function */
};
typedef struct tagSndCallback CBTBL;			/*!< Define */

/**
 * @brief Stream
 */
struct tagSndStream
{
	size_t  cur;
	CBTBL	cb[STREAM_CBMAX];
};
typedef struct tagSndStream SNDSTREAM;			/*!< Define */

static	SNDSTREAM	s_sndstream;

/**
 * Prepare
 * @param[in] sndstream The instance
 * @param[in] samples The samples
 */
void streamprepare(const SNDSTREAM sndstream, UINT samples)
{
	UINT count = min(44100, samples);

	if (count){
		for (size_t i = 0; i < sndstream.cur; i++){
			SINT32 buffer [count * 2];
			memset(buffer, 0, count * 2 * sizeof(SINT32));
			sndstream.cb[i].cbfn(sndstream.cb[i].hdl, buffer, count);
			soundmng_sync(soundcfg.soundRef, i, buffer, count * 2);
		}

		//SINT32 buffer [count * 2];
		//memset(buffer, 0, count * 2 * sizeof(SINT32));
		//for (size_t i = 0; i < sndstream.cur; i++){
		//	sndstream.cb[i].cbfn(sndstream.cb[i].hdl, buffer, count);
		//}

		//soundmng_sync(soundcfg.soundRef, 0, buffer, count * 2);
	}
}

/* ---- */

BRESULT sound_create(void *soundRef)
{
	memset(&s_sndstream, 0, sizeof(s_sndstream));

	UINT rate = soundmng_getRate(soundRef);

	switch (rate)
	{
		case 11025:
		case 22050:
		case 44100:
		case 48000:
		case 88200:
		case 96000:
		case 176400:
		case 192000:
			break;

		default:
			return FAILURE;
	}

	soundcfg.soundRef = soundRef;
	soundcfg.rate = rate;
	sound_changeclock();

	return SUCCESS;
}

void sound_destroy(void)
{
	soundcfg.rate = 0;
}

void sound_reset(void)
{
	soundcfg.lastclock = CPU_CLOCK;
	beep_eventreset();
	soundmng_reset(soundcfg.soundRef);
	s_sndstream.cur = 0;
}

void sound_changeclock(void)
{
	UINT32	clk;
	UINT	hz;
	UINT	hzmax;

	/* とりあえず 25で割り切れる。 */
	clk = pccore.realclock / 25;
	hz = soundcfg.rate / 25;

	/* で、クロック数に合せて調整。(64bit演算しろよな的) */
	hzmax = (1 << (32 - 8)) / (clk >> 8);
	while (hzmax < hz)
	{
		clk = (clk + 1) >> 1;
		hz = (hz + 1) >> 1;
	}
	TRACEOUT(("hzbase/clockbase = %d/%d", hz, clk));
	soundcfg.hzbase = hz;
	soundcfg.clockbase = clk;
	soundcfg.minclock = 2 * clk / hz;
	soundcfg.lastclock = CPU_CLOCK;
}

void sound_streamregist(const char *name, void *hdl, SOUNDCB cbfn)
{
	if (cbfn && s_sndstream.cur < STREAM_CBMAX){
		s_sndstream.cb[s_sndstream.cur].hdl = hdl;
		s_sndstream.cb[s_sndstream.cur].cbfn = cbfn;

		soundmng_addStream(soundcfg.soundRef, name, s_sndstream.cur);

		s_sndstream.cur++;
	}
}

/* ---- */

void sound_sync(void)
{
	UINT32	length = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK - soundcfg.lastclock;

	if (length < soundcfg.minclock) return;

	length = (length * soundcfg.hzbase) / soundcfg.clockbase;

	if (!length) return;

	streamprepare(s_sndstream, length);
	soundcfg.lastclock += length * soundcfg.clockbase / soundcfg.hzbase;
	beep_eventreset();
}
