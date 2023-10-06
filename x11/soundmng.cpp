/*
 * Copyright (c) 2001-2003, 2015 NONAKA Kimihiro
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

#include "soundmng.h"
#include "compiler.h"
#include "parts.h"

#include <strings.h>
#include <sys/stat.h>

#include "sound/vermouth/vermouth.h"

#include "pulse/PulseSoundEngine.h"

#include <memory>

extern "C" MIDIMOD vermouth_module;
MIDIMOD vermouth_module = NULL;

UINT soundmng_getRate(void *soundRef) {
    BR::Sfx::PulseSoundEngine &engine =
        *((BR::Sfx::PulseSoundEngine *)soundRef);
    return engine.sampleRate;
}

BRESULT
soundmng_initialize(void) { return 0; }

void soundmng_deinitialize(void) {}

void soundmng_reset(void *soundRef) {

    if (!soundRef)
        return;

    BR::Sfx::PulseSoundEngine &engine =
        *((BR::Sfx::PulseSoundEngine *)soundRef);

    engine.reset();
}

void PARTSCALL saturation_s16_2(SINT16 *dst, const SINT32 *src, UINT size) {

    SINT32 data;

    while (size--) {
        data = *src++;
        if (data > 32767) {
            data = 32767;
        } else if (data < -32768) {
            data = -32768;
        }
        *dst++ = (SINT16)data;
    }
}

void soundmng_sync(void *soundRef, int index, const SINT32 *pcm, size_t sz) {
    if (!soundRef || !pcm || !sz)
        return;

    BR::Sfx::PulseSoundEngine &engine =
        *((BR::Sfx::PulseSoundEngine *)soundRef);

    SINT16 buffer[sz];
    saturation_s16_2(buffer, pcm, sz);

    engine.add(index, buffer, sz);
}

void soundmng_addStream(const void *soundRef, const char *name, int index) {
    if (!soundRef || !name)
        return;

    BR::Sfx::PulseSoundEngine &engine =
        *((BR::Sfx::PulseSoundEngine *)soundRef);
    engine.addStream(name, index);
}
