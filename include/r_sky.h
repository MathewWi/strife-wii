/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 * $Id: r_sky.h,v 1.3.2.1 2002/07/20 18:08:37 proff_fs Exp $
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
 *      Sky rendering.
 *
 *-----------------------------------------------------------------------------*/

#ifndef __R_SKY__
#define __R_SKY__

#include "m_fixed.h"

#ifdef __GNUG__
#pragma interface
#endif

/* SKY, store the number for name. */
#define SKYFLATNAME  "F_SKY001"	//changed to strife's sky - kaiser

/* The sky map is 256*128*4 maps. */
#define ANGLETOSKYSHIFT         22

extern int skytexture;
extern int skytexturemid;

/* Called whenever the view size changes. */
void R_InitSkyMap(void);

#endif
