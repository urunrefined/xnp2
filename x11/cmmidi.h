#ifndef NP2_X11_CMMIDI_H__
#define NP2_X11_CMMIDI_H__

// ---- com manager midi for unix

#include "commng.h"

#ifdef __cplusplus
extern "C" {
#endif

extern struct _commgr *cm_mpu98;

enum {
    MIDI_MT32 = 0,
    MIDI_CM32L,
    MIDI_CM64,
    MIDI_CM300,
    MIDI_CM500LA,
    MIDI_CM500GS,
    MIDI_SC55,
    MIDI_SC88,
    MIDI_LA,
    MIDI_GM,
    MIDI_GS,
    MIDI_XG,
    MIDI_OTHER
};

extern const char cmmidi_vermouth[];

extern const char cmmidi_midiout_device[];
extern const char cmmidi_midiin_device[];

extern const char *cmmidi_mdlname[];

void cmmidi_initialize(void);
struct _commng *cmmidi_create(const char *midiout, const char *midiin,
                              const char *module);

#ifdef __cplusplus
}
#endif

#endif /* NP2_X11_CMMIDI_H__ */
