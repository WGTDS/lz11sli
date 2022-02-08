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

    The sampled executable binaries, "sliencw11.exe" and "ntcompress.exe",
    were reverse-software engineered using "Ghidra" and the x86 decompiler,
    "Snowman".

    "sliencw11.exe" is an encoder for version 1.10 of the SLI format,
    "Yay0", and is a part of the Nintendo 64 SDK.

    "ntcompress.exe" is a tool provided with the Nintendo Wii SDK that
    handles compression and decompression for a variety of formats.

    Additionally, the SLI format is to be regarded as the intellectual
    property of << Nintendo EAD >> and << "Melody-Yoshi" >>.
---------------------------------------------------------------------------*/



#include "lz11sli.h"
#include <stdio.h>
#include <stdlib.h>



static void _null_sli_lz11_block( u8 *def )
{
  if ( def != (u8 *)0 )
  {
    free( def );
    def = (u8 *)0;
  }

  return;
}



static void _produce_sli_lz11( u8 *src, u8 **dst,
                               const u32 srclen, u32 **dstlen )
{
  u8  *def;
  u32  dp;
  u32  cp;
  u32  match_length;
  i32  lookahead_position;
  u32  lookahead_length;
  u32  mask;
  i32  match_position;
  register u32 srcpos;
  u32  ndp;
  u32  i;
  u32  j;
  u32  ext;
  i8   code;
  u8   cmds;
  u8   bytes[4];

  cp  = 0;
  dp  = cp + 1U;
  ndp = 0x1000U << ENC_GREED;
  def = malloc( (ndp << 2U) );
  srcpos = 0;
  cmds   = 0;
  mask   = 0x80U;

  while ( srcpos < srclen )
  {
    _search( src, srcpos, srclen,
             &match_position, &match_length );

    if ( match_length < 3U )
    {
      cmds |= mask;
      def[dp++] = src[srcpos++];

      if ( ndp == dp )
      {
        ndp += ((0x1000U << 2) << ENC_GREED);
        def  = realloc( def, ndp );

        if ( def == (u8 *)0 )
        {
          goto err;
        }
      }
    }
    else
    {
      _search( src, (srcpos + 1U), srclen,
               &lookahead_position, &lookahead_length );

      if ( (match_length + 1U) < lookahead_length )
      {
        cmds |= mask;
        def[dp++] = src[srcpos++];

        if ( ndp == dp )
        {
          ndp += ((0x1000U << 2) << ENC_GREED);
          def  = realloc( def, ndp );

          if ( def == (u8 *)0 )
          {
            goto err;
          }
        }

        mask >>= 1;

        if ( mask == 0 )
        {
          mask = 0x80U;
          def[cp] = cmds;
          cp = dp++;

          if ( ndp == dp )
          {
            ndp += ((0x1000U << 2) << ENC_GREED);
            def  = realloc( def, ndp );

            if ( def == (u8 *)0 )
            {
              goto err;
            }
          }

          cmds = 0;
        }

        match_length   = lookahead_length;
        match_position = lookahead_position;
      }

      match_position = ((srcpos - match_position) + -1);
      i = 0;
      j = 0;

      if ( match_length < 0x111U )
      {
        if ( match_length < 0x11U )
        {
          code = (i8)match_length + -1;
        }
        else
        {
          code = (i8)(match_length - 0x11U);
          bytes[j] = (u8)((match_length - 0x11U) >> 4);
          j++;
        }
      }
      else
      {
        ext  = match_length - 0x111U;
        code = (i8)ext;
        bytes[j    ] = (u8)(ext >> 0xC) | 0x10;
        bytes[j + 1] = (u8)(ext >> 0x4);
        j += 2;
      }

      bytes[j    ] = (u8)(match_position >> 8) | code << 4;
      bytes[j + 1] = (i8)match_position;
      j += 2;

      while ( i < j )
      {
        def[dp++] = bytes[i++];

        if ( ndp == dp )
        {
          ndp += ((0x1000U << 2) << ENC_GREED);
          def  = realloc( def, ndp );

          if ( def == (u8 *)0 )
          {
            goto err;
          }
        }
      }

      srcpos += match_length;
    }

    mask >>= 1;

    if ( mask == 0 )
    {
      mask = 0x80U;
      def[cp] = cmds;
      cp = dp++;

      if ( ndp == dp )
      {
        ndp += ((0x1000U << 2) << ENC_GREED);
        def  = realloc( def, ndp );

        if ( def == (u8 *)0 )
        {
          goto err;
        }
      }

      cmds = 0;
    }
  }

  if ( mask != 0x80U )
  {
    def[cp] = cmds;
  }
  else
  {
    --dp;
  }

  **dstlen = (u32)(dp + 4U);
  *dst     = (u8 *)calloc(**dstlen, sizeof(u8));

  if ( *dst == (u8 *)0 )
  {
    goto err;
  }

  *((u32 *)*dst) = _swap32( (u32)(0x11000000 | (srclen & 0x00FFFFFF)) );

  for ( srcpos = 0; srcpos < dp; srcpos++ )
  {
    *(*dst + 4U + srcpos) = def[srcpos];
  }

  _null_sli_lz11_block( def );
  return;

err:

  printf( "RAM Unavailable!\n" );

  if ( *dst != (u8 *)0 )
  {
    free( *dst );
  }

  _null_sli_lz11_block( def );
  *dst = (u8 *)0;
  return;
}



void _encode_sli_lz11( u8 *src, u8 **dst,
                       const u32 srclen, u32 *dstlen )
{
  _produce_sli_lz11( src, &*dst, srclen, &dstlen );
  return;
}
