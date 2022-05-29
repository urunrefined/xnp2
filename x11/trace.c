#include	"compiler.h"
#include	<stdarg.h>
#include    <stdio.h>

void trace_fmt(const char *fmt, ...) {
	va_list	ap;
	va_start(ap, fmt);
	printf(fmt, ap);
	va_end(ap);
	printf("\n");
}
