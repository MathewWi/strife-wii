/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 * $Id: m_fixed.h,v 1.7.2.3 2002/07/20 18:08:36 proff_fs Exp $
 *
 *  PrBoom a Doom port merged with LxDoom and LSDLDoom
 *  based on BOOM, a modified and improved DOOM engine
 *  Copyright (C) 1999 by
 *  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
 *  Copyright (C) 1999-2000 by
 *  Jess Haas, Nicolas Kalkhof, Colin Phipps, Florian Schulze
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 *  02111-1307, USA.
 *
 * DESCRIPTION:
 *      Fixed point arithemtics, implementation.
 *
 *-----------------------------------------------------------------------------*/

#ifndef __M_FIXED__
#define __M_FIXED__

#include "config.h"
#include "doomtype.h"

/*
 * Fixed point, 32bit as 16.16.
 */

#define FRACBITS 16
#define FRACUNIT (1<<FRACBITS)

typedef int fixed_t;

/*
 * Absolute Value
 *
 * killough 5/10/98: In djgpp, use inlined assembly for performance
 * killough 9/05/98: better code seems to be gotten from using inlined C
 */

#ifdef _MSC_VER
# ifdef I386_ASM
#pragma warning( disable : 4035 )
__inline static int D_abs(int x)
{
    __asm
    {
        mov eax,x
        cdq
        xor eax,edx
        sub eax,edx
    }
}
# else /* I386_ASM */
inline static const int D_abs(x)
{
  fixed_t _t = (x),_s;
  _s = _t >> (8*sizeof _t-1);
  return (_t^_s)-_s;
}
# endif /* I386_ASM */
#else /* _MSC_VER */
#define D_abs(x) ({fixed_t _t = (x), _s = _t >> (8*sizeof _t-1); (_t^_s)-_s;})
#endif /* _MSC_VER */

/*
 * Fixed Point Multiplication
 */

#ifdef I386_ASM
# ifdef _MSC_VER
#pragma warning( disable : 4035 )
__inline static fixed_t FixedMul(fixed_t a, fixed_t b)
{
//    return (fixed_t)((longlong) a*b >> FRACBITS);
    __asm
    {
        mov  eax,a
        imul b
        shrd eax,edx,16
    }
}
#pragma warning( default : 4035 )
# else /* _MSC_VER */
/* killough 5/10/98: In djgpp, use inlined assembly for performance
 * CPhipps - made __inline__ to inline, as specified in the gcc docs
 * Also made const */
inline
static const fixed_t FixedMul(fixed_t a, fixed_t b)
{
  fixed_t result;
  int dummy;

  asm("  imull %3 ;"
      "  shrdl $16,%1,%0 ;"
      : "=a" (result),          /* eax is always the result */
        "=d" (dummy)    /* cphipps - fix compile problem with gcc-2.95.1
           edx is clobbered, but it might be an input */
      : "0" (a),                /* eax is also first operand */
        "r" (b)                 /* second operand could be mem or reg before,
           but gcc compile problems mean i can only us reg */
      : "%cc"                   /* edx and condition codes clobbered */
      );

  return result;
}
# endif /* _MSC_VER */

#else /* I386_ASM */

/* CPhipps - made __inline__ to inline, as specified in the gcc docs
 * Also made const */

inline static const fixed_t FixedMul(fixed_t a, fixed_t b)
{
  return (fixed_t)((int_64_t) a*b >> FRACBITS);
}

#endif /* I386_ASM */

/*
 * Fixed Point Division
 */

#ifdef I386_ASM

# ifdef _MSC_VER
#pragma warning( disable : 4035 )
__inline static fixed_t FixedDiv(fixed_t a, fixed_t b)
{
    if (D_abs(a) >> 14 >= D_abs(b))
        return (a^b)<0 ? INT_MIN : INT_MAX;
    __asm
    {
        mov  eax,a
        mov  ebx,b
        mov  edx,eax
        shl  eax,16     // proff 11/06/98: Changed from sal to shl, I think
                        // this is better
        sar  edx,16
        idiv ebx        // This is needed, because when I used 'idiv b' the
                        // compiler produced wrong code in a different place
    }
}
#pragma warning( default : 4035 )
# else /* _MSC_VER */
/* killough 5/10/98: In djgpp, use inlined assembly for performance
 * killough 9/5/98: optimized to reduce the number of branches
 * CPhipps - made __inline__ to inline, as specified in the gcc docs
 * Also made const */
inline
static const fixed_t FixedDiv(fixed_t a, fixed_t b)
{
  if (D_abs(a) >> 14 < D_abs(b))
    {
      fixed_t result;
      int dummy;
      asm(" idivl %4 ;"
    : "=a" (result),
      "=d" (dummy)  /* cphipps - fix compile problems with gcc 2.95.1
           edx is clobbered, but also an input */
    : "0" (a<<16),
      "1" (a>>16),
      "r" (b)
    : "%cc"
    );
      return result;
    }
  return ((a^b)>>31) ^ INT_MAX;
}
# endif /* _MSC_VER */

#else /* I386_ASM */
/* CPhipps - made __inline__ to inline, as specified in the gcc docs
 * Also made const */

inline static const fixed_t FixedDiv(fixed_t a, fixed_t b)
{
  return (D_abs(a)>>14) >= D_abs(b) ? ((a^b)>>31) ^ INT_MAX :
    (fixed_t)(((int_64_t) a << FRACBITS) / b);
}

#endif /* I386_ASM */

/* CPhipps -
 * FixedMod - returns a % b, guaranteeing 0<=a<b
 * (notice that the C standard for % does not guarantee this)
 */

inline static const fixed_t FixedMod(fixed_t a, fixed_t b)
{
  if (b & (b-1)) {
    fixed_t r = a % b;
    return ((r<0) ? r+b : r);
  } else
    return (a & (b-1));
}

#endif
