#include "pccorecallbacks.h"
#include "compiler.h"
#include "scrnmng.h"

extern "C" UINT8 mousemng_getstat(void *inContext, short *x, short *y) {

    BR::CallbackContext *context = (BR::CallbackContext *)inContext;

    auto &input = *(context->input);

    input.getMouseMove(*x, *y);
    input.mouse_move_x = 0;
    input.mouse_move_y = 0;

    UINT8 mouseButtonState = 0;

    // The entire logic here seems very strange....
    // Needs to be revisited
    if (input.getButton(BR::MouseButtons::BUTTON_LEFT)) {
        mouseButtonState &= 0x7f;
    } else {
        mouseButtonState |= 0x80;
    }

    if (input.getButton(BR::MouseButtons::BUTTON_RIGHT)) {
        mouseButtonState &= 0xdf;
    } else {
        mouseButtonState |= 0x20;
    }

    return mouseButtonState;
}

SCRNSURF scrnmng_surflock(void *inContext) {
    BR::CallbackContext *context = (BR::CallbackContext *)inContext;

    SCRNSURF scrnsurf;

    scrnsurf.ptr = (UINT8 *)context->texture->image.data.data();
    scrnsurf.bpp = 32;
    scrnsurf.width = context->texture->image.width;
    scrnsurf.height = context->texture->image.height;
    scrnsurf.xalign = 4;
    scrnsurf.yalign = context->texture->image.width * 4;

    return scrnsurf;
}

void scrnmng_surfunlock(void *inContext) {
    BR::CallbackContext *context = (BR::CallbackContext *)inContext;
    context->texture->textureDirty = true;
}
