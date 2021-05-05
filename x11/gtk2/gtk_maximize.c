
#include "gtk_maximize.h"
#include <assert.h>

/*
 * TODO: Remove this global eventually...
 * There's no way to query the maximization state
 * from a window... as far as I can tell, anyway...
*/

static int borderlessMaximizeState = 0;

void toggleBorderlessMaximize(GtkWindow *window){
	assert(window);

	if(borderlessMaximizeState){
		gtk_window_unmaximize(window);
		gtk_window_set_decorated(window, TRUE);
	}
	else {
		gtk_window_set_decorated(window, FALSE);
		gtk_window_maximize(window);
	}

	borderlessMaximizeState = !borderlessMaximizeState;
}

int isBorderlessMaximized(){
	return borderlessMaximizeState;
}
