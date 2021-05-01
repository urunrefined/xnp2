#ifndef	NP2_X11_OEMTEXT_H__
#define	NP2_X11_OEMTEXT_H__

#include "codecnv/codecnv.h"

#define	oemtext_sjistooem	codecnv_sjistoutf8
#define	oemtext_oemtosjis	codecnv_utf8tosjis

#ifdef __cplusplus
#include <string>
namespace std
{
	typedef string oemstring;
}
#endif  /* __cplusplus */

#endif	/* NP2_X11_OEMTEXT_H__ */
