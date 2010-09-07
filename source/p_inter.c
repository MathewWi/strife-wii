/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 * $Id: p_inter.c,v 1.6.2.1 2002/07/20 18:08:36 proff_fs Exp $
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
 *      Handling interactions (i.e., collisions).
 *
 *-----------------------------------------------------------------------------*/

static const char
rcsid[] = "$Id: p_inter.c,v 1.6.2.1 2002/07/20 18:08:36 proff_fs Exp $";

#include "doomstat.h"
#include "dstrings.h"
#include "m_random.h"
#include "am_map.h"
#include "r_main.h"
#include "s_sound.h"
#include "sounds.h"
#include "d_deh.h"  // Ty 03/22/98 - externalized strings
#include "p_tick.h"
#include "lprintf.h"
#include "w_wad.h"
#include "m_misc.h"
#include "p_spec.h"
#include "g_game.h"
#include "m_menu.h"
#include "p_map.h"
#include "d_main.h"	//kaiser
#include "p_dialog.h"

#include "p_inter.h"
#include "p_enemy.h"

#ifdef __GNUG__
#pragma implementation "p_inter.h"
#endif
#include "p_inter.h"

#define BONUSADD        6

// Ty 03/07/98 - add deh externals
// Maximums and such were hardcoded values.  Need to externalize those for
// dehacked support (and future flexibility).  Most var names came from the key
// strings used in dehacked.

int initial_health = 100;
int initial_bullets = 0;
int maxhealth = 100;
int max_armor = 200;
int green_armor_class = 1;  // these are involved with armortype below
int blue_armor_class = 2;
int max_soul = 200;
int soul_health = 100;
int mega_health = 200;
int god_health = 100;   // these are used in cheats (see st_stuff.c)
int idfa_armor = 200;
int idfa_armor_class = 2;
// not actually used due to pairing of cheat_k and cheat_fa
int idkfa_armor = 200;
int idkfa_armor_class = 2;

int bfgcells = 40;      // used in p_pspr.c
int bfgcells2 = 20;      // used in p_pspr.c	//kaiser
// Ty 03/07/98 - end deh externals

// a weapon is found with two clip loads,
// a big item has five clip loads
int maxammo[NUMAMMO]  = {250, 50, 25, 400, 100, 30, 16};
int clipammo[NUMAMMO] = { 10, 4, 10, 20, 4, 6, 4};

//
// GET STUFF
//

//
// P_GiveAmmo
// Num is the number of clip loads,
// not the individual count (0= 1/2 clip).
// Returns false if the ammo can't be picked up at all
//

boolean P_GiveAmmo(player_t *player, ammotype_t ammo, int num)
{
  int oldammo;

  if (ammo == am_noammo)
    return false;

#ifdef RANGECHECK
  if (ammo < 0 || ammo > NUMAMMO)
    I_Error ("P_GiveAmmo: bad type %i", ammo);
#endif

  if ( player->ammo[ammo] == player->maxammo[ammo]  )
    return false;

  if (num)
    num *= clipammo[ammo];
  else
    num = clipammo[ammo]/2;

  // give double ammo in trainer mode, you'll need in nightmare
  if (gameskill == sk_baby || gameskill == sk_nightmare)
    num <<= 1;

  oldammo = player->ammo[ammo];
  player->ammo[ammo] += num;

  if (player->ammo[ammo] > player->maxammo[ammo])
    player->ammo[ammo] = player->maxammo[ammo];

  // If non zero ammo, don't change up weapons, player was lower on purpose.
  if (oldammo)
    return true;

  // We were down to zero, so select a new weapon.
  // Preferences are not user selectable.

  switch (ammo)
    {
    case am_clip:
      if (player->readyweapon == wp_fist || player->readyweapon == wp_rifle) {
        if (player->weaponowned[wp_rifle])
          player->pendingweapon = wp_rifle;
      }
      break;

    case am_arrows:
      if (player->readyweapon == wp_fist || player->readyweapon == wp_crossbow)
        if (player->weaponowned[wp_crossbow])
          player->pendingweapon = wp_crossbow;
        break;

      case am_cell:
        if (player->readyweapon == wp_fist || player->readyweapon == wp_blaster)
          if (player->weaponowned[wp_blaster])
            player->pendingweapon = wp_blaster;
        break;

      case am_misl:
        if (player->readyweapon == wp_fist || player->readyweapon == wp_missile)
          if (player->weaponowned[wp_missile])
            player->pendingweapon = wp_missile;
		  break;

	  case am_poisarrows:
		  if (player->readyweapon == wp_fist || player->readyweapon == wp_crossbow)
		  if (player->weaponowned[wp_crossbow])
		  {
			player->weaponowned[wp_crossbow2] = true;
		  }
		  break;
	  case am_grenades1:
		  if (player->readyweapon == wp_fist || player->readyweapon == wp_grenade)
          if (player->weaponowned[wp_grenade])
            player->pendingweapon = wp_grenade;
		  break;

		case am_grenades2:
		  if (player->readyweapon == wp_fist || player->readyweapon == wp_grenade)
		  if (player->weaponowned[wp_grenade])
		  {
			player->weaponowned[wp_grenade2] = true;
		  }
    default:
      break;
    }
  return true;
}

//
// P_GiveWeapon
// The weapon name may have a MF_DROPPED flag ored in.
//

boolean P_GiveWeapon(player_t *player, weapontype_t weapon, boolean dropped)
{
  boolean gaveammo;
  boolean gaveweapon;

  if (netgame && deathmatch!=2 && !dropped)
    {
      // leave placed weapons forever on net games
      if (player->weaponowned[weapon])
        return false;

      player->bonuscount += BONUSADD;
      player->weaponowned[weapon] = true;

      P_GiveAmmo(player, weaponinfo[weapon].ammo, deathmatch ? 5 : 2);

      player->pendingweapon = weapon;
      S_StartSound (player->mo, sfx_wpnup|PICKUP_SOUND); // killough 4/25/98
      return false;
    }

  if (weaponinfo[weapon].ammo != am_noammo)
    {
      // give one clip with a dropped weapon,
      // two clips with a found weapon
      gaveammo = P_GiveAmmo (player, weaponinfo[weapon].ammo, dropped ? 1 : 2);
    }
  else
    gaveammo = false;

  if (player->weaponowned[weapon])
    gaveweapon = false;
  else
    {
      gaveweapon = true;
      player->weaponowned[weapon] = true;
      player->pendingweapon = weapon;
    }
  return gaveweapon || gaveammo;
}

//
// P_GiveBody
// Returns false if the body isn't needed at all
//

boolean P_GiveBody(player_t *player, int num)
{
  if (player->health >= player->maxhealth)	//kaiser
    return false;
  player->health += num;
  if (player->health > player->maxhealth)
    player->health = player->maxhealth;
  player->mo->health = player->health;
  return true;
}

//
// P_GiveArmor
// Returns false if the armor is worse
// than the current armor.
//

boolean P_GiveArmor(player_t *player, int armortype)
{
  int hits = armortype*100;
  if (player->armorpoints >= hits)
    return false;   // don't pick up
  player->armortype = armortype;
  player->armorpoints = hits;
  return true;
}

//
// P_GiveCard
//

void P_GiveCard(player_t *player, card_t card)
{
  if (player->cards[card])
    return;
  player->bonuscount = BONUSADD;
  player->cards[card] = 1;
}

boolean P_GiveKey(player_t *player, int key, int pic)
{
	if (player->keys & (1 << key))
		return false;

	//player->bonuscount = BONUSADD;
	player->keys |= 1 << key;
	player->inventory.invPic[key] = pic;
	return true;
}

//
// P_GivePower
//
// Rewritten by Lee Killough
//

boolean P_GivePower(player_t *player, int power)
{
  static const int tics[NUMPOWERS] = {
    INVULNTICS, 1 /* strength */, INVISTICS,
    IRONTICS, 1 /* allmap */, INFRATICS,
	1,/*allthings*/TARGETTICS,
   };

  switch (power)
    {
      case pw_invisibility:
        player->mo->flags |= MF_SHADOW;
        break;
      case pw_allmap:
        if (player->powers[pw_allmap])
          return false;
        break;
      case pw_strength:
        P_GiveBody(player,100);
        break;
	  case pw_allthings:
        if (player->powers[pw_allthings])
          return false;
        break;
	  case pw_targeter:
        if (player->powers[pw_targeter])
          return false;
        break;
    }

  // Unless player has infinite duration cheat, set duration (killough)

  if (player->powers[power] >= 0)
    player->powers[power] = tics[power];
  return true;
}

//==========================================================================
//
//	AdjustInvFirst
//
//==========================================================================

void P_AdjustInvFirst(player_t *player)
{
	int FirstIdx;
	int CursIdx;
	int Idx;
	int i;

	if (player->inventory.invFirst > player->inventory.invCursor)
	{
		player->inventory.invFirst = player->inventory.invCursor;
	}
	if (!player->inventory.mobjItem[player->inventory.invCursor])
	{
		return;
	}
	FirstIdx = 0;
	CursIdx = 0;
	Idx = 0;
	for (i = 0; i < NUMMOBJTYPES; i++)
	{
		if (i == player->inventory.invFirst)
			FirstIdx = Idx;
		if (i == player->inventory.invCursor)
			CursIdx = Idx;
		if (player->inventory.mobjItem[i])
			Idx++;
	}
	while (FirstIdx - CursIdx > 5)
	{
		player->inventory.invFirst--;
		FirstIdx--;
		while (!player->inventory.mobjItem[player->inventory.invFirst] &&
			player->inventory.invFirst > 0)
		{
			player->inventory.invFirst--;
		}
	}
	while (FirstIdx - CursIdx < -5)
	{
		player->inventory.invFirst++;
		FirstIdx++;
		while (!player->inventory.mobjItem[player->inventory.invFirst] &&
			player->inventory.invFirst < NUMMOBJTYPES - 1)
		{
			player->inventory.invFirst++;
		}
	}
}

//==========================================================================
//
//	GiveInvItem
//
//==========================================================================

boolean P_GiveInvItem(player_t *player, int item, int amount, int pic)
{
	player->inventory.mobjItem[item] += amount;
	player->inventory.invPic[item] = pic;
	while (!(player->inventory.mobjItem[player->inventory.invCursor] &&
		player->inventory.invCursor < NUMMOBJTYPES - 1))
	{
		player->inventory.invCursor++;
	}
	while (!(player->inventory.mobjItem[player->inventory.invCursor] &&
		player->inventory.invCursor > 0))
	{
		player->inventory.invCursor--;
	}
	P_AdjustInvFirst(player);
	return true;
}

//==========================================================================
//
//	RemoveInvItem
//
//==========================================================================

boolean P_RemoveInvItem(player_t *player, int item, int amount)
{
	if(player->inventory.mobjItem[item] < amount)
	{
		return false;
	}
	player->inventory.mobjItem[item] -= amount;
	while (!player->inventory.mobjItem[player->inventory.invCursor] &&
		player->inventory.invCursor < NUMMOBJTYPES - 1)
	{
		player->inventory.invCursor++;
	}
	while (!player->inventory.mobjItem[player->inventory.invCursor] &&
		player->inventory.invCursor > 0)
	{
		player->inventory.invCursor--;
	}
	P_AdjustInvFirst(player);
	return true;
}

//==========================================================================
//
//	InvRight
//
//==========================================================================

void P_InvRight(player_t *player)
{
	if (player->inventory.invCursor < NUMMOBJTYPES - 1)
	{
		player->inventory.invCursor++;
		while (!player->inventory.mobjItem[player->inventory.invCursor] &&
		player->inventory.invCursor < NUMMOBJTYPES - 1)
		{
			player->inventory.invCursor++;
		}
		while (!player->inventory.mobjItem[player->inventory.invCursor] &&
		player->inventory.invCursor > 0)
		{
			player->inventory.invCursor--;
		}
		P_AdjustInvFirst(player);
	}
}

//==========================================================================
//
//	InvLeft
//
//==========================================================================

void P_InvLeft(player_t *player)
{
	if (player->inventory.invCursor > 0)
	{
		player->inventory.invCursor--;
		while (!player->inventory.mobjItem[player->inventory.invCursor] &&
		player->inventory.invCursor > 0)
		{
			player->inventory.invCursor--;
		}
		while (!player->inventory.mobjItem[player->inventory.invCursor] &&
		player->inventory.invCursor < NUMMOBJTYPES - 1)
		{
			player->inventory.invCursor++;
		}
		P_AdjustInvFirst(player);
	}
}

//==========================================================================
//
//	InvUse
//
//==========================================================================

void P_InvUse(player_t *player)
{
	boolean itemUsed;
	char *itemname;

	if (!player->inventory.mobjItem[player->inventory.invCursor])
	{
		return;
	}
	itemUsed = false;
	switch (player->inventory.invCursor)
	{
	case MT_INV_MED1:
		itemUsed = P_GiveBody(player,10);
		itemname = MC_MEDPATCH;
		break;

	case MT_INV_MED2:
		itemUsed = P_GiveBody(player,25);
		itemname = MC_MEDICALKIT;
		break;

	case MT_INV_MED3:
		itemUsed = P_GiveBody(player,100);
		itemname = MC_SURGERYKIT;
		break;

	case MT_INV_ARMOR1:
		itemUsed = P_GiveArmor(player,1);
		itemname = MC_LEATHERARMOR;
		break;

	case MT_INV_ARMOR2:
		itemUsed = P_GiveArmor(player,2);
		itemname = MC_METALARMOR;
		break;

	case MT_INV_SHADOWARMOR:
		itemUsed = P_GivePower(player,pw_invisibility);
		itemname = MC_SHADOWARMOR;
		break;

	case MT_INV_SUIT:
		if(player->powers[pw_ironfeet])
			return;
		itemUsed = P_GivePower(player,pw_ironfeet);
		itemname = MC_ENVIRONMENTALSUIT;
		break;
	case MT_INV_RADAR:
		if(!player->powers[pw_allmap])
		{
			doom_printf("the scanner won't work without a map!");
			return;
		}
		itemUsed = P_GivePower(player,pw_allthings);
		itemname = MC_SCANNER;
		break;
	case MT_INV_TARGETER:
		itemUsed = P_GivePower(player,pw_targeter);
		itemname = MC_TARGETER;
		break;
	case MT_BEACON:
		itemUsed = true;
		P_TossInventory(player, true);
		itemname = MC_TELEPORTERBEACON;
		break;
	default:
		itemname = "unknown item";
	}
	//	Remove item if it was used.
	if (itemUsed)
	{
		P_RemoveInvItem(player,player->inventory.invCursor, 1);
//		player->hasitem[player->inventory.invCursor] -= 1;
		doom_printf("You used the %s",itemname);
	}
}

void P_TossInventory(player_t *player, boolean special)	//updated 9-3-05
{
	mobjtype_t item;
	mobj_t *mo;
	int an2, amount, i;
	fixed_t distance;
	if (!player->inventory.mobjItem[player->inventory.invCursor])
	{
		return;
	}
	switch (player->inventory.invCursor)
	{
	case MT_INV_GOLD1:
		if(player->inventory.mobjItem[MT_INV_GOLD1] > 50)
		{
			item = MT_INV_GOLD50;
			amount = 50;
		}
		else if(player->inventory.mobjItem[MT_INV_GOLD1] > 25)
		{
			item = MT_INV_GOLD25;
			amount = 25;
		}
		else if(player->inventory.mobjItem[MT_INV_GOLD1] > 10)
		{
			item = MT_INV_GOLD10;
			amount = 10;
		}
		else
		{
		item =	MT_INV_GOLD1;
		amount = 1;
		}
			break;
	default:
	item = player->inventory.invCursor;
	amount = 1;
	break;
	}

	/*in the oringinal strife game, you could actually drop items behind walls! Making
	the item unreachable unless used the noclip cheat. I've added a few things to
	try to make sure this won't happen in svstrife. 80% chance that the item
	won't spawn past a wall.*/
	an2 = player->mo->angle >> ANGLETOFINESHIFT;
	mo = P_SpawnMobj(player->mo->x + FixedMul(player->mo->radius * 4, finecosine[an2]),
		player->mo->y + FixedMul(player->mo->radius * 4, finesine[an2]),
		player->mo->z+16*FRACUNIT, item);
	mo->radius = 20*FRACUNIT;	//to prevent issues comming from smaller items, I've rigged the radius to 20 then change it back when fully spawned - kaiser
	if (mo)
		{
			mo->flags &= ~MF_SOLID;// avoid issues with solid items
			for(i = 3; i < 9; i++)	//prevent spawning items past walls
			{
				if(!P_TryMove (mo, player->mo->x + (mo->x - player->mo->x) * i / 5,
				player->mo->y + (mo->y - player->mo->y) * i / 5, true)
				&& BlockingMobj != mo && !floatok)
				{
					P_RemoveMobj(mo);
					return;
				}
			}
			distance = FixedMul (FRACUNIT + (FRACUNIT / 2), player->mo->radius + 8*FRACUNIT + mo->radius);
			mo->x = player->mo->x + player->mo->momx + FixedMul(distance, finecosine[an2]);
			mo->y = player->mo->y + player->mo->momy + FixedMul(distance, finesine[an2]);
			mo->z = player->mo->z + player->momz + 8*FRACUNIT;
			mo->angle = player->mo->angle;
			mo->momx = player->mo->momx + 5 * finecosine[an2];
			mo->momy = player->mo->momy + 5 * finesine[an2];
			mo->momz = player->mo->momz + FRACUNIT;
			mo->flags |= (MF_DROPPED|MF_SPECIAL);
			if(special == true)
			{
				P_SetMobjState(mo, mo->info->seestate);
			}
			P_RemoveInvItem(player,player->inventory.invCursor, amount);
//			player->hasitem[item] -= amount;
			mo->radius = mo->info->radius;// reset radius to normal
			if(mo->type == MT_DEGNINORE)
				mo->flags |= MF_SOLID;	//make it solid again
		}
}

//
// P_TouchSpecialThing
//
void P_TouchSpecialThing(mobj_t *special, mobj_t *toucher)
{
  player_t *player;
  int      i;
  boolean isakey = false;
  int      sound;
  fixed_t  delta = special->z - toucher->z;
  int lump = 0;
  char lumpname[100];
  char stringname[1024 / 4];
  line_t junk = *lines;

  if (delta > toucher->height || delta < -8*FRACUNIT)
    return;        // out of reach

  sound = sfx_itemup;
  player = toucher->player;

  // Dead thing touching.
  // Can happen with a sliding player corpse.
  if (toucher->health <= 0)
    return;

  if(special->type != MT_INV_GOLD1
	  && (player->inventory.mobjItem[special->type] >= special->info->inv_amount))	//kaiser
  {
	  doom_printf("you cannot hold anymore");
		return;
  }

  //Give keys
  if (special->type >= 133 && special->type < 160)
  {
		isakey = true;
		if(!P_GiveKey(player,special->type - 133 + 1, special->sprite))
		return;
  }

    // Identify by sprite.
  switch (special->sprite)
    {
  case SPR_COMM:
	  player->communicator = true;
	  break;

  case SPR_COIN:
	  if(!P_GiveInvItem(player,MT_INV_GOLD1,1,SPR_COIN))
		  return;
	  break;

  case SPR_CRED:
	  if(!P_GiveInvItem(player,MT_INV_GOLD1,10,SPR_COIN))
		  return;
	  break;

  case SPR_SACK:
	  if(!P_GiveInvItem(player,MT_INV_GOLD1,25,SPR_COIN))
		  return;
	  break;

  case SPR_CHST:
	  if(!P_GiveInvItem(player,MT_INV_GOLD1,50,SPR_COIN))
		  return;
	  break;

    case SPR_PMAP:
      if (!P_GivePower (player, pw_allmap))
        return;
      break;

      // ammo
    case SPR_BLIT:
      if (special->flags & MF_DROPPED)
        {
          if (!P_GiveAmmo (player,am_clip,0))
            return;
        }
      else
        {
          if (!P_GiveAmmo (player,am_clip,1))
            return;
        }
      break;

    case SPR_BBOX:
      if (!P_GiveAmmo (player, am_clip,5))
        return;
      break;

    case SPR_MSSL:
      if (!P_GiveAmmo (player, am_misl,1))
        return;
      break;

    case SPR_ROKT:
      if (!P_GiveAmmo (player, am_misl,5))
        return;
      break;

    case SPR_BRY1:
      if (!P_GiveAmmo (player, am_cell,1))
        return;
      break;

    case SPR_CPAC:
      if (!P_GiveAmmo (player, am_cell,8))
        return;
      break;

    case SPR_XQRL:
      if (!P_GiveAmmo (player, am_arrows,5))
        return;
      break;

    case SPR_PQRL:
      if (!P_GiveAmmo (player, am_poisarrows,1))
        return;
      break;

	  case SPR_GRN1:
      if (!P_GiveAmmo (player, am_grenades1,1))
        return;
      break;

    case SPR_GRN2:
      if (!P_GiveAmmo (player, am_grenades2,1))
        return;
      break;

    case SPR_BKPK:
      if (!player->backpack)
        {
          for (i=0 ; i<NUMAMMO ; i++)
            player->maxammo[i] *= 2;
          player->backpack = true;
        }
      for (i=0 ; i<NUMAMMO ; i++)
        P_GiveAmmo (player, i, 1);
      break;

        // weapons
	//kaiser
    case SPR_GRND:
	  if (!P_GiveWeapon (player, wp_grenade, false) )
        return;
      sound = sfx_wpnup;
      break;

    case SPR_TRPD:
	  if (!P_GiveWeapon (player, wp_blaster, false) )
        return;
	  player->weaponowned[wp_blaster2] = true;
      sound = sfx_wpnup;
      break;

    case SPR_MMSL:
      if (!P_GiveWeapon (player, wp_missile, false) )
        return;
      sound = sfx_wpnup;
      break;

    case SPR_CBOW:
      if (!P_GiveWeapon (player, wp_crossbow, false) )
        return;
      sound = sfx_wpnup;
      break;

    case SPR_RIFL:
      if (!P_GiveWeapon (player, wp_rifle, (special->flags&MF_DROPPED)!=0 ) )
        return;
      sound = sfx_wpnup;
      break;

    case SPR_FLAM:
      if (!P_GiveWeapon(player, wp_flamethrower, (special->flags&MF_DROPPED)!=0))
        return;
      sound = sfx_wpnup;
      break;

	case SPR_HELT:	//giving toughness also gives accuracy, not if dropped. - kaiser
			if(!P_GiveInvItem(player,MT_TOKEN_TOUGHNESS,1,special->sprite))
		  return;
			if(!(special->flags & MF_DROPPED))
			{
			if(!P_GiveInvItem(player,MT_TOKEN_ACCURACY,1,SPR_GUNT))
		  return;
			}
			if(special->silenceditem)
			{
				if(!P_GiveInvItem(player,MT_INV_GOLD1,300,SPR_COIN))
				return;
			}
			break;

	case SPR_TOKN:
		if (special->type >= 312 && special->type <= 342)
		{
			player->quest |= 1 << ((special->type - 312)+1);
		}
		switch(special->type)
		{
		case MT_INV_GOLD300:
		if(!P_GiveInvItem(player,MT_INV_GOLD1,300,SPR_COIN))
		  return;
		  break;
		case MT_TOKEN_ALARM:
			P_NoiseAlert(player->mo,player->mo);
			break;
		case MT_TOKEN_SHOPCLOSE:
			P_NoiseAlert(player->mo,player->mo);
			junk.tag = 222;
			junk.special = 3;
			P_CrossSpecialLine(&junk,0,player->mo);
			break;
		case MT_TOKEN_DOOR1:
			junk.tag = 222;
			junk.special = 2;
			P_CrossSpecialLine(&junk,0,player->mo);
			break;
		case MT_TOKEN_DOOR3:
			junk.tag = 224;
			junk.special = 2;
			P_CrossSpecialLine(&junk,0,player->mo);
			break;
		case MT_TOKEN_PRISON_PASS:
			junk.tag = 223;
			junk.special = 2;
			P_CrossSpecialLine(&junk,0,player->mo);
			if(!P_GiveInvItem(player,MT_TOKEN_PRISON_PASS,1,special->sprite))
			return;
			break;
		case MT_TOKEN_QUEST6:
			S_StartVoice(NULL,"VOC13");
			player->objectives = 13;
			junk.tag = 44;
			junk.special = 36;
			P_CrossSpecialLine(&junk,0,player->mo);
			junk.tag = 225;
			junk.special = 110;
			P_CrossSpecialLine(&junk,0,player->mo);
			break;
		case MT_TOKEN_HEALTH:
			player->health += 50;
			break;
		case MT_TOKEN_AMMO:
			player->ammo[am_clip] += 50;
			break;
		case MT_TOKEN_QUEST7:
			S_StartVoice(player->mo,"VOC14");
			player->objectives = 14;
			break;
		case MT_TOKEN_STAMINA:
			if(player->stamina != 100 && player->maxhealth != 200)
			{
				player->stamina += 10;
				player->maxhealth += 10;
			}
			break;
		case MT_TOKEN_NEW_ACCURACY:
			if(player->accuracy != 100)
			{
				player->accuracy += 10;
			}
			break;
		case MT_SLIDESHOW:
			if(gamemap  <= 3)
				finaletype = maciltalk1;
			else { finaletype = maciltalk2;
					player->quest |= 1 << 17;
			}
				S_StartSound(NULL,sfx_mtalht);
				G_PlayerForceSave();
				D_StartEndPic();
			break;
		}

	case SPR_SIGL:	//kaiser
		switch (special->type)
    {
	case MT_SIGIL_A:
	  player->weaponowned[wp_sigil] = true;
	  player->sigilowned = sl_sigil1;
	  player->pendingweapon = wp_sigil;
	  sound = sfx_siglup;
      break;
	case MT_SIGIL_B:
	  player->weaponowned[wp_sigil] = true;
	  player->sigilowned = sl_sigil2;
	  player->pendingweapon = wp_sigil;
	  sound = sfx_siglup;
	  break;
	case MT_SIGIL_C:
	  player->weaponowned[wp_sigil] = true;
	  player->sigilowned = sl_sigil3;
	  player->pendingweapon = wp_sigil;
	  sound = sfx_siglup;
	  break;
	case MT_SIGIL_D:
	  player->weaponowned[wp_sigil] = true;
	  player->sigilowned = sl_sigil4;
	  player->pendingweapon = wp_sigil;
	  sound = sfx_siglup;
	  break;
	case MT_SIGIL_E:
	  player->weaponowned[wp_sigil] = true;
	  player->sigilowned = sl_sigil5;
	  player->pendingweapon = wp_sigil;
	  sound = sfx_siglup;
	  break;
	}
		break;

    default:
		if(!(special->flags & MF_BOSS))
		{
			if(isakey == false)
			P_GiveInvItem(player,special->type,1,special->sprite);
		}
		sprintf(lumpname, "STRNG%i", special->type);	//see if there's a custom name
		if(W_CheckNumForName(lumpname) == -1)
		{
			if(!special->info->stringname)
			{
				special->info->stringname = "item";
			}
			break;
		}

		lump = W_GetNumForName(lumpname);
		LoadTextLump((lumpname), &stringname[0], 1024);
		special->info->stringname = stringname;
		break;
    }
	if(special->flags & MF_GIVEQUEST)
	{
		player->quest |= 1 << (special->info->speed);
	}

  if (special->flags & MF_COUNTITEM)
    player->itemcount++;
  P_RemoveMobj (special);
  if(special->silenceditem == false)
  {
	if(!(special->flags & MF_BOSS))
	{
		player->bonuscount += BONUSADD;
		doom_printf("Picked Up The %s", special->info->stringname);	//kaiser
		S_StartSound (player->mo, sound | PICKUP_SOUND);   // killough 4/25/98
	}
	else
		if(special->info->stringname)
		  doom_printf(special->info->stringname);	//kaiser
  }
}

//simplified 10-11-05	-kaiser
boolean P_InventoryCheck(player_t *player, mobj_t *item)	//kaiser
{
	int i;
	int weapons[6] = {wp_crossbow,wp_rifle,wp_grenade,wp_flamethrower,wp_blaster,
	wp_missile};
	int mobjWpn[6] = {MT_CROSSBOW, MT_RIFLE, MT_GRENADELAUNCHER, MT_FLAMETHROWER,
	MT_BLASTER, MT_MINIMISSILE};
	int ammos[10] = {am_grenades1, am_grenades2, am_clip, am_clip, am_misl, am_misl,
		am_cell, am_cell, am_poisarrows, am_arrows};
	int mobjAmmo[10] = {MT_AGREN, MT_APGREN, MT_ACLIP, MT_AAMMOBOX, MT_AMINI, MT_AMINIBOX,
	MT_ACELL, MT_APCELL, MT_APAROW, MT_AAROW};
	for(i = 0; i < 6; i++)
	{
		if(player->weaponowned[weapons[i]] && item->type == mobjWpn[i])
			return false;
	}
	for(i = 0; i < 10; i++)
	{
		if(player->ammo[ammos[i]] == player->maxammo[ammos[i]]
			&& item->type == mobjAmmo[i])
			return false;
	}

	if (item->type >= 133 && item->type < 160)
	{
		if(GetKeyType(player->mo,item->type - 133 + 1))
		return false;
	}
	else if(item->type != MT_INV_GOLD1	//coins are infinite, don't limit its amount
		&& (player->inventory.mobjItem[item->type] >= item->info->inv_amount))
	{
		return false;
	}
	else if(item->type == MT_TOKEN_HEALTH && player->health >= 50)//9-04-05
		return false;

	else if(item->type == MT_TOKEN_AMMO && player->ammo[am_clip] >= 50)//9-04-05
		return false;

	return true;	//continue as normal, if something is maxed out get the
					//"You seem to have enough" message - kaiser
}

//
// KillMobj
//
// killough 11/98: make static
static void P_KillMobj(mobj_t *source, mobj_t *target)
{
//	mobj_t* quest;
	mobj_t *mo;

  target->flags &= ~(MF_SHOOTABLE|MF_FLOAT|MF_SKULLFLY);

  if (target->type != MT_TURRET)	//kaiser
    target->flags &= ~MF_NOGRAVITY;

  target->flags |= MF_CORPSE|MF_DROPOFF;
  target->height >>= 2;

  if (source && source->player)
    {
      // count for intermission
      if (target->flags & MF_COUNTKILL)
        source->player->killcount++;
      if (target->player)
        source->player->frags[target->player-players]++;
    }
    else
      if (target->flags & MF_COUNTKILL) { /* Add to kills tally */
  if ((compatibility_level < lxdoom_1_compatibility) || !netgame) {
    if (!netgame)
      // count all monster deaths,
      // even those caused by other monsters
      players[0].killcount++;
  } else
    if (!deathmatch) {
      // try and find a player to give the kill to, otherwise give the
      // kill to a random player.  this fixes the missing monsters bug
      // in coop - rain
      // CPhipps - not a bug as such, but certainly an inconsistency.
      if (target->lastenemy && target->lastenemy->health > 0
    && target->lastenemy->player) // Fighting a player
          target->lastenemy->player->killcount++;
        else {
        // cph - randomely choose a player in the game to be credited
        //  and do it uniformly between the active players
        unsigned int activeplayers = 0, player, i;

        for (player = 0; player<MAXPLAYERS; player++)
    if (playeringame[player])
      activeplayers++;

        if (activeplayers) {
    player = P_Random(pr_friends) % activeplayers;

    for (i=0; i<MAXPLAYERS; i++)
      if (playeringame[i])
        if (!player--)
          players[i].killcount++;
        }
      }
    }
      }

  if (target->player)
    {
      // count environment kills against you
      if (!source)
        target->player->frags[target->player-players]++;

	  P_PlayerOnFire(target->player);
      target->flags &= ~MF_SOLID;
	  if(target->player->burntics == 0)
	  {
		target->player->playerstate = PST_DEAD;
		P_DropWeapon (target->player);
	  }

      if (target->player == &players[consoleplayer] && (automapmode & am_active))
        AM_Stop();    // don't die in auto map; switch view prior to dying
    }

  target->tics -= P_Random(pr_killtics)&3;

  if (target->tics < 1)
    target->tics = 1;

	if(target->type == MT_COUPLING)	//kaiser
	{
		//1-22-06 changed target to source->player->mo
		P_TossUpMobj(target,MT_COUPLING_BROKEN);
		mo = P_SpawnMobj(source->player->mo->x, source->player->mo->y,
			source->player->mo->z, MT_TOKEN_QUEST6);
		mo->silenceditem = true;
		P_TouchSpecialThing(mo, source->player->mo);
	}

  if(target->flags & MSF_OBLIVIATE && (!(target->flags & MF_NOBLOOD)))
		{
			P_SetMobjState(target, S_BURN_1);
			return;
		}
  else if(target->flags & MSF_OBLIVIATE2  && (!(target->flags & MF_NOBLOOD)))
  {
	  P_SetMobjState(target, S_DISR_1);
	  P_DropItem(target);
	  return;
  }

  if (target->health < -target->info->spawnhealth && target->info->xdeathstate)
  {
    P_SetMobjState (target, target->info->xdeathstate);
	target->disable_npc = true;
	P_DropItem(target);
  }
  else
  {
	P_DropItem(target);
    P_SetMobjState (target, target->info->deathstate);
	target->disable_npc = true;
  }
}

void P_LoreMasterChain(mobj_t *source, mobj_t *target)	//kaiser
{
	angle_t angle;
	fixed_t thrust;
	player_t *player;
	int damage;

	if(player = target->player)
	{
		angle = R_PointToAngle2(source->x, source->y, target->x, target->y);
		angle >>= ANGLETOFINESHIFT;
		thrust = 32*FRACUNIT+(S_Random()<<10);
		player->mo->momx -= FixedMul(thrust, finecosine[angle]);
		player->mo->momy -= FixedMul(thrust, finesine[angle]);
		player->mo->momz = FRACUNIT * 16;
		player->jumptics = 24;
		damage = ((S_Random()%5)+1)*source->info->damage;
		P_DamageMobj(target, NULL, NULL, damage);
	}
}

//
// P_DamageMobj
// Damages both enemies and players
// "inflictor" is the thing that caused the damage
//  creature or missile, can be NULL (slime, etc)
// "source" is the thing to target after taking damage
//  creature or NULL
// Source and inflictor are the same for melee attacks.
// Source can be NULL for slime, barrel explosions
// and other environmental stuff.
//

void P_DamageMobj(mobj_t *target,mobj_t *inflictor, mobj_t *source, int damage)
{
  player_t *player;
  boolean justhit;          /* killough 11/98 */

  /* killough 8/31/98: allow bouncers to take damage */
  if (!(target->flags & (MF_SHOOTABLE | MF_BOUNCES)))
    return; // shouldn't happen...

  if (target->health <= 0)
    return;

  if (target->flags & MF_SKULLFLY)
    target->momx = target->momy = target->momz = 0;

  player = target->player;
  if (player && gameskill == sk_baby)
    damage >>= 1;   // take half damage in trainer mode

  if(inflictor)
	{
		switch(inflictor->type)
		{
			case MT_HOOKSHOT:
				P_LoreMasterChain(inflictor,target);	//kaiser
				break;
			case MT_POISARROW:
				if(!(target->flags & MF_NOBLOOD))
				{
					//fixed 9-12-05	//kaiser
					P_CheckForClass(target, CLASS_SHOPGUY) ? 
						P_SetMobjState(target, S_SHOP_10) : 
					(damage = target->info->spawnhealth);
				}
				else return;
				break;
		}
	}

  // Some close combat weapons should not
  // inflict thrust and push the victim out of reach,
  // thus kick away unless using the chainsaw.

  if (inflictor && !(target->flags & MF_NOCLIP) &&
      (!source || !source->player ||
       source->player->readyweapon != wp_blaster))
    {
      unsigned ang = R_PointToAngle2 (inflictor->x, inflictor->y,
                                      target->x,    target->y);

      fixed_t thrust = damage*(FRACUNIT>>3)*100/target->info->mass;

      // make fall forwards sometimes
      if ( damage < 40 && damage > target->health
           && target->z - inflictor->z > 64*FRACUNIT
           && P_Random(pr_damagemobj) & 1)
        {
          ang += ANG180;
          thrust *= 4;
        }

      ang >>= ANGLETOFINESHIFT;
      target->momx += FixedMul (thrust, finecosine[ang]);
      target->momy += FixedMul (thrust, finesine[ang]);

      /* killough 11/98: thrust objects hanging off ledges */
      if (target->intflags & MIF_FALLING && target->gear >= MAXGEAR)
        target->gear = 0;
    }

  // player specific
  if (player)
    {
      // end of game hell hack
      if (target->subsector->sector->special == 11 && damage >= target->health)
        damage = target->health - 1;

      // Below certain threshold,
      // ignore damage in GOD mode, or with INVUL power.
      // killough 3/26/98: make god mode 100% god mode in non-compat mode

      if ((damage < 1000 || (!comp[comp_god] && (player->cheats&CF_GODMODE))) &&
          (player->cheats&CF_GODMODE || player->powers[pw_invulnerability]))
        return;

      if (player->armortype)
        {
          int saved = player->armortype == 1 ? damage/3 : damage/2;
          if (player->armorpoints <= saved)
            {
              // armor is used up
              saved = player->armorpoints;
              player->armortype = 0;
            }
          player->armorpoints -= saved;
          damage -= saved;
        }

      player->health -= damage;       // mirror mobj health here for Dave
      if (player->health < 0)
        player->health = 0;

      player->attacker = source;
      player->damagecount += damage;  // add damage after armor / invuln

      if (player->damagecount > 100)
        player->damagecount = 100;  // teleport stomp does 10k points...
    }

  // do the damage
  target->health -= damage;
  if (target->health <= 0)
    {
	  if(inflictor)		//OMG HE'S ON FIRE!! OH NO!		-kaiser
		{ // Check for flame death
			if((inflictor->flags & MSF_OBLIVIATE
				&& inflictor->flags & MF_MISSILE)
				|| ((inflictor->type == MT_SFIREBALL)
				&& (target->health > -1) && (damage > 1)))
			{
				target->flags |= MSF_OBLIVIATE;
			}
			else if((inflictor->flags & MSF_OBLIVIATE2
				&& inflictor->flags & MF_MISSILE)
				|| ((inflictor->type == MT_TORPEDOSPREAD)
				|| ((inflictor->type == MT_TORPEDO)
				&& !target->type == MT_MISC_06)
				&& (target->health > -1) && (damage > 1)))
			{
				target->flags |= MSF_OBLIVIATE2;
			}
		}
	  if(target->type != MT_RLEADER)	//kaiser
      P_KillMobj (source, target);
      return;
    }

  if (target->health <= 6 && target->info->crashstate)	//Randy
	{
		P_SetMobjState(target,target->info->crashstate);
		target->disable_npc = true;
		return;
	}

  // killough 9/7/98: keep track of targets so that friends can help friends
  if (mbf_features)
    {
      /* If target is a player, set player's target to source,
       * so that a friend can tell who's hurting a player
       */
      if (player)
  P_SetTarget(&target->target, source);

      /* killough 9/8/98:
       * If target's health is less than 50%, move it to the front of its list.
       * This will slightly increase the chances that enemies will choose to
       * "finish it off", but its main purpose is to alert friends of danger.
       */
      if (target->health*2 < target->info->spawnhealth)
  {
    thinker_t *cap = &thinkerclasscap[target->flags & MF_FRIEND ?
             th_friends : th_enemies];
    (target->thinker.cprev->cnext = target->thinker.cnext)->cprev =
      target->thinker.cprev;
    (target->thinker.cnext = cap->cnext)->cprev = &target->thinker;
    (target->thinker.cprev = cap)->cnext = &target->thinker;
  }
    }

  if ((justhit = (P_Random (pr_painchance) < target->info->painchance &&
      !(target->flags & MF_SKULLFLY)))) //killough 11/98: see below
    P_SetMobjState(target, target->info->painstate);

  target->reactiontime = 0;           // we're awake now...

  /* killough 9/9/98: cleaned up, made more consistent: */

  if (source && source != target /*&& source->type != MT_VILE*/ &&
      (!target->threshold /*|| target->type == MT_VILE*/) &&
      ((source->flags ^ target->flags) & MF_FRIEND ||
       monster_infighting ||
       !mbf_features))
    {
      /* if not intent on another player, chase after this one
       *
       * killough 2/15/98: remember last enemy, to prevent
       * sleeping early; 2/21/98: Place priority on players
       * killough 9/9/98: cleaned up, made more consistent:
       */

      if (!target->lastenemy || target->lastenemy->health <= 0 ||
    (!mbf_features ?
     !target->lastenemy->player :
     !((target->flags ^ target->lastenemy->flags) & MF_FRIEND) &&
     target->target != source)) // remember last enemy - killough
  P_SetTarget(&target->lastenemy, target->target);

      P_SetTarget(&target->target, source);       // killough 11/98
      target->threshold = BASETHRESHOLD;
      if (target->state == &states[target->info->spawnstate]
          && target->info->seestate != S_NULL)
        P_SetMobjState (target, target->info->seestate);
    }

  /* killough 11/98: Don't attack a friend, unless hit by that friend. */
  if (justhit && (target->target == source || !target->target ||
      !(target->flags & target->target->flags & MF_FRIEND)))
    target->flags |= MF_JUSTHIT;    // fight back!

  //if the target mobj has the MSF_OBLIVIATE2 enabled and is not dead, then remove the flag
  if(target->flags & MSF_OBLIVIATE2 && target->health > 0)	//1-24-06	-kaiser
	  target->flags &= ~MSF_OBLIVIATE2;
}

void P_FallingDamage(player_t *player)	//kaiser
{
	int damage;
	int mom;
	int dist;

	if(!player)
		return;

	mom = abs(player->mo->momz);	
	dist = FixedMul(mom, 16*FRACUNIT/23);

	if(mom >= 63*FRACUNIT)
	{ // automatic death
		P_DamageMobj(player->mo, NULL, NULL, 10000);
		return;
	}
	damage = ((FixedMul(dist, dist)/10)>>FRACBITS)+32;
	if(player->mo->momz > -39*FRACUNIT && damage > player->mo->health
		&& player->mo->health != 1)
	{ // No-death threshold
		damage = player->mo->health-1;
	}
	S_StartSound(player->mo,sfx_pcrush);
	P_DamageMobj(player->mo, NULL, NULL, damage);
}
