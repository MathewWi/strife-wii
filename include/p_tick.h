/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 * $Id: p_tick.h,v 1.4.2.1 2002/07/20 18:08:37 proff_fs Exp $
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
 *  Core thinker processing prototypes.
 *-----------------------------------------------------------------------------*/

#ifndef __P_TICK__
#define __P_TICK__

#include "d_think.h"

#ifdef __GNUG__
#pragma interface
#endif

/* Called by C_Ticker, can call G_PlayerExited.
 * Carries out all thinking of monsters and players. */

void P_Ticker(void);

extern thinker_t thinkercap;  // Both the head and tail of the thinker list

void P_InitThinkers(void);
void P_AddThinker(thinker_t *thinker);
void P_RemoveThinker(thinker_t *thinker);
void P_RemoveThinkerDelayed(thinker_t *thinker);    // killough 4/25/98

void P_UpdateThinker(thinker_t *thinker);   // killough 8/29/98

void P_SetTarget(mobj_t **mo, mobj_t *target);   // killough 11/98

// killough 8/29/98: threads of thinkers, for more efficient searches
typedef enum {
  th_misc,
  th_friends,
  th_enemies,
  NUMTHCLASS
} th_class;

extern thinker_t thinkerclasscap[];

#endif
