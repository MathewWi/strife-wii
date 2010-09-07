/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 * $Id: p_inter.h,v 1.3.2.1 2002/07/20 18:08:36 proff_fs Exp $
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
 *  Thing events, and dehacked specified numbers controlling them.
 *
 *-----------------------------------------------------------------------------*/

#ifndef __P_INTER__
#define __P_INTER__

#include "d_player.h"
#include "p_mobj.h"

#ifdef __GNUG__
#pragma interface
#endif


/* follow a player exlusively for 3 seconds */
#define BASETHRESHOLD   (100)

boolean P_GivePower(player_t *, int);
void P_TouchSpecialThing(mobj_t *special, mobj_t *toucher);
boolean P_InventoryCheck(player_t *player, mobj_t *item);	//kaiser
//void P_UpdateSigil(mobj_t *special, mobj_t *toucher);	//kaiser
boolean P_GiveKey(player_t *player, int key, int pic);	//kaiser
void P_DamageMobj(mobj_t *target,mobj_t *inflictor,mobj_t *source,int damage);
void P_FallingDamage(player_t *player); //kaiser
void P_InvRight(player_t *player);	//kaiser
void P_InvLeft(player_t *player);	//kaiser
void P_TossInventory(player_t *player, boolean special);	//kaiser
void P_InvUse(player_t *player);	//kaiser
boolean P_RemoveInvItem(player_t *player, int item, int amount);
boolean P_GiveWeapon(player_t *player, weapontype_t weapon, boolean dropped);
boolean P_GiveAmmo(player_t *player, ammotype_t ammo, int num);

/* killough 5/2/98: moved from d_deh.c, g_game.c, m_misc.c, others: */

extern int god_health;   /* Ty 03/09/98 - deh support, see also p_inter.c */
extern int idfa_armor;
extern int idfa_armor_class;
extern int idkfa_armor;
extern int idkfa_armor_class;  /* Ty - end */
/* Ty 03/13/98 - externalized initial settings for respawned player */
extern int initial_health;
extern int initial_bullets;
extern int maxhealth;
extern int max_armor;
extern int green_armor_class;
extern int blue_armor_class;
extern int max_soul;
extern int soul_health;
extern int mega_health;
extern int god_health;
extern int idfa_armor;
extern int idfa_armor_class;
extern int idkfa_armor;
extern int idkfa_armor_class;
extern int bfgcells;
extern int bfgcells2;	//kaiser
extern int maxammo[], clipammo[];

#endif
