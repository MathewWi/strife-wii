/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 * $Id: d_items.c,v 1.4.2.1 2002/07/20 18:08:34 proff_fs Exp $
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
 *  Something to do with weapon sprite frames. Don't ask me.
 *
 *-----------------------------------------------------------------------------
 */

static const char
rcsid[] = "$Id: d_items.c,v 1.4.2.1 2002/07/20 18:08:34 proff_fs Exp $";

// We are referring to sprite numbers.
#include "doomtype.h"
#include "info.h"

#ifdef __GNUG__
#pragma implementation "d_items.h"
#endif
#include "d_items.h"


//
// PSPRITE ACTIONS for waepons.
// This struct controls the weapon animations.
//
// Each entry is:
//  ammo/amunition type
//  upstate
//  downstate
//  readystate
//  atkstate, i.e. attack/fire/hit frame
//  flashstate, muzzle flash
//
weaponinfo_t    weaponinfo[NUMWEAPONS] =
{
  {
    // fist
    am_noammo,
		0,
    S_PUNCHUP,
    S_PUNCHDOWN,
    S_PUNCH,
    S_PUNCH1,
    S_NULL
  },
  {
    // xbow
    am_arrows,
		1,
    S_XBOWUP,
    S_XBOWDOWN,
    S_XBOW,
    S_XBOW1,
    S_XBOWFLASH1
  },
  {
    // rifle
    am_clip,
		1,
    S_RIFFLEUP,
    S_RIFFLEDOWN,
    S_RIFFLE,
    S_RIFFLE1,
    S_LIGHTDONE
  },
  {
    // grenade1
    am_grenades1,
		1,
    S_SGRENADEUP,
    S_SGRENADEDOWN,
    S_SGRENADE,
    S_SGRENADE1,
    S_SGRENADEFLASH1
  },
  {
    // flamethrower
    am_cell,
		1,
    S_FLAMEUP,
    S_FLAMEDOWN,
    S_FLAMEREADY1,
    S_FLAME1,
    S_LIGHTDONE
  },
  {
    // xbow2
    am_poisarrows,
		1,
    S_PXBOWUP,
    S_PXBOWDOWN,
    S_PXBOW,
    S_PXBOW1,
    S_LIGHTDONE
  },
  {
    // firegrenade
    am_grenades2,
		1,
    S_FGRENADEUP,
    S_FGRENADEDOWN,
    S_FGRENADE,
    S_FGRENADE1,
    S_FGRENADEFLASH1
  },
  {
    // blaster
    am_cell,
		20,
    S_BLASTERUP,
    S_BLASTERDOWN,
    S_BLASTER,
    S_BLASTER1,
    S_LIGHTDONE
  },
  {
    // minimissile
    am_misl,
		1,
    S_MMISSILEUP,
    S_MMISSILEDOWN,
    S_MMISSILE,
    S_MMISSILE1,
    S_LIGHTDONE
  },

  {
    // blaster 2
    am_cell,
		40,
    S_XBLASTERUP,
    S_XBLASTERDOWN,
    S_XBLASTER,
    S_XBLASTER1,
    S_LIGHTDONE
  },

	{
		//sigil
		am_noammo,
			0,
		S_SIGIL_UP,
		S_SIGIL_DOWN,
		S_SIGIL,
		S_SIGIL_1,
		S_SIGIL_F1
	},
};
