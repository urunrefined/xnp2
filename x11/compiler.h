/*-
 * Copyright (C) 2003, 2004 NONAKA Kimihiro <nonakap@gmail.com>
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

#ifndef	NP2_X11_COMPILER_H__
#define	NP2_X11_COMPILER_H__

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define	BYTESEX_BIG
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define	BYTESEX_LITTLE
#else
#error Please set preprocessor macro __BYTE_ORDER__ to __ORDER_BIG_ENDIAN__ or __ORDER_LITTLE_ENDIAN__
#endif

#define	X11
#define	OSLINEBREAK_LF

#include <stdint.h>

typedef	int32_t		SINT;
typedef	uint32_t	UINT;

typedef	int8_t		SINT8;
typedef	int16_t		SINT16;
typedef	int32_t		SINT32;
typedef	int64_t		SINT64;

typedef	uint8_t		UINT8;
typedef	uint16_t	UINT16;
typedef	uint32_t	UINT32;
typedef	uint64_t	UINT64;

typedef	int			BOOL;

typedef signed long nxp2ptr;
#define	INTPTR nxp2ptr

#define PTR_TO_UINT32(p)	((unsigned int) (unsigned long) (p))
#define UINT32_TO_PTR(v)	((void*)(unsigned long) ((UINT32)(v)))

#ifndef	FALSE
#define	FALSE	0
#endif

#ifndef	TRUE
#define	TRUE	(!FALSE)
#endif

//TODO: Sort this out later
#define	MAX_PATH 4096

#ifndef	__cplusplus
#ifndef	max
#define	max(a,b)	(((a) > (b)) ? (a) : (b))
#endif
#ifndef	min
#define	min(a,b)	(((a) < (b)) ? (a) : (b))
#endif
#endif	/* !__cplusplus */

//TODO: Remove this and the ZeroMemory / FillMemory macros
#include <string.h>

#define	ZeroMemory(d,n)		memset((d), 0, (n))
#define	FillMemory(a, b, c)	memset((a), (c), (b))

#ifndef	roundup
#define	roundup(x, y)	((((x)+((y)-1))/(y))*(y))
#endif

#ifndef	NELEMENTS
#define	NELEMENTS(a)	((int)(sizeof(a) / sizeof(a[0])))
#endif

/* archtecture */
/* amd64 */
#if defined(amd64) || defined(__AMD64__) || defined(__amd64__) || \
    defined(x86_64) || defined(__x86_64__) || defined(__X86_64__)
#define	NP2_CPU_ARCH_AMD64
#endif /* amd64 */
/* i386 */
#if defined(i386) || defined(__i386__) || defined(NP2_CPU_ARCH_AMD64)
#define	NP2_CPU_ARCH_IA32
#endif /* i386 */

#if defined(__GNUC__)
#if defined(NP2_CPU_ARCH_IA32)
#define	GCC_CPU_ARCH_IA32
#endif
#if defined(NP2_CPU_ARCH_AMD64)
#define	GCC_CPU_ARCH_AMD64
#endif
#endif /* __GNUC__ */

#define	GETRAND()	random()
#define	SPRINTF		sprintf

#define	OEMCHAR		char
#define OEMTEXT(s)	s

#if defined(CPUCORE_IA32)
#define	msgbox(title, msg)	toolkit_messagebox(title, msg);

#if !defined(DISABLE_PC9821)
#define	SUPPORT_PC9821
#define	SUPPORT_CRT31KHZ
#endif
#define	SUPPORT_IDEIO
#else
#define	SUPPORT_CRT15KHZ
#endif

#if defined(NP2_CPU_ARCH_IA32)
#undef	MEMOPTIMIZE
#define LOADINTELDWORD(a)	(*((const UINT32 *)(a)))
#define LOADINTELWORD(a)	(*((const UINT16 *)(a)))
#define STOREINTELDWORD(a, b)	*(UINT32 *)(a) = (b)
#define STOREINTELWORD(a, b)	*(UINT16 *)(a) = (b)
#if !defined(DEBUG) && !defined(NP2_CPU_ARCH_AMD64)
#define	FASTCALL	__attribute__((regparm(2)))
#endif	/* !DEBUG && !NP2_CPU_ARCH_AMD64 */
#elif defined(arm) || defined (__arm__)
#define	MEMOPTIMIZE	2
#define	REG8		UINT
#define	REG16		UINT
#define	OPNGENARM
#else
#define	MEMOPTIMIZE	1
#endif

#ifndef	FASTCALL
#define	FASTCALL
#endif
#define	CPUCALL		FASTCALL
#define	MEMCALL		FASTCALL
#define	DMACCALL	FASTCALL
#define	IOOUTCALL	FASTCALL
#define	IOINPCALL	FASTCALL
#define	SOUNDCALL	FASTCALL
#define	VRAMCALL	FASTCALL
#define	SCRNCALL	FASTCALL

#ifdef	DEBUG
#define	INLINE
#define	__ASSERT(s)	assert(s)
#else
#ifndef	__ASSERT
#define	__ASSERT(s)
#endif
#ifndef	INLINE
#define	INLINE		inline
#endif
#endif

#define	SUPPORT_24BPP
#define	SUPPORT_32BPP
#define	SUPPORT_NORMALDISP

#undef	SOUND_CRITICAL
#undef	SOUNDRESERVE
#define	SUPPORT_EXTERNALCHIP

#define	SUPPORT_PC9861K
#define	SUPPORT_HOSTDRV

#define	SUPPORT_RESUME
#define	SUPPORT_STATSAVE

#undef	SUPPORT_SASI
#undef	SUPPORT_SCSI

#define	SUPPORT_S98
#define	SUPPORT_KEYDISP
#define	SUPPORT_SOFTKBD	0

/*
 * You could specify a complete path, e.g. "/etc/timidity.cfg", and
 * then specify the library directory in the configuration file.
 */
extern char timidity_cfgfile_path[MAX_PATH];
#define	TIMIDITY_CFGFILE	timidity_cfgfile_path

#endif	/* NP2_X11_COMPILER_H__ */
