/****************************************************************************
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <https://unlicense.org>
 ***************************************************************************/
/*---------------------------------------------------------------------------
    LZ11 SLI

    Author  : White Guy That Don't Smile
    Date    : 2021/06/19, Saturday, June 19th; 2259 HOURS
    License : UnLicense | Public Domain

    This code builds upon the SLI algorithm by Nintendo with another
    one of their technologies used in the Wii SDK ["Revolution"].
    The technology in question is an extended extension based on the
    Lempel-Ziv family of compression algorithms, called "LZ77 Extended"
    in this particular instance.
    The goal here is to deliver a better compression ratio using the
    SLI algorithm over the one used in the Wii SDK tool "ntcompress.exe".
    However, this has not been tested in an official capacity, and should
    be treated as an experimental tool.
---------------------------------------------------------------------------*/



#ifndef LZ11SLI_H_INCLUDED
#define LZ11SLI_H_INCLUDED



/*------------------------------------------------------
At the expense of RAM, adjusting this macro value
will improve execution time during encoding by reducing
the frequency of re-allocation calls with the work
buffer(s) in use, in addition to, increasing the
possibility of errors concerning integer overflows or
allocation rejection.
The aforementioned macro corresponds to the number of
bits that will be left-shifted by a hard-coded value
as part of an incremental supplement in conjunction
with work RAM buffers used for storing encoded data
representation elements.
[Safe]
  0 =   4096 (Standard)
  1 =   8192
[Risky]
  2 =  16384
  3 =  32768
[Asking for Trouble]
  4 =  65536
  5 = 131072
[Absolute Madman]
  6 = 262144
  7 = 524288
------------------------------------------------------*/
#define ENC_GREED 2



typedef   signed char  i8;
typedef unsigned char  u8;
typedef   signed short i16;
typedef unsigned short u16;
#ifdef __x86_64__
typedef   signed int   i32;
typedef unsigned int   u32;
#else
typedef   signed long  i32;
typedef unsigned long  u32;
#endif



u16 _swap16( u16 data );
u32 _swap32( u32 data );
void _search( u8 *src, u32 mypos, const u32 data_size,
              i32 *match_position, u32 *match_length );
void _encode_sli_lz11( u8 *src, u8 **dst,
                       const u32 srclen, u32 *dstlen );
void _decode_sli_lz11( u8 *src, u8 **dst );



#endif
