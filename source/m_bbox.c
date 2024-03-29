/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 * $Id: m_bbox.c,v 1.3.2.1 2002/07/20 18:08:36 proff_fs Exp $
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
 *      Main loop menu stuff.
 *      Random number LUT.
 *      Default Config File.
 *      PCX Screenshots.
 *
 *-----------------------------------------------------------------------------*/

static const char
rcsid[] = "$Id: m_bbox.c,v 1.3.2.1 2002/07/20 18:08:36 proff_fs Exp $";


#ifdef __GNUG__
#pragma implementation "m_bbox.h"
#endif
#include "m_bbox.h"

void M_ClearBox (fixed_t *box)
{
  box[BOXTOP] = box[BOXRIGHT] = INT_MIN;
  box[BOXBOTTOM] = box[BOXLEFT] = INT_MAX;
}

void M_AddToBox(fixed_t* box,fixed_t x,fixed_t y)
  {
  if (x<box[BOXLEFT])
    box[BOXLEFT] = x;
  else if (x>box[BOXRIGHT])
    box[BOXRIGHT] = x;
  if (y<box[BOXBOTTOM])
    box[BOXBOTTOM] = y;
  else if (y>box[BOXTOP])
    box[BOXTOP] = y;
  }
