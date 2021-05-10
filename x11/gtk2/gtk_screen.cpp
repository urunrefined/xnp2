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

#include <math.h>

#include "np2.h"
#include "palettes.h"
#include "scrndraw.h"

#include "toolkit.h"
#include "common.h"

#include "scrnmng.h"
#include "mousemng.h"
#include "soundmng.h"

#include "gtk2/xnp2.h"
#include "gtk2/gtk_drawmng.h"
#include "gtk2/gtk_menu.h"

#include <algorithm>

typedef struct {
	//TODO: Replace with a proper mutex
	volatile int	drawing;

	GdkPixbuf	   *backsurf;
	GdkPixbuf	   *surface;

	GdkInterpType  interp;
	ScaleMode      scaleMode;
	int	multiple;

} DRAWMNG;

static const unsigned int pc98Width = 640;
static const unsigned int pc98Height = 400;

static SCRNMNG scrnmng;
static DRAWMNG drawmng;
static SCRNSURF scrnsurf;

SCRNMNG *scrnmngp = &scrnmng;

GtkWidget *main_window;
GtkWidget *drawarea;

#define	BITS_PER_PIXEL	24
#define	BYTES_PER_PIXEL	3

void
scrnmng_initialize(void)
{
	drawmng.drawing = FALSE;
	drawmng.multiple = SCREEN_DEFMUL;

	gtk_widget_set_size_request(drawarea, pc98Width, pc98Height);

	switch (np2oscfg.drawinterp) {
	case INTERP_NEAREST:
		drawmng.interp = GDK_INTERP_NEAREST;
		break;

	case INTERP_TILES:
		drawmng.interp = GDK_INTERP_TILES;
		break;

	case INTERP_HYPER:
		drawmng.interp = GDK_INTERP_HYPER;
		break;

	case INTERP_BILINEAR:
	default:
		drawmng.interp = GDK_INTERP_BILINEAR;
		break;
	}

	drawmng.scaleMode = ScaleMode::SCALEMODE_INTEGER;
}

static void
scrnmng_setMinimumCanvasSize(void){
	gtk_widget_set_size_request(drawarea,
		pc98Width * drawmng.multiple /SCREEN_DEFMUL,
		pc98Height * drawmng.multiple /SCREEN_DEFMUL);
}


static GdkPixbuf *getPixmapWithDimensions(
		unsigned int wantedWidth, unsigned int wantedHeight){

	if(drawmng.surface){
		unsigned int currentWidth = gdk_pixbuf_get_width(drawmng.surface);
		unsigned int currentHeight = gdk_pixbuf_get_height(drawmng.surface);

		if(wantedWidth == currentWidth && wantedHeight == currentHeight){
			//Old surface matches, no need to recreate.
			return drawmng.surface;
		}

		//Doesn't fit, recreate it
		g_object_unref(drawmng.surface);
		drawmng.surface = NULL;
	}

	drawmng.surface = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8,
		wantedWidth, wantedHeight);

	gdk_pixbuf_fill(drawmng.surface, 0xAA);

	return drawmng.surface;
}

BRESULT
scrnmng_create(UINT8 mode)
{
	(void) mode;

	while (drawmng.drawing)
		gtk_main_iteration_do(FALSE);
	drawmng.drawing = TRUE;

	scrnmng.bpp = BITS_PER_PIXEL;
	scrnsurf.bpp = BITS_PER_PIXEL;
	scrnsurf.height = pc98Height;
	scrnsurf.width = pc98Width;

	//This is what the engine will draw into
	drawmng.backsurf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, pc98Width, pc98Height);
	if (drawmng.backsurf == NULL) {
		drawmng.drawing = FALSE;
		g_message("can't create backsurf.");
		return FAILURE;
	}

	gdk_pixbuf_fill(drawmng.backsurf, 0);

	//This surface will be shown to the user
	drawmng.surface = NULL;
	drawmng.drawing = FALSE;

	return SUCCESS;
}

void
scrnmng_destroy(void)
{

	if (drawmng.backsurf) {
		g_object_unref(drawmng.backsurf);
		drawmng.backsurf = NULL;
	}

	if (drawmng.surface) {
		g_object_unref(drawmng.surface);
		drawmng.surface = NULL;
	}
}

void
scrnmng_renewal()
{
	scrnmng_surfunlock(0);
}

void
scrnmng_setmultiple(int multiple)
{
	drawmng.multiple = multiple;
	scrnmng_setMinimumCanvasSize();
}

void scrnmng_setScaleMode(ScaleMode scaleMode){
	drawmng.scaleMode = scaleMode;
}

const SCRNSURF *
scrnmng_surflock(void)
{

	scrnsurf.ptr = (UINT8 *)gdk_pixbuf_get_pixels(drawmng.backsurf);
	scrnsurf.xalign = BYTES_PER_PIXEL;
	scrnsurf.yalign = gdk_pixbuf_get_rowstride(drawmng.backsurf);

	return &scrnsurf;
}

static unsigned int getCenteringOffset(unsigned int inner, unsigned int outer){
	if(inner >= outer) return 0;

	return (outer - inner) / 2;
}

static void getUpperSide(RECT_T& rect,
	const uint offsetY, const uint parentWindowWidth){
	rect.top = 0;
	rect.left = 0;
	rect.width = parentWindowWidth;
	rect.height = offsetY;
}

static void getLeftSideWithoutUpper(RECT_T& rect,
	const int offsetX, const uint offsetY, const uint parentWindowHeight){
	rect.top = offsetY;
	rect.left = 0;
	rect.width = offsetX;
	rect.height = parentWindowHeight - offsetY;
}

static void getLowerSideWithoutLeft(RECT_T& rect,
	const int offsetX, const uint offsetY, const uint drawHeight, const uint parentWindowWidth, const uint parentWindowHeight){
	rect.top = offsetY + drawHeight;
	rect.left = offsetX;
	rect.width = parentWindowWidth - offsetX;
	rect.height = parentWindowHeight - offsetY;
}

static void getRightSideWithoutUpperOrLower(RECT_T& rect,
	const int offsetX, const uint offsetY, const uint drawWidth, const uint drawHeight, const uint parentWindowWidth){
	rect.top = offsetY;
	rect.left = offsetX + drawWidth;
	rect.width = parentWindowWidth - (offsetX + drawWidth);
	rect.height = drawHeight;
}

static void getBorderRects(
	const uint offsetX, const uint offsetY,
		const uint drawWidth, const uint drawHeight,
	const uint parentWindowWidth, const uint parentWindowHeight,
	RECT_T (&rects)[4])
{
	getUpperSide(rects[0], offsetY, parentWindowWidth);
	getLeftSideWithoutUpper(rects[1], offsetX, offsetY, parentWindowHeight);
	getLowerSideWithoutLeft(rects[2], offsetX, offsetY, drawHeight, parentWindowWidth, parentWindowHeight);
	getRightSideWithoutUpperOrLower(rects[3], offsetX, offsetY, drawWidth, drawHeight, parentWindowWidth);
};

static void
scrnmng_update(GdkDrawable *d, GdkPixbuf *toDraw)
{	
	unsigned int updateWidth = gdk_pixbuf_get_width(drawmng.surface);
	unsigned int updateHeight = gdk_pixbuf_get_height(drawmng.surface);

	unsigned int drawableWidth =  (unsigned int)gdk_window_get_width (d);
	unsigned int drawableHeight = (unsigned int)gdk_window_get_height (d);

	unsigned int offsetX = getCenteringOffset(updateWidth, drawableWidth);
	unsigned int offsetY = getCenteringOffset(updateHeight, drawableHeight);

	if (scrnmng.palchanged) {
		scrnmng.palchanged = FALSE;
	}

	if (drawmng.drawing)
		return;

	drawmng.drawing = TRUE;

	GdkGC *gc = drawarea->style->fg_gc[gtk_widget_get_state(drawarea)];

	/*
	 * We need to preserve the previous frame and can't
	 * just paint over the entire canvas, as we might lose
	 * previous pixels due to interlacing, so
	 * we paint by finding out the border rectangles around
	 * the drawing area
	*/

	RECT_T borderRects[4];

	getBorderRects(
		offsetX, offsetY,
		updateWidth, updateHeight,
		gdk_window_get_width(d), gdk_window_get_height(d),
		borderRects
	);

	for(RECT_T& rect : borderRects){
		if(rect.height && rect.width){
			gdk_draw_rectangle(d, gc, TRUE,
				rect.left, rect.top, rect.width, rect.height);
		}
	}

	gdk_draw_pixbuf(d, gc, toDraw,
		0, 0,
		offsetX, offsetY,
		updateWidth, updateHeight,
		GDK_RGB_DITHER_NONE, 0, 0);

	drawmng.drawing = FALSE;
}

void
scrnmng_surfunlock(const SCRNSURF *surf)
{
	(void) surf;

	/* scale up the backsurface (which is always 640x400)
	 * to whatever we need and put it into the actual surface,
	 * which will be shown on the screen.
	 * */

	if(drawmng.scaleMode == SCALEMODE_INTEGER){
		int drawWidth = pc98Width * drawmng.multiple / SCREEN_DEFMUL;
		int drawHeight = pc98Height * drawmng.multiple / SCREEN_DEFMUL;

		double ratio_w = (double)drawWidth / pc98Width;
		double ratio_h = (double)drawHeight / pc98Height;

		GdkPixbuf *front = getPixmapWithDimensions(drawWidth, drawHeight);

		gdk_pixbuf_scale(drawmng.backsurf, front,
			0, 0, drawWidth, drawHeight,
			0, 0, ratio_w, ratio_h,
			drawmng.interp);

		scrnmng_update(drawarea->window, front);
	}
	else {
		GdkDrawable *d = drawarea->window;

		unsigned int drawableWidth =  (unsigned int)gdk_window_get_width (d);
		unsigned int drawableHeight = (unsigned int)gdk_window_get_height (d);

		GdkPixbuf *front = getPixmapWithDimensions(drawableWidth, drawableHeight);

		double ratio_w = (double)drawableWidth / pc98Width;
		double ratio_h = (double)drawableHeight / pc98Height;

		gdk_pixbuf_scale(drawmng.backsurf, front,
			0, 0, drawableWidth, drawableHeight,
			0, 0, ratio_w, ratio_h,
			drawmng.interp);

		scrnmng_update(drawarea->window, front);
	}

}
