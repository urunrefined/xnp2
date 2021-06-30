/**
 * @file	sound.c
 * @brief	Implementation of the sound
 */

#include "compiler.h"
#include "sound.h"
#include "cpucore.h"
#include "pccore.h"
#include "iocore.h"
#include "sndcsec.h"
#include "beep.h"
#include "soundmng.h"
#include "trace.h"
#include "_memory.h"

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
	SINT32	*buffer;
	SINT32	*ptr;
	UINT	samples;
	UINT	reserve;
	UINT	remain;
	CBTBL	*cbreg;
	CBTBL	cb[STREAM_CBMAX];
};
typedef struct tagSndStream SNDSTREAM;			/*!< Define */
typedef struct tagSndStream *PSNDSTREAM;		/*!< Define */

static	SNDSTREAM	s_sndstream;

/**
 * Reset
 * @param[in] sndstream The instance
 */
static void streamreset(PSNDSTREAM sndstream)
{
	sndstream->ptr = sndstream->buffer;
	sndstream->remain = sndstream->samples + sndstream->reserve;
	sndstream->cbreg = sndstream->cb;
}

/**
 * Prepare
 * @param[in] sndstream The instance
 * @param[in] samples The samples
 */
static void streamprepare(PSNDSTREAM sndstream, UINT samples)
{
	CBTBL	*cb;
	UINT	count;

	count = min(sndstream->remain, samples);
	if (count)
	{
		memset(sndstream->ptr, 0, count * 2 * sizeof(SINT32));
		for (cb = sndstream->cb; cb < sndstream->cbreg; cb++)
		{
			cb->cbfn(cb->hdl, sndstream->ptr, count);
		}
		sndstream->ptr += count * 2;
		sndstream->remain -= count;
	}
}

/* ---- */

BRESULT sound_create(UINT rate, UINT ms)
{
	UINT	samples;
	UINT	reserve;

	memset(&s_sndstream, 0, sizeof(s_sndstream));
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
	samples = soundmng_create(rate, ms);
	if (samples == 0)
	{
		goto scre_err1;
	}
	soundmng_reset();

	soundcfg.rate = rate;
	sound_changeclock();

#if defined(SOUNDRESERVE)
	reserve = rate * SOUNDRESERVE / 1000;
#else
	reserve = 0;
#endif
	s_sndstream.buffer = (SINT32 *)_MALLOC((samples + reserve) * 2 * sizeof(SINT32), "stream");
	if (s_sndstream.buffer == NULL)
	{
		goto scre_err2;
	}
	s_sndstream.samples = samples;
	s_sndstream.reserve = reserve;

	streamreset(&s_sndstream);
	return SUCCESS;

scre_err2:
	soundmng_destroy();

scre_err1:
	return FAILURE;
}

void sound_destroy(void)
{
	if (s_sndstream.buffer)
	{
		soundmng_stop();
		streamreset(&s_sndstream);
		soundmng_destroy();
		_MFREE(s_sndstream.buffer);
		s_sndstream.buffer = NULL;
	}

	soundcfg.rate = 0;
}

void sound_reset(void)
{
	if (s_sndstream.buffer)
	{
		soundmng_reset();
		streamreset(&s_sndstream);
		soundcfg.lastclock = CPU_CLOCK;
		beep_eventreset();
	}
}

void sound_changeclock(void)
{
	UINT32	clk;
	UINT	hz;
	UINT	hzmax;

	if (s_sndstream.buffer == NULL)
	{
		return;
	}

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

void sound_streamregist(void *hdl, SOUNDCB cbfn)
{
	if (s_sndstream.buffer)
	{
		if ((cbfn) && (s_sndstream.cbreg < (s_sndstream.cb + STREAM_CBMAX)))
		{
			s_sndstream.cbreg->hdl = hdl;
			s_sndstream.cbreg->cbfn = cbfn;
			s_sndstream.cbreg++;
		}
	}
}


/* ---- */

void sound_sync(void)
{
	UINT32	length;

	if (s_sndstream.buffer == NULL)
	{
		return;
	}

	length = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK - soundcfg.lastclock;
	if (length < soundcfg.minclock)
	{
		return;
	}
	length = (length * soundcfg.hzbase) / soundcfg.clockbase;
	if (length == 0)
	{
		return;
	}

	streamprepare(&s_sndstream, length);
	soundcfg.lastclock += length * soundcfg.clockbase / soundcfg.hzbase;
	beep_eventreset();

	soundcfg.writecount += length;
	if (soundcfg.writecount >= 100)
	{
		soundcfg.writecount = 0;
		SNDCSEC_LEAVE;
		soundmng_sync();
		SNDCSEC_ENTER;
	}
}

static volatile int locks = 0;

/**
 * PCM バッファを得る (フレームワークから呼ばれる)
 * @return バッファ
 */
const SINT32 *sound_pcmlock(void)
{
	const SINT32 *ret;

	if (locks)
	{
		TRACEOUT(("sound pcm lock: already locked"));
		return NULL;
	}

	SNDCSEC_ENTER;
	locks++;
	ret = s_sndstream.buffer;
	if (ret)
	{
		if (s_sndstream.remain > s_sndstream.reserve)
		{
			streamprepare(&s_sndstream, s_sndstream.remain - s_sndstream.reserve);
			soundcfg.lastclock = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
			beep_eventreset();
		}
	}
	else
	{
		locks--;
		SNDCSEC_LEAVE;
	}
	return ret;
}

/**
 * PCM バッファを解放する
 * @param[in] hdl バッファ
 */
void sound_pcmunlock(const SINT32 *hdl)
{
	int		leng;

	if (hdl)
	{
		leng = s_sndstream.reserve - s_sndstream.remain;
		if (leng > 0)
		{
			memcpy(s_sndstream.buffer, s_sndstream.buffer + (s_sndstream.samples * 2), leng * 2 * sizeof(SINT32));
		}
		s_sndstream.ptr = s_sndstream.buffer + (leng * 2);
		s_sndstream.remain = s_sndstream.samples + s_sndstream.reserve - leng;
		locks--;
		SNDCSEC_LEAVE;
	}
}
