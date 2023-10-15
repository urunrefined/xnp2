#ifndef NP2_X11_COMPILER_H__
#define NP2_X11_COMPILER_H__

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

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define BYTESEX_BIG
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define BYTESEX_LITTLE
#else
#error Please set preprocessor macro __BYTE_ORDER__ to __ORDER_BIG_ENDIAN__ or __ORDER_LITTLE_ENDIAN__
#endif

#define CPUCORE_IA32

#include <stdint.h>

typedef int32_t SINT;
typedef uint32_t UINT;

typedef int8_t SINT8;
typedef int16_t SINT16;
typedef int32_t SINT32;
typedef int64_t SINT64;

typedef uint8_t UINT8;
typedef uint16_t UINT16;
typedef uint32_t UINT32;
typedef uint64_t UINT64;

typedef int BOOL;

typedef signed long nxp2ptr;
#define INTPTR nxp2ptr

#define PTR_TO_UINT32(p) ((unsigned int)(unsigned long)(p))
#define UINT32_TO_PTR(v) ((void *)(unsigned long)((UINT32)(v)))

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif

// TODO: Sort this out later
#define MAX_PATH 4096

// TODO: Remove this and the ZeroMemory / FillMemory macros
#include <string.h>

#define ZeroMemory(d, n) memset((d), 0, (n))
#define FillMemory(a, b, c) memset((a), (c), (b))

#ifndef roundup
#define roundup(x, y) ((((x) + ((y)-1)) / (y)) * (y))
#endif

#ifndef NELEMENTS
#define NELEMENTS(a) ((int)(sizeof(a) / sizeof(a[0])))
#endif

#define NP2_CPU_ARCH_AMD64
#define NP2_CPU_ARCH_IA32

#define GCC_CPU_ARCH_IA32
#define GCC_CPU_ARCH_AMD64

#define GETRAND() random()
#define SPRINTF sprintf

#define OEMCHAR char
#define OEMTEXT(s) s

#define USE_FPU
#define SUPPORT_PC9821
#define SUPPORT_CRT31KHZ
#define SUPPORT_IDEIO
#define SUPPORT_CRT15KHZ

#define LOADINTELDWORD(a) (*((const UINT32 *)(a)))
#define LOADINTELWORD(a) (*((const UINT16 *)(a)))
#define STOREINTELDWORD(a, b) *(UINT32 *)(a) = (b)
#define STOREINTELWORD(a, b) *(UINT16 *)(a) = (b)

#ifndef FASTCALL
#define FASTCALL
#endif
#define CPUCALL FASTCALL
#define MEMCALL FASTCALL
#define DMACCALL FASTCALL
#define IOOUTCALL FASTCALL
#define IOINPCALL FASTCALL
#define SOUNDCALL FASTCALL
#define VRAMCALL FASTCALL
#define SCRNCALL FASTCALL

#ifdef DEBUG
#define INLINE
#define __ASSERT(s) assert(s)
#else
#ifndef __ASSERT
#define __ASSERT(s)
#endif
#ifndef INLINE
#define INLINE inline
#endif
#endif

#ifndef __cplusplus
#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif
#endif /* !__cplusplus */

#define SUPPORT_24BPP
#define SUPPORT_32BPP
#define SUPPORT_NORMALDISP

#define SUPPORT_EXTERNALCHIP
#define SUPPORT_PC9861K
#define SUPPORT_HOSTDRV
#define SUPPORT_STATSAVE
#define SUPPORT_S98

#undef SUPPORT_SASI
#undef SUPPORT_SCSI

#ifdef DEBUG
#define TRACE
#define TRACEVERBOSE
#endif

#endif /* NP2_X11_COMPILER_H__ */
