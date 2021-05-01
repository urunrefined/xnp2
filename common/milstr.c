#include	"compiler.h"

int STRCALL milutf8_charsize(const char *str) {

	if (str[0] == '\0') {
		return(0);
	}
	else if (!(str[0] & 0x80)) {
		return(1);
	}
	else if ((str[0] & 0xe0) == 0xc0) {
		if ((str[1] & 0xc0) == 0x80) {
			return(2);
		}
	}
	else if ((str[0] & 0xf0) == 0xe0) {
		if (((str[1] & 0xc0) == 0x80) ||
			((str[2] & 0xc0) == 0x80)) {
			return(3);
		}
	}
	return(0);
}

int STRCALL milutf8_cmp(const char *str, const char *cmp) {

	int		s;
	int		c;

	do {
		s = (UINT8)*str++;
		if (((s - 'a') & 0xff) < 26) {
			s -= 0x20;
		}
		c = (UINT8)*cmp++;
		if (((c - 'a') & 0xff) < 26) {
			c -= 0x20;
		}
		if (s != c) {
			return((s > c)?1:-1);
		}
	} while(s);
	return(0);
}

int STRCALL milutf8_memcmp(const char *str, const char *cmp) {

	int		s;
	int		c;

	do {
		c = (UINT8)*cmp++;
		if (c == 0) {
			return(0);
		}
		if (((c - 'a') & 0xff) < 26) {
			c -= 0x20;
		}
		s = (UINT8)*str++;
		if (((s - 'a') & 0xff) < 26) {
			s -= 0x20;
		}
	} while(s == c);
	return((s > c)?1:-1);
}

int STRCALL milutf8_kanji1st(const char *str, int pos) {

	return(((str[pos] & 0xc0) >= 0xc0)?1:0);
}

int STRCALL milutf8_kanji2nd(const char *str, int pos) {

	return(((str[pos] & 0xc0) == 0x80)?1:0);
}

void STRCALL milutf8_ncpy(char *dst, const char *src, UINT maxlen) {

	UINT	i;

	if (maxlen > 0) {
		maxlen--;
		for (i=0; i<maxlen && src[i]; i++) {
			dst[i] = src[i];
		}
		dst[i] = '\0';
		if (i) {
			do {
				i--;
			} while((i) && ((dst[i] & 0xc0) == 0x80));
			i += milutf8_charsize(dst + i);
			dst[i] = '\0';
		}
	}
}

void STRCALL milutf8_ncat(char *dst, const char *src, UINT maxlen) {

	UINT	i;
	UINT	j;

	if (maxlen > 0) {
		maxlen--;
		for (i=0; i<maxlen; i++) {
			if (!dst[i]) {
				break;
			}
		}
		for (j=0; i<maxlen && src[j]; i++, j++) {
			dst[i] = src[j];
		}
		dst[i] = '\0';
		if (i) {
			do {
				i--;
			} while((i) && ((dst[i] & 0xc0) == 0x80));
			i += milutf8_charsize(dst + i);
			dst[i] = '\0';
		}
	}
}

char * STRCALL milutf8_chr(const char *str, int c) {

	int		s;

	if (str) {
		do {
			s = *str;
			if (s == c) {
				return((char *)str);
			}
			str++;
		} while(s);
	}
	return(NULL);
}

// ---- other

int STRCALL milstr_extendcmp(const OEMCHAR *str, const OEMCHAR *cmp) {

	int		c;
	int		s;

	do {
		while(1) {
			c = (UINT8)*cmp++;
			if (!c) {
				return(0);
			}
			if (((c - '0') & 0xff) < 10) {
				break;
			}
			c |= 0x20;
			if (((c - 'a') & 0xff) < 26) {
				break;
			}
		}
		while(1) {
			s = *str++;
			if (!s) {
				break;
			}
			if (((s - '0') & 0xff) < 10) {
				break;
			}
			s |= 0x20;
			if (((s - 'a') & 0xff) < 26) {
				break;
			}
		}
	} while(s == c);
	return((s > c)?1:-1);
}

OEMCHAR * STRCALL milstr_nextword(const OEMCHAR *str) {

	if (str) {
		while((*str > '\0') && (*str <= ' ')) {
			str++;
		}
	}
	return((OEMCHAR *)str);
}

UINT STRCALL milstr_getarg(OEMCHAR *str, OEMCHAR *arg[], UINT maxarg) {

	UINT	ret = 0;
	OEMCHAR	*p;
	BOOL	quot;

	while(maxarg--) {
		quot = FALSE;
		while((*str > '\0') && (*str <= ' ')) {
			str++;
		}
		if (*str == '\0') {
			break;
		}
		arg[ret++] = str;
		p = str;
		while(*str) {
			if (*str == '\"') {
				quot = !quot;
			}
			else if ((quot) || (*str < '\0') || (*str > ' ')) {
				*p++ = *str;
			}
			else {
				str++;
				break;
			}
			str++;
		}
		*p = '\0';
	}
	return(ret);
}

long STRCALL milstr_solveHEX(const OEMCHAR *str) {

	long	ret;
	int		i;
	OEMCHAR	c;

	ret = 0;
	for (i=0; i<8; i++) {
		c = *str++;
		if ((c >= '0') && (c <= '9')) {
			c -= '0';
		}
		else if ((c >= 'A') && (c <= 'F')) {
			c -= '7';
		}
		else if ((c >= 'a') && (c <= 'f')) {
			c -= 'W';
		}
		else {
			break;
		}
		ret <<= 4;
		ret += (long)c;
	}
	return(ret);
}

long STRCALL milstr_solveINT(const OEMCHAR *str) {

	unsigned long	ret;
	BOOL			minus;
	int				c;

	ret = 0;
	minus = FALSE;
	c = *str;
	if (c == '+') {
		str++;
	}
	else if (c == '-') {
		str++;
		minus = TRUE;
	}
	while(1) {
		c = *str++;
		c -= '0';
		if ((c >= 0) && (c < 10)) {
			ret *= 10;
			ret += c;
		}
		else {
			break;
		}
	}
	if (!minus) {
		return((long)ret);
	}
	else {
		return((long)(0 - ret));
	}
}

OEMCHAR * STRCALL milstr_list(const OEMCHAR *lststr, UINT pos) {

	if (lststr) {
		while(pos) {
			pos--;
			while(*lststr++ != '\0') {
			}
		}
	}
	return((OEMCHAR *)lststr);
}

