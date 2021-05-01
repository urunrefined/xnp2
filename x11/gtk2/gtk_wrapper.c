/*
 * Copyright (c) 2002-2013 NONAKA Kimihiro
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include "gtk2/xnp2.h"

#include <gdk/gdkx.h>

extern int verbose;
extern volatile sig_atomic_t np2running;

#ifdef DEBUG
#ifndef	VERBOSE
#define	VERBOSE(s)	if (verbose) g_printerr s
#endif	/* !VERBOSE */
#else	/* !DEBUG */
#define	VERBOSE(s)
#endif	/* DEBUG */

void
gtk_scale_set_default_values(GtkScale *scale)
{

	gtk_range_set_update_policy(GTK_RANGE(scale), GTK_UPDATE_CONTINUOUS);
	gtk_scale_set_digits(scale, 1);
	gtk_scale_set_value_pos(scale, GTK_POS_RIGHT);
	gtk_scale_set_draw_value(scale, TRUE);
}

void
gdk_window_set_pointer(GdkWindow *w, gint x, gint y)
{ 
	Display *xdisplay;
	Window xwindow;

	g_return_if_fail(w != NULL);

	xdisplay = GDK_WINDOW_XDISPLAY(w);
	xwindow = GDK_WINDOW_XWINDOW(w);
	XWarpPointer(xdisplay, None, xwindow, 0, 0, 0, 0, x, y);
}

gboolean
gdk_window_get_pixmap_format(GdkWindow *w, GdkVisual *visual, pixmap_format_t *fmtp)
{
	Display *xdisplay;
	XPixmapFormatValues *format;
	int count;
	int i;

	g_return_val_if_fail(w != NULL, FALSE);
	g_return_val_if_fail(visual != NULL, FALSE);
	g_return_val_if_fail(fmtp != NULL, FALSE);

	xdisplay = GDK_WINDOW_XDISPLAY(w);
	format = XListPixmapFormats(xdisplay, &count);
	if (format) {
		for (i = 0; i < count; i++) {
			if (visual->depth == format[i].depth) {
				fmtp->depth = format[i].depth;
				fmtp->bits_per_pixel = format[i].bits_per_pixel;
				fmtp->scanline_pad = format[i].scanline_pad;
				XFree(format);
				return TRUE;
			}
		}
		XFree(format);
	}
	return FALSE;
}

/*
 * Full screen support.
 */
extern int ignore_fullscreen_mode;
static int use_xvid;
static int use_netwm;
static int is_fullscreen;

static int
check_netwm(GtkWidget *widget)
{
	Display *xdisplay;
	Window root_window;
	Atom _NET_SUPPORTED;
	Atom _NET_WM_STATE_FULLSCREEN;
	Atom type;
	int format;
	unsigned long nitems;
	unsigned long remain;
	unsigned char *prop;
	guint32 *data;
	int rv;
	unsigned long i;

	g_return_val_if_fail(widget != NULL, 0);

	xdisplay = GDK_WINDOW_XDISPLAY(widget->window);
	root_window = DefaultRootWindow(xdisplay);

	_NET_SUPPORTED = XInternAtom(xdisplay, "_NET_SUPPORTED", False);
	_NET_WM_STATE_FULLSCREEN = XInternAtom(xdisplay,
	    "_NET_WM_STATE_FULLSCREEN", False);

	rv = XGetWindowProperty(xdisplay, root_window, _NET_SUPPORTED,
	    0, 65536 / sizeof(guint32), False, AnyPropertyType,
	    &type, &format, &nitems, &remain, &prop);
	if (rv != Success) {
		return 0;
	}
	if (type != XA_ATOM) {
		return 0;
	}
	if (format != 32) {
		return 0;
	}

	rv = 0;
	data = (guint32 *)prop;
	for (i = 0; i < nitems; i++) {
		if (data[i] == _NET_WM_STATE_FULLSCREEN) {
			VERBOSE(("Support _NET_WM_STATE_FULLSCREEN\n"));
			rv = 1;
			break;
		}
	}
	XFree(prop);

	return rv;
}

int
gtk_window_init_fullscreen(GtkWidget *widget)
{
	use_netwm = check_netwm(widget);

	if (use_xvid && (ignore_fullscreen_mode & 1)) {
		VERBOSE(("Support XF86VidMode extension, but disabled\n"));
		use_xvid = 0;
	}
	if (use_netwm && (ignore_fullscreen_mode & 2)) {
		VERBOSE(("Support _NET_WM_STATE_FULLSCREEN, but disabled\n"));
		use_netwm = 0;
	}

	if (verbose) {
		if (use_xvid) {
			VERBOSE(("Using XF86VidMode extension\n"));
		} else if (use_netwm) {
			VERBOSE(("Using _NET_WM_STATE_FULLSCREEN\n"));
		} else {
			VERBOSE(("not supported\n"));
		}
	}

	return use_xvid;
}

void
gtk_window_fullscreen_mode(GtkWidget *widget)
{

	g_return_if_fail(widget != NULL);
	g_return_if_fail(widget->window != NULL);

	if (use_netwm) {
		gtk_window_fullscreen(GTK_WINDOW(widget));
	}
	is_fullscreen = 1;
}

void
gtk_window_restore_mode(GtkWidget *widget)
{

	g_return_if_fail(widget != NULL);

	if (!is_fullscreen)
		return;
	is_fullscreen = 0;

	if (use_netwm && GTK_IS_WINDOW(widget)) {
		gtk_window_unfullscreen(GTK_WINDOW(widget));
	}
}
