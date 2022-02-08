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
/*---------------------------------------------------------------------------
    DISCLAIMER

    This code segment has been both procured and cross-referenced
    through the use of decompilers/disassemblers, and modified/adapted to
    implement the extended length representation capabilities defined by
    << Makoto Takano >> of << Nintendo >>.

    The sampled executable binary, "sliencw11.exe", was reverse-software
    engineered using "Ghidra" and the x86 decompiler, "Snowman".

    "sliencw11.exe" is an encoder for version 1.10 of the SLI format,
    "Yay0", and is a part of the Nintendo 64 SDK.

    "ntcompress.exe" is a tool provided with the Nintendo Wii SDK that
    handles compression and decompression for a variety of formats.

    Additionally, the SLI format is to be regarded as the intellectual
    property of << Nintendo EAD >> and << "Melody-Yoshi" >>.
---------------------------------------------------------------------------*/



#include "lz11sli.h"



static i16 _values[0x100];



static void _initskip( u8 *pat, const i32 patlen )
{
  i32 i;

  i = 0;

  while ( _values[i] = (i16)patlen, ++i < 0x100 );

  i = 0;

  while ( _values[pat[i]] = ((i16)patlen - (i16)i) + -1, ++i < patlen );

  return;
}



static i32 _mischarsearch( u8 *pat,  const i32 patlen,
                           u8 *text, const i32 textlen )
{
  i32 c;
  u32 t;
  i32 j;
  i32 p;

  if ( textlen < patlen )
  {
    return textlen;
  }

  _initskip( pat, patlen );
  p = patlen + -1;

  do
  {
    while ( text[p] != pat[patlen + -1] )
    {
      p += (u32)_values[(u32)text[p]];
    }

    j = patlen + -2;
    c = p;

    while ( 1 )
    {
      p = c + -1;

      if ( j < 0 )
      {
        return c;
      }

      if ( text[p] != pat[j] )
      {
        break;
      }

      j += -1;
      c  = p;
    }

    t = patlen - j;

    if ( (patlen - j) <= (i32)(u32)_values[(u32)text[p]] )
    {
      t = (u32)_values[(u32)text[p]];
    }

    p += t;
  }
  while ( 1 );

}



void _search( u8 *src, u32 mypos, const u32 data_size,
              i32 *match_position, u32 *match_length )
{
  u32 cnt;
  u32 ms;
  i32 posp;
  u32 pos;
  u32 mm;

  posp = 0;
  mm   = 3U;
  pos  = ( mypos < 0x1001U ) ? 0 : (mypos - 0x1000U);
  cnt  = data_size - mypos;

  if ( (0x10110 - 1U) < cnt )
  {
    cnt = 0x10110;
  }

  if ( cnt < 3U )
  {
    *match_length   = 0;
    *match_position = 0;
  }
  else
  {
    while (    (pos < mypos)
            && (ms = _mischarsearch( &src[mypos], mm, &src[pos],
                                     (mypos + mm) - pos ),
                ms < (mypos - pos)) )
    {
      while (    (mm < cnt)
              && (src[pos + mm + ms] == src[mypos + mm]) )
      {
        ++mm;
      }

      if ( cnt == mm )
      {
        *match_position = (pos + ms);
        *match_length   = cnt;
        return;
      }

      posp = (pos + ms);
      ++mm;
      pos += (ms + 1U);
    }

    *match_position = posp;
    *match_length   = ( mm < 4U ) ? 0 : (mm - 1U);
  }

  return;
}
