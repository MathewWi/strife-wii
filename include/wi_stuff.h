/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 * $Id: wi_stuff.h,v 1.3.2.1 2002/07/20 18:08:37 proff_fs Exp $
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
 *  Intermission screens.
 *
 *-----------------------------------------------------------------------------*/

#ifndef __WI_STUFF__
#define __WI_STUFF__

//#include "v_video.h"

#include "doomdef.h"

// States for the intermission

typedef enum
{
  NoState = -1,
  StatCount,
  ShowNextLoc

} stateenum_t;

// Called by main loop, animate the intermission.
void WI_Ticker (void);

// Called by main loop,
// draws the intermission directly into the screen buffer.
void WI_Drawer (void);

void WI_initNoState(void);	//kaiser

// Setup for an intermission screen.
void WI_Start(wbstartstruct_t*   wbstartstruct);

// Release intermission screen memory
void WI_End(void);

#endif
