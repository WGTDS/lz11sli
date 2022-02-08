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

    This code segment has been transcribed into C from the Assembler
    source, "slidec.s", and modified/adapted to implement the extended
    length representation capabilities defined by << Makoto Takano >>
    of << Nintendo >>.

    "slidec.s" is Assembly code for decoding version 1.10 of the
    SLI format, "Yay0", and is a part of the Nintendo 64 SDK.

    "ntcompress.exe" is a tool provided with the Nintendo Wii SDK that
    handles compression and decompression for a variety of formats.

    Additionally, the SLI format is to be regarded as the intellectual
    property of << Nintendo EAD >> and << "Melody-Yoshi" >>.
---------------------------------------------------------------------------*/



#include "lz11sli.h"
#include <stdio.h>
#include <stdlib.h>



static void _process_sli_lz11( u8 *src, u8 **dst )
{
  u32 declen;
  u32 dsp;
  u32 qty;
  u32 flags;
  u8 *dest;
  u8 *prev;
  u8  byte;
  u32 code;
  i32 sign;

  declen = _swap32( *(u32 *)src );

  if ( ((declen >> 24) != 0x11) || ((declen & 0x00FFFFFF) == 0U) )
  {
    goto err;
  }

  declen &= 0x00FFFFFF;
  dest = (*dst + declen);
  flags = 0;
  sign  = 0;
  src  += 4U;

  do
  {
    if ( flags == 0 )
    {
      sign   = (i32)*src;
      sign <<= 24;
      flags  = 8U;
      src++;
    }
    else
    {
      if ( sign < 0 )
      {
        *(*dst)++ = *src++;
      }
      else
      {
        byte = *src;
        code = (u32)byte >> 4;

        if ( code == 1 )
        {
          byte = *(src + 2);
          qty  = ((u32)(byte >> 4) |
                  (((u32)*src & 0xF) << 0xC) |
                  (u32)*(src + 1) << 4) + 0x111;
          src += 2;
        }
        else
        {
          if ( code == 0 )
          {
            byte = *(src + 1);
            qty  = ((u32)(byte >> 4) | ((u32)*src & 0xF) << 4) + 0x11;
            src++;
          }
          else
          {
            qty = code + 1;
          }
        }

        dsp  = ((u32)((u16)*(src + 1) | ((u16)byte & 0xF) << 8) + 1);
        prev = *dst - dsp;
        src += 2;

        while ( *(*dst)++ = *prev++, --qty );
      }

      sign <<= 1;
      flags--;
    }
  }
  while ( *dst < dest );

  *dst -= declen;
  return;

err:

  free( *dst );
  printf( "Bad Header!\n"
          "Magic  == 0x11?     --> 0x%2X\n"
          "Length != 0x000000? --> 0x%6X\n",
          (_swap32( *(u32 *)(src        ) ) >> 24),
          (_swap32( *(u32 *)(src        ) )  & 0x00FFFFFF) );
  *dst = (u8 *)0;
  return;
}



void _decode_sli_lz11( u8 *src, u8 **dst )
{
  _process_sli_lz11( src, &*dst );
  return;
}
