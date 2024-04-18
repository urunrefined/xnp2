#include <alsa/asoundlib.h>

#include "cmamidi.h"

struct SerialMidi {
    struct _commng commng;
    snd_rawmidi_t *handle;
};

static UINT serialread(struct _commng *self, UINT8 *data) {
    struct SerialMidi *midi = (struct SerialMidi *)self;

    ssize_t ret = snd_rawmidi_read(midi->handle, data, 1);

    if (ret <= 0)
        return 0;
    return 1;
}

// TODO: Handle EAGAIN
static UINT serialwrite(struct _commng *self, UINT8 data) {
    struct SerialMidi *midi = (struct SerialMidi *)self;

    snd_rawmidi_write(midi->handle, &data, 1);
    snd_rawmidi_drain(midi->handle);

    return 0;
}

static UINT8 serialgetstat(struct _commng *self) {
    (void)self;
    return 0x00;
}

static INTPTR serialmsg(struct _commng *self, UINT msg, INTPTR param) {
    (void)self;
    (void)msg;
    (void)param;

    return 0;
}

static void serialrelease(struct _commng *self) {
    struct SerialMidi *midi = (struct SerialMidi *)self;

    snd_rawmidi_close(midi->handle);
    free(midi);
}

struct _commng *cmALSASerial_create(const char *hwdevice) {
    printf("(ALSA Device %s)\n", hwdevice ? hwdevice : "NULL");
    snd_rawmidi_t *midiHandle = 0;

    int err = snd_rawmidi_open(NULL, &midiHandle, hwdevice, 0);

    if (err) {
        printf("%s: Cannot open %s", __FUNCTION__, hwdevice);
        return 0;
    }

    struct SerialMidi *serial =
        (struct SerialMidi *)malloc(sizeof(struct SerialMidi));

    if (!serial) {
        printf("%s: memory alloc failure\n", __FUNCTION__);
        snd_rawmidi_close(midiHandle);
        return 0;
    }

    memset(serial, 0, sizeof(struct SerialMidi));
    serial->handle = midiHandle;

    serial->commng.connect = COMCONNECT_SERIAL;
    serial->commng.read = serialread;
    serial->commng.write = serialwrite;
    serial->commng.getstat = serialgetstat;
    serial->commng.msg = serialmsg;
    serial->commng.release = serialrelease;

    return (struct _commng *)serial;
}
