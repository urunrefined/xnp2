#pragma once

#if defined(SUPPORT_MEMDBG32)

#ifdef __cplusplus
extern "C" {
#endif

void debugwin_create(void);
void debugwin_destroy(void);
void debugwin_process(void);

#ifdef __cplusplus
}
#endif

#else

#define	debugwin_create()
#define	debugwin_destroy()
#define	debugwin_process()

#endif
