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
	UINT8		scrnmode;
	volatile int	drawing;
	int		width;		/* drawarea の width */
	int		height;		/* drawarea の height */
	int		extend;
	int		clipping;

	PAL16MASK	pal16mask;

	RECT_T		scrn;		/* drawarea 内の描画領域位置 */
	RECT_T		rect;		/* drawarea に描画するサイズ */

	/* toolkit depend */
	GdkPixbuf	   *drawsurf;
	GdkPixbuf	   *backsurf;
	GdkPixbuf	   *surface;
	double		   ratio_w, ratio_h;
	GdkInterpType  interp;

	GdkColor	pal[NP2PAL_MAX];
} DRAWMNG;

typedef struct {
	int	width;
	int	height;
	int	multiple;
} SCRNSTAT;

static SCRNMNG scrnmng;
static DRAWMNG drawmng;
static SCRNSTAT scrnstat = { 640, 400, SCREEN_DEFMUL };
static SCRNSURF scrnsurf;

SCRNMNG *scrnmngp = &scrnmng;

GtkWidget *main_window;
GtkWidget *drawarea;

#define	BITS_PER_PIXEL	24
#define	BYTES_PER_PIXEL	3

static void
set_window_size(int width, int height)
{
	gtk_widget_set_size_request(drawarea, width, height);
}

static void
renewal_client_size(void)
{
	int width;
	int height;

	int scrnwidth;
	int scrnheight;
	int multiple;

	width = std::min(scrnstat.width, drawmng.width);
	height = std::min(scrnstat.height, drawmng.height);

	multiple = scrnstat.multiple;

	scrnwidth = (width * multiple) / SCREEN_DEFMUL;
	scrnheight = (height * multiple) / SCREEN_DEFMUL;

	drawmng.rect.width = scrnwidth;
	drawmng.rect.height = scrnheight;

	drawmng.ratio_w = (double)drawmng.rect.width / width;
	drawmng.ratio_h = (double)drawmng.rect.height / height;

	drawmng.scrn.left = 0;
	drawmng.scrn.top = 0;

	drawmng.scrn.width = scrnwidth;
	drawmng.scrn.height = scrnheight;

	set_window_size(scrnwidth, scrnheight);

	scrnsurf.width = width;
	scrnsurf.height = height;
}

static void
palette_init(void)
{
	GdkColormap *cmap;
	gboolean success;
	int i;

	cmap = gdk_colormap_get_system();
	for (i = 0; i < 8; i++) {
		drawmng.pal[NP2PAL_TEXT + i + 1].pixel =
		    (np2_pal32[NP2PAL_TEXT + i + 1].p.r << 0) |
		    (np2_pal32[NP2PAL_TEXT + i + 1].p.g << 8) |
		    (np2_pal32[NP2PAL_TEXT + i + 1].p.b << 16);
		drawmng.pal[NP2PAL_TEXT + i + 1].red =
		    np2_pal32[NP2PAL_TEXT + i + 1].p.r << 8;
		drawmng.pal[NP2PAL_TEXT + i + 1].green =
		    np2_pal32[NP2PAL_TEXT + i + 1].p.g << 8;
		drawmng.pal[NP2PAL_TEXT + i + 1].blue =
		    np2_pal32[NP2PAL_TEXT + i + 1].p.b << 8;
	}

	gdk_colormap_alloc_colors(cmap, &drawmng.pal[NP2PAL_TEXT + 1], 8,
	    TRUE, FALSE, &success);
}

static void
palette_set(void)
{
	static int first = 1;
	GdkColormap *cmap;
	gboolean success;
	int i;

	cmap = gdk_colormap_get_system();

	if (!first) {
		gdk_colormap_free_colors(cmap, &drawmng.pal[NP2PAL_GRPH],
		    NP2PALS_GRPH);
	}
	first = 0;

	for (i = 0; i < NP2PALS_GRPH; i++) {
		drawmng.pal[NP2PAL_GRPH + i].pixel =
		    (np2_pal32[NP2PAL_GRPH + i].p.r << 0) |
		    (np2_pal32[NP2PAL_GRPH + i].p.g << 8) |
		    (np2_pal32[NP2PAL_GRPH + i].p.b << 16);
		drawmng.pal[NP2PAL_GRPH + i].red =
		    np2_pal32[NP2PAL_GRPH + i].p.r << 8;
		drawmng.pal[NP2PAL_GRPH + i].green =
		    np2_pal32[NP2PAL_GRPH + i].p.g << 8;
		drawmng.pal[NP2PAL_GRPH + i].blue =
		    np2_pal32[NP2PAL_GRPH + i].p.b << 8;
	}
	gdk_colormap_alloc_colors(cmap, &drawmng.pal[NP2PAL_GRPH], NP2PALS_GRPH,
	    TRUE, FALSE, &success);
}

void
scrnmng_initialize(void)
{

	drawmng.drawing = FALSE;
	scrnstat.width = 640;
	scrnstat.height = 400;
	scrnstat.multiple = SCREEN_DEFMUL;
	set_window_size(scrnstat.width, scrnstat.height);

	//real_fullscreen = gtk_window_init_fullscreen(main_window);

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
}

BRESULT
scrnmng_create(UINT8 mode)
{
	GdkScreen *screen;
	GdkVisual *visual;
	RECT_T rect;
	pixmap_format_t fmt;

	while (drawmng.drawing)
		gtk_main_iteration_do(FALSE);
	drawmng.drawing = TRUE;

	visual = gtk_widget_get_visual(drawarea);
	if (!gtkdrawmng_getformat(drawarea, main_window, &fmt))
		return FAILURE;

	switch (fmt.bits_per_pixel) {
	case 16:
		drawmng_make16mask(&drawmng.pal16mask, visual->blue_mask,
		    visual->red_mask, visual->green_mask);
		break;

	case 8:
		palette_init();
		break;
	}

	scrnmng.flag = 0;
	drawmng.extend = 0;

	screen = gdk_screen_get_default();
	drawmng.width = gdk_screen_get_width(screen);
	drawmng.height = gdk_screen_get_height(screen);

	rect.width = 640;
	rect.height = 480;

	scrnmng.bpp = BITS_PER_PIXEL;
	scrnsurf.bpp = BITS_PER_PIXEL;
	drawmng.scrnmode = mode;
	drawmng.clipping = 0;
	renewal_client_size();

	drawmng.backsurf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8,
		rect.width, rect.height);
	if (drawmng.backsurf == NULL) {
		drawmng.drawing = FALSE;
		g_message("can't create backsurf.");
		return FAILURE;
	}
	gdk_pixbuf_fill(drawmng.backsurf, 50);

	drawmng.surface = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8,
		drawmng.rect.width, drawmng.rect.height);
	if (drawmng.surface == NULL) {
		drawmng.drawing = FALSE;
		g_message("can't create surface.");
		return FAILURE;
	}
	gdk_pixbuf_fill(drawmng.surface, 80);


	drawmng.drawsurf = (scrnstat.multiple == SCREEN_DEFMUL)
		? drawmng.backsurf : drawmng.surface;
	gtk_window_restore_mode(main_window);

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

RGB16
scrnmng_makepal16(RGB32 pal32)
{

	return drawmng_makepal16(&drawmng.pal16mask, pal32);
}

void
scrnmng_setwidth(int posx, int width)
{
	(void) posx;

	scrnstat.width = width;
	renewal_client_size();
}

void
scrnmng_setheight(int posy, int height)
{
	(void) posy;

	scrnstat.height = height;
	renewal_client_size();
}

void
scrnmng_setextend(int extend)
{
	renewal_client_size();
}

void
scrnmng_setmultiple(int multiple)
{

	if (scrnstat.multiple != multiple) {
		scrnstat.multiple = multiple;

		soundmng_stop();
		mouse_running(MOUSE_STOP);
		scrnmng_destroy();
		if (scrnmng_create(scrnmode) != SUCCESS) {
			toolkit_widget_quit();
			return;
		}
		renewal_client_size();
		scrndraw_redraw();
		mouse_running(MOUSE_CONT);
		soundmng_play();
	}
}

const SCRNSURF *
scrnmng_surflock(void)
{
	const int lpitch = gdk_pixbuf_get_rowstride(drawmng.backsurf);

	scrnsurf.ptr = (UINT8 *)gdk_pixbuf_get_pixels(drawmng.backsurf);

	scrnsurf.xalign = BYTES_PER_PIXEL;
	scrnsurf.yalign = lpitch;

	return &scrnsurf;
}

void
scrnmng_surfunlock(const SCRNSURF *surf)
{

	if (drawmng.drawsurf == drawmng.surface) {
		gdk_pixbuf_scale(drawmng.backsurf, drawmng.surface,
			0, 0, drawmng.rect.width, drawmng.rect.height,
			0, 0, drawmng.ratio_w, drawmng.ratio_h,
		    drawmng.interp);
	}

	scrnmng_update();
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
	const int offsetX, const uint offsetY, const uint drawWidth, const uint drawHeight, const uint parentWindowWidth, const uint parentWindowHeight){
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
	rects[0] = {0,0,0,0};
	rects[1] = {0,0,0,0};
	rects[2] = {0,0,0,0};
	rects[3] = {0,0,0,0};

	getUpperSide(rects[0], offsetY, parentWindowWidth);
	getLeftSideWithoutUpper(rects[1], offsetX, offsetY, parentWindowHeight);
	getLowerSideWithoutLeft(rects[2], offsetX, offsetY, drawHeight, parentWindowWidth, parentWindowHeight);
	getRightSideWithoutUpperOrLower(rects[3], offsetX, offsetY, drawWidth, drawHeight, parentWindowWidth, parentWindowHeight);
};

void
scrnmng_update(void)
{
	GdkDrawable *d = drawarea->window;

	unsigned int drawableWidth =  (unsigned int)gdk_window_get_width (d);
	unsigned int drawableHeight = (unsigned int)gdk_window_get_height (d);

	unsigned int offsetX = getCenteringOffset(drawmng.rect.width, drawableWidth);
	unsigned int offsetY = getCenteringOffset(drawmng.rect.height, drawableHeight);

	GdkGC *gc = drawarea->style->fg_gc[gtk_widget_get_state(drawarea)];

	if (scrnmng.palchanged) {
		scrnmng.palchanged = FALSE;
		palette_set();
	}

	if (drawmng.drawing)
		return;

	drawmng.drawing = TRUE;

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
		drawmng.rect.width, drawmng.rect.height,
		gdk_window_get_width(d), gdk_window_get_height(d),
		borderRects
	);

	for(RECT_T& rect : borderRects){
		if(rect.height && rect.width){
			gdk_draw_rectangle(d, gc, TRUE,
				rect.left, rect.top, rect.width, rect.height);
		}
	}

	gdk_draw_pixbuf(d, gc, drawmng.drawsurf,
		0, 0,
		offsetX, offsetY,
		drawmng.rect.width, drawmng.rect.height,
		GDK_RGB_DITHER_NONE, 0, 0);

	drawmng.drawing = FALSE;
}
