/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 * $Id: p_pspr.c,v 1.5.2.1 2002/07/20 18:08:37 proff_fs Exp $
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
 *      Weapon sprite animation, weapon objects.
 *      Action functions for weapons.
 *
 *-----------------------------------------------------------------------------*/

static const char
rcsid[] = "$Id: p_pspr.c,v 1.5.2.1 2002/07/20 18:08:37 proff_fs Exp $";

#include "doomstat.h"
#include "r_main.h"
#include "p_map.h"
#include "p_inter.h"
#include "p_pspr.h"
#include "p_enemy.h"
#include "m_random.h"
#include "s_sound.h"
#include "sounds.h"
#include "d_event.h"

#define LOWERSPEED   (FRACUNIT*6)
#define RAISESPEED   (FRACUNIT*6)
#define WEAPONBOTTOM (FRACUNIT*128)
#define WEAPONTOP    (FRACUNIT*32)

#define BFGCELLS bfgcells        /* Ty 03/09/98 externalized in p_inter.c */
#define BFGCELLS2 bfgcells2        /* Ty 03/09/98 externalized in p_inter.c */

extern void P_Thrust(player_t *, angle_t, fixed_t);	//kaiser

// The following array holds the recoil values         // phares

static const int recoil_values[] = {    // phares
  10, // wp_fist
  10, // wp_crossbow
  30, // wp_rifle
  10, // wp_grenade
  100,// wp_missile
  20, // wp_crossbow2
  100,// wp_grenade2
  0,  // wp_blaster
  80,  // wp_missile
  80	//wp_sigil
};

//
// P_SetPsprite
//

static void P_SetPsprite(player_t *player, int position, statenum_t stnum)
{
  pspdef_t *psp = &player->psprites[position];

  do
    {
      state_t *state;

      if (!stnum)
        {
          // object removed itself
          psp->state = NULL;
          break;
        }

      state = &states[stnum];
      psp->state = state;
      psp->tics = state->tics;        // could be 0

      if (state->misc1)
        {
          // coordinate set
          psp->sx = state->misc1 << FRACBITS;
          psp->sy = state->misc2 << FRACBITS;
        }

      // Call action routine.
      // Modified handling.
      if (state->action)
        {
          state->action(player, psp);
          if (!psp->state)
            break;
        }
      stnum = psp->state->nextstate;
    }
  while (!psp->tics);     // an initial state of 0 could cycle through
}

//
// P_BringUpWeapon
// Starts bringing the pending weapon up
// from the bottom of the screen.
// Uses player
//

static void P_BringUpWeapon(player_t *player)
{
  statenum_t newstate;

  if (player->pendingweapon == wp_nochange)
    player->pendingweapon = player->readyweapon;

  /*if (player->pendingweapon == wp_blaster)
    S_StartSound (player->mo, sfx_sawup);*/

  newstate = weaponinfo[player->pendingweapon].upstate;

  player->pendingweapon = wp_nochange;
  // killough 12/98: prevent pistol from starting visibly at bottom of screen:
  player->psprites[ps_weapon].sy =
    mbf_features ? WEAPONBOTTOM+FRACUNIT*2 : WEAPONBOTTOM;

  P_SetPsprite(player, ps_weapon, newstate);
}

// The first set is where the weapon preferences from             // killough,
// default.cfg are stored. These values represent the keys used   // phares
// in DOOM2 to bring up the weapon, i.e. 6 = plasma gun. These    //    |
// are NOT the wp_* constants.                                    //    V

int weapon_preferences[2][NUMWEAPONS+1] = {
  {6, 9, 4, 3, 2, 8, 5, 7, 1, 0},  // !compatibility preferences
  {6, 9, 4, 3, 2, 8, 5, 7, 1, 0},  //  compatibility preferences
};

// P_SwitchWeapon checks current ammo levels and gives you the
// most preferred weapon with ammo. It will not pick the currently
// raised weapon. When called from P_CheckAmmo this won't matter,
// because the raised weapon has no ammo anyway. When called from
// G_BuildTiccmd you want to toggle to a different weapon regardless.

int P_SwitchWeapon(player_t *player)
{
  int *prefer = weapon_preferences[demo_compatibility!=0]; // killough 3/22/98
  int currentweapon = player->readyweapon;
  int newweapon = currentweapon;
  int i = NUMWEAPONS+1;   // killough 5/2/98

  // killough 2/8/98: follow preferences and fix BFG/SSG bugs

  do
    switch (*prefer++)
      {
      case 1:
        if (!player->powers[pw_strength])      // allow chainsaw override
          break;
      case 0:
        newweapon = wp_fist;
        break;
      case 2:
        if (player->weaponowned[wp_crossbow] && player->ammo[am_arrows])
          newweapon = wp_crossbow;
        break;
      case 3:
        if (player->weaponowned[wp_rifle] && player->ammo[am_clip])
          newweapon = wp_rifle;
        break;
      case 4:
        if (player->weaponowned[wp_grenade] && player->ammo[am_grenades1])
          newweapon = wp_grenade;
        break;
      case 9:
        if (player->weaponowned[wp_missile] && player->ammo[am_misl])
          newweapon = wp_missile;
        break;
      case 6:
        if (player->weaponowned[wp_crossbow2] && player->ammo[am_poisarrows])
          newweapon = wp_crossbow2;
        break;
      case 7:
        if (player->weaponowned[wp_grenade2] && player->ammo[am_grenades2])
          newweapon = wp_grenade2;
        break;
      case 5:
        if (player->weaponowned[wp_flamethrower] && player->ammo[am_cell])
          newweapon = wp_flamethrower;
		break;
      case 8:
        if (player->weaponowned[wp_blaster] && player->ammo[am_cell])
          newweapon = wp_blaster;
        break;
	  case 10:	//kaiser
        if (player->weaponowned[wp_blaster2] && player->ammo[am_cell])
          newweapon = wp_blaster2;
        break;
      }
  while (newweapon==currentweapon && --i);          // killough 5/2/98
  return newweapon;
}

// killough 5/2/98: whether consoleplayer prefers weapon w1 over weapon w2.
int P_WeaponPreferred(int w1, int w2)
{
  return
    (weapon_preferences[0][0] != ++w2 && (weapon_preferences[0][0] == ++w1 ||
    (weapon_preferences[0][1] !=   w2 && (weapon_preferences[0][1] ==   w1 ||
    (weapon_preferences[0][2] !=   w2 && (weapon_preferences[0][2] ==   w1 ||
    (weapon_preferences[0][3] !=   w2 && (weapon_preferences[0][3] ==   w1 ||
    (weapon_preferences[0][4] !=   w2 && (weapon_preferences[0][4] ==   w1 ||
    (weapon_preferences[0][5] !=   w2 && (weapon_preferences[0][5] ==   w1 ||
    (weapon_preferences[0][6] !=   w2 && (weapon_preferences[0][6] ==   w1 ||
    (weapon_preferences[0][7] !=   w2 && (weapon_preferences[0][7] ==   w1
   ))))))))))))))));
}

//
// P_CheckAmmo
// Returns true if there is enough ammo to shoot.
// If not, selects the next weapon to use.
// (only in demo_compatibility mode -- killough 3/22/98)
//

boolean P_CheckAmmo(player_t *player)
{
  ammotype_t ammo = weaponinfo[player->readyweapon].ammo;
  int count = 1;  // Regular

  //if (player->readyweapon == wp_grenade2)  // Minimal amount for one shot varies.
    //count = BFGCELLS;
 // else
  //  if (player->readyweapon == wp_missile)        // Double barrel.
   //   count = 2;

  // Some do not need ammunition anyway.
  // Return if current ammunition sufficient.

  if (ammo == am_noammo || player->ammo[ammo] >= count)
    return true;

  // Out of ammo, pick a weapon to change to.
  //
  // killough 3/22/98: for old demos we do the switch here and now;
  // for Boom games we cannot do this, and have different player
  // preferences across demos or networks, so we have to use the
  // G_BuildTiccmd() interface instead of making the switch here.

  if (demo_compatibility)
    {
      player->pendingweapon = P_SwitchWeapon(player);      // phares
      // Now set appropriate weapon overlay.
      P_SetPsprite(player,ps_weapon,weaponinfo[player->readyweapon].downstate);
    }

  return false;
}

//
// P_FireWeapon.
//

int lastshottic; // killough 3/22/98

static void P_FireWeapon(player_t *player)
{
  statenum_t newstate;

  if (!P_CheckAmmo(player))
    return;

  P_SetMobjState(player->mo, S_PLAY_ATK1);
  newstate = weaponinfo[player->readyweapon].atkstate;
  P_SetPsprite(player, ps_weapon, newstate);
  //P_NoiseAlert(player->mo, player->mo);
  lastshottic = gametic;                       // killough 3/22/98
}

//
// P_DropWeapon
// Player died, so put the weapon away.
//

void P_DropWeapon(player_t *player)
{
  P_SetPsprite(player, ps_weapon, weaponinfo[player->readyweapon].downstate);
}

void P_PlayerOnFire(player_t *player)
{
	if(player->mo->flags & MSF_OBLIVIATE)
	  {
		  P_SetPsprite (player, ps_weapon, S_WAVE1);
		  P_SetPsprite (player, ps_flash, S_NULL);
		  player->burntics = 150;
		  }
	else
		P_SetPsprite (player, ps_flash, S_NULL);
}

//
// A_WeaponReady
// The player can fire the weapon
// or change to another weapon at this time.
// Follows after getting weapon up,
// or after previous attack/fire sequence.
//

void A_WeaponReady(player_t *player, pspdef_t *psp)
{
  // get out of attack state
  if (player->mo->state == &states[S_PLAY_ATK1]
      || player->mo->state == &states[S_PLAY_ATK2] )
    P_SetMobjState(player->mo, S_PLAY);

  if (player->readyweapon == wp_flamethrower && psp->state == &states[S_FLAMEREADY1])
    S_StartSound(player->mo, sfx_flidl);		//kaiser

  // check for change
  //  if player is dead, put the weapon away

  if (player->pendingweapon != wp_nochange || !player->health)
    {
      // change weapon (pending weapon should already be validated)
      statenum_t newstate = weaponinfo[player->readyweapon].downstate;
      P_SetPsprite(player, ps_weapon, newstate);
      return;
    }

  // check for fire
  //  the missile launcher and bfg do not auto fire

  if (player->cmd.buttons & BT_ATTACK)
    {
      if (!player->attackdown || (player->readyweapon != wp_missile &&
                                  player->readyweapon != wp_grenade2))
        {
          player->attackdown = true;
          P_FireWeapon(player);
          return;
        }
    }
  else
    player->attackdown = false;

  // bob the weapon based on movement speed
  {
    int angle = (128*leveltime) & FINEMASK;
    psp->sx = FRACUNIT + FixedMul(player->bob, finecosine[angle]);
    angle &= FINEANGLES/2-1;
    psp->sy = WEAPONTOP + FixedMul(player->bob, finesine[angle]);
  }
}

//
// A_ReFire
// The player can re-fire the weapon
// without lowering it entirely.
//

void A_ReFire(player_t *player, pspdef_t *psp)
{
  // check for fire
  //  (if a weaponchange is pending, let it go through instead)

  if ( (player->cmd.buttons & BT_ATTACK)
       && player->pendingweapon == wp_nochange && player->health)
    {
      player->refire++;
      P_FireWeapon(player);
    }
  else
    {
      player->refire = 0;
      P_CheckAmmo(player);
    }
}

void A_CheckReload(player_t *player, pspdef_t *psp)
{
  P_CheckAmmo(player);
  if(player->readyweapon == wp_crossbow)
	  P_SetPsprite (player, ps_flash, S_XBOWFLASH1);	//kaiser
}

//
// A_Lower
// Lowers current weapon,
//  and changes weapon at bottom.
//

void A_Lower(player_t *player, pspdef_t *psp)
{
  psp->sy += LOWERSPEED;

  // Is already down.
  if (psp->sy < WEAPONBOTTOM)
    return;

  // Player is dead.
  if (player->playerstate == PST_DEAD)
    {
      psp->sy = WEAPONBOTTOM;
      return;      // don't bring weapon back up
    }

  // The old weapon has been lowered off the screen,
  // so change the weapon and start raising it

  if (!player->health)
    {      // Player is dead, so keep the weapon off screen.
      P_SetPsprite(player,  ps_weapon, S_NULL);
      return;
    }

  player->readyweapon = player->pendingweapon;

  P_BringUpWeapon(player);
  P_SetPsprite(player,  ps_flash, S_NULL);	//kaiser
}

//updated 10-13-05
void P_DrawWhichSigil(player_t *player, pspdef_t *psp)	//kaiser
{
	int i;
	int sigils[4] = {sl_sigil2,sl_sigil3,sl_sigil4,sl_sigil5};
	int sigilWpn[4] = {S_SIGIL_B,S_SIGIL_C,S_SIGIL_D,S_SIGIL_E};
	P_SetPsprite (player, ps_flash, S_NULL);
	for(i = 0; i < 4; i++)
	{
		if(player->readyweapon == wp_sigil && player->sigilowned == sigils[i])
		{
			P_SetPsprite(player, ps_flash, sigilWpn[i]);
		}
	}
}

//
// A_Raise
//

void A_Raise(player_t *player, pspdef_t *psp)
{
  statenum_t newstate;

  P_DrawWhichSigil(player,psp);
  if(player->readyweapon == wp_crossbow)
	   P_SetPsprite (player, ps_flash, S_XBOWFLASH1);

  psp->sy -= RAISESPEED;

  if (psp->sy > WEAPONTOP)
    return;

  psp->sy = WEAPONTOP;

  // The weapon has been raised all the way,
  //  so change to the ready state.

  newstate = weaponinfo[player->readyweapon].readystate;

  P_SetPsprite(player, ps_weapon, newstate);
}


// Weapons now recoil, amount depending on the weapon.              // phares
//                                                                  //   |
// The P_SetPsprite call in each of the weapon firing routines      //   V
// was moved here so the recoil could be synched with the
// muzzle flash, rather than the pressing of the trigger.
// The BFG delay caused this to be necessary.

static void A_FireSomething(player_t* player,int adder)
{
  P_SetPsprite(player, ps_flash,
               weaponinfo[player->readyweapon].flashstate+adder);

  // killough 3/27/98: prevent recoil in no-clipping mode
  if (!(player->mo->flags & MF_NOCLIP))
    if (!compatibility && weapon_recoil)
      P_Thrust(player,
               ANG180+player->mo->angle,                          //   ^
               2048*recoil_values[player->readyweapon]);          //   |
}                                                                 // phares

//
// A_GunFlash
//

void A_GunFlash(player_t *player, pspdef_t *psp)
{
  P_SetMobjState(player->mo, S_PLAY_ATK2);

  A_FireSomething(player,0);                                      // phares
}

//
// WEAPON ATTACKS
//

//Had to changed some stuff around to get it working -kaiser
void P_DaggerAlert(mobj_t * pmo, mobj_t * linetarget)
{
	sector_t * sec;
	mobj_t *mo;
	thinker_t *think;
    //mobj_t * link;

	sec = linetarget->subsector->sector;
	if(linetarget->flags & MF_NOALERT)
		return;

	if(!sec->soundtarget && linetarget->health>0)
	{
		P_SetMobjState(linetarget,linetarget->info->painstate);
		linetarget->target=pmo;
		 for(think = thinkercap.next; think != &thinkercap;
		think = think->next)
	{
		if(think->function != P_MobjThinker)
		{ // Not a mobj thinker
			continue;
		}
		mo = (mobj_t *)think;
		if((mo->health > 0) && mo->subsector->sector == sec &&
			P_CheckForClass(mo,CLASS_ACOLYTE)
			&& P_LookForTargets(mo,false) && 
			(P_CheckSight(mo,pmo) || P_CheckSight(mo,linetarget))
			&& mo != linetarget && mo!= pmo)
		{
			mo->target=pmo;
			P_SetMobjState(mo, mo->info->seestate);
			mo->flags|=MF_NOALERT;
		}
	}
    }
}


//
// A_Punch
//

void A_Punch(player_t *player, pspdef_t *psp)
{
  angle_t angle;
  int t, slope, damage;
  fixed_t somestat;
  fixed_t thrust;

  somestat = FRACUNIT;
  somestat = (somestat >> FRACBITS) / 10;

  //hmm..
  damage = (P_Random(pr_punch) & (somestat + 7)) * (somestat + 2);

  if (player->powers[pw_strength])
    damage *= 10;

  if (player->stamina != 0)
	  damage *= (player->stamina / 10);

  angle = player->mo->angle;

  // killough 5/5/98: remove dependence on order of evaluation:
  t = P_Random(pr_punchangle);
  angle += (t - P_Random(pr_punchangle))<<18;

  PuffType = MT_STRIFEPUFF2;	//kaiser
  /* killough 8/2/98: make autoaiming prefer enemies */
  if (!mbf_features ||
      (slope = P_AimLineAttack(player->mo, angle, MELEERANGE, MF_FRIEND),
       !linetarget))
    //slope = P_AimLineAttack(player->mo, angle, MELEERANGE, 0);
	slope = ((player->lookdir)<<FRACBITS)/256;

  P_LineAttack(player->mo, angle, MELEERANGE, slope, damage);

  if (!linetarget)
  {
	  S_StartSound(player->mo, sfx_swish);	//swish sound if miss - kaiser
  }
  else if(linetarget->flags & MF_COUNTKILL)
  {
	  //updated 9-10-05  kaiser
		S_StartSound(player->mo, sfx_meatht);
		P_DaggerAlert(player->mo,linetarget);
		angle = player->mo->angle;
		angle >>= ANGLETOFINESHIFT;
		thrust = FRACUNIT+(S_Random()<<10);
		player->mo->momx += FixedMul(thrust, finecosine[angle]);
		player->mo->momy += FixedMul(thrust, finesine[angle]);
		player->mo->angle = R_PointToAngle2(player->mo->x, player->mo->y,
        linetarget->x, linetarget->y);
	}
	else 
		S_StartSound(player->mo, sfx_mtalht);	
    return;
}

//
// A_FireMissile
//

void A_FireMissile(player_t *player, pspdef_t *psp)	//kaiser
{
  mobj_t *missile;
  int accuracy = player->accuracy / 17;

    player->ammo[weaponinfo[player->readyweapon].ammo]--;
	psp->sx = ((S_Random()&3)-2)*FRACUNIT;
	missile = P_SPMAngle(player->mo, MT_MINIMISSLE, player->mo->angle
		+(((S_Random()&7)-4)<<(24 - accuracy)));
	P_NoiseAlert(player->mo, player->mo);
}

//
// P_BulletSlope
// Sets a slope so a near miss is at aproximately
// the height of the intended target
//

static fixed_t bulletslope;

static void P_BulletSlope(mobj_t *mo)
{
  angle_t an = mo->angle;    // see which target is to be aimed at

  /* killough 8/2/98: make autoaiming prefer enemies */
  uint_64_t mask = mbf_features ? MF_FRIEND : 0;

  do
    {
      bulletslope = P_AimLineAttack(mo, an, 16*64*FRACUNIT, mask);
      if (!linetarget)
  bulletslope = P_AimLineAttack(mo, an += 1<<26, 16*64*FRACUNIT, mask);
      if (!linetarget)
  bulletslope = P_AimLineAttack(mo, an -= 2<<26, 16*64*FRACUNIT, mask);
	  if (!linetarget)
		{
			an += 2<<26;
			bulletslope = (mo->player->lookdir<<FRACBITS)/256;
		}
    }
  while (mask && (mask=0, !linetarget));  /* killough 8/2/98 */
}

//
// P_GunShot
//

void P_GunShot(mobj_t *mo, boolean accurate)
{
  int damage = 5*(P_Random(pr_gunshot)%3+1);
  angle_t angle = mo->angle;

  if (!accurate)
    {  // killough 5/5/98: remove dependence on order of evaluation:
      int t = P_Random(pr_misfire);
      angle += (t - P_Random(pr_misfire))<<18;
    }

  P_LineAttack(mo, angle, MISSILERANGE, bulletslope, damage);
}

//
// A_FireShotgun2
//

void A_FireShotgun2(player_t *player, pspdef_t *psp)
{
  int i;

  S_StartSound(player->mo, sfx_pgrdat);
  P_SetMobjState(player->mo, S_PLAY_ATK2);
  player->ammo[weaponinfo[player->readyweapon].ammo]  -= BFGCELLS2;

  A_FireSomething(player,0);                                      // phares

  P_BulletSlope(player->mo);

  for (i=0; i<20; i++)
    {
      int damage = 5*(P_Random(pr_shotgun)%3+1);
      angle_t angle = player->mo->angle;
      // killough 5/5/98: remove dependence on order of evaluation:
      int t = P_Random(pr_shotgun);
      angle += (t - P_Random(pr_shotgun))<<19;
      t = P_Random(pr_shotgun);
	  PuffType = MT_STRIFEPUFF3;
      P_LineAttack(player->mo, angle, MISSILERANGE, bulletslope +
                   ((t - P_Random(pr_shotgun))<<5), damage);
    }
  P_NoiseAlert(player->mo, player->mo);	//kaiser
}

void A_Light0(player_t *player, pspdef_t *psp)
{
  player->extralight = 0;
}

void A_Light1 (player_t *player, pspdef_t *psp)
{
  player->extralight = 1;
}

void A_Light2 (player_t *player, pspdef_t *psp)
{
  player->extralight = 2;
}

//======================Strife Add on actions====================kaiser
void A_RemoveElectricBolt (player_t *player, pspdef_t *psp)
{
  P_SetPsprite (player, ps_flash, S_NULL);
}

void A_GunFlashThinker(player_t *player,pspdef_t *psp)
{
	switch(player->readyweapon)
	{
	case wp_crossbow:
		P_SetPsprite (player, ps_flash, S_NULL);
		break;
	case wp_sigil:
		P_DrawWhichSigil(player,psp);
		break;
	}
}

void A_ShowElectricBolt(player_t *player, pspdef_t *psp)
{
  statenum_t newstate;

  psp->sy -= RAISESPEED;

  if (psp->sy > WEAPONTOP)
    return;

  psp->sy = WEAPONTOP;

  // The weapon has been raised all the way,
  //  so change to the ready state.

  newstate = weaponinfo[player->readyweapon].readystate;

  P_SetPsprite(player, ps_weapon, newstate);
  P_SetPsprite (player, ps_flash, S_XBOWFLASH1);
}

void A_FireArrowBolt(player_t *player, pspdef_t *psp)
{
  mobj_t *missile;
  int accuracy = player->accuracy / 17;
  player->ammo[weaponinfo[player->readyweapon].ammo]--;
	missile = P_SPMAngle(player->mo, MT_ELECARROW, player->mo->angle
		+(((S_Random()&7)-4)<<(24 - accuracy)));
}

void A_BoltAlarm(mobj_t *thingy)
{
  if(thingy->type == MT_ELECARROW)
  P_NoiseAlert(thingy->target->player->mo, thingy); 
}

void A_FireArrowPoison(player_t *player, pspdef_t *psp)
{
 mobj_t *missile;
 int accuracy = player->accuracy / 17;
  player->ammo[weaponinfo[player->readyweapon].ammo]--;

  //A_FireSomething(player,P_Random(pr_pistol)&1);              // phares
  P_SetPsprite (player, ps_flash, S_NULL);
	missile = P_SPMAngle(player->mo, MT_POISARROW, player->mo->angle
		+(((S_Random()&7)-4)<<(24 - accuracy)));
}

void A_BringUpBolt(player_t *player, pspdef_t *psp)
{
	P_SetPsprite (player, ps_flash, S_XBOWFLASH1);
}

void A_FireRiffle(player_t *player, pspdef_t *psp)
{
	int accuracy = player->accuracy / 17;
	int damage = 4 * (P_Random(pr_shotgun) % 3 + 1);
	angle_t angle = player->mo->angle;

	S_StartSound(player->mo, sfx_rifle);

	if (!player->ammo[weaponinfo[player->readyweapon].ammo])
    return;

	P_SetMobjState(player->mo, S_PLAY_ATK2);
	player->ammo[weaponinfo[player->readyweapon].ammo]--;

	P_BulletSlope(player->mo);
	if (player->refire)
	{
      int t = P_Random(pr_shotgun);
      angle += (t - P_Random(pr_shotgun))<<(20 - accuracy);
      t = P_Random(pr_shotgun);
	}
	PuffType = MT_STRIFEPUFF;
	P_LineAttack(player->mo, angle, MISSILERANGE, bulletslope, damage);
	P_NoiseAlert(player->mo, player->mo);
}

//in the oringinal strife, the player can spawn grenades behind walls. I am not sure
//how this can be prevented..
void A_FireGrenade(player_t *player, pspdef_t *psp)	//based loosely on Zdoom's code
{
	mobjtype_t grenadetype;
	mobj_t *grenade;
	angle_t an;
	fixed_t tworadii;

	if (player == NULL)
		return;

	if(player->readyweapon == wp_grenade)
	{
		grenadetype = MT_HEGRENADE;
	}
	else if(player->readyweapon == wp_grenade2)
	{
		grenadetype = MT_PGRENADE;
	}
	player->ammo[weaponinfo[player->readyweapon].ammo]--;
	P_SetPsprite (player, ps_flash, weaponinfo[player->readyweapon].flashstate
		+ psp->state - &states[weaponinfo[player->readyweapon].atkstate]);

	player->mo->z += 32*FRACUNIT;
	grenade = P_SpawnSubMissile (player->mo, grenadetype);
	player->mo->z -= 32*FRACUNIT;
	if (grenade == NULL)
		return;
	S_StartSound(grenade, sfx_phoot);

	grenade->z = player->mo->z + (32+(player->lookdir / 16))*FRACUNIT;
	grenade->momz += (7+(player->lookdir / 16))*FRACUNIT;
	if(player->mo->feetisclipped == true)
	{
		grenade->z -= 10*FRACUNIT;
	}
	an = player->mo->angle >> ANGLETOFINESHIFT;
	tworadii = player->mo->radius + grenade->radius;
	grenade->x += FixedMul (finecosine[an], tworadii);
	grenade->y += FixedMul (finesine[an], tworadii);
	if(&states[weaponinfo[player->readyweapon].atkstate] == psp->state)
	an = player->mo->angle - ANG90;
	else an = player->mo->angle + ANG90;
	an >>= ANGLETOFINESHIFT;
	grenade->x += FixedMul (finecosine[an], 15*FRACUNIT);
	grenade->y += FixedMul (finesine[an], 15*FRACUNIT);
}

void A_GrendadeTic(mobj_t *mo)
{
	if(mo->type != MT_INQGRENADE)
	{
		mo->reactiontime -= 4;
	}
	else
	{
		mo->reactiontime --;	//INQGRENADE tics slower..-kaiser
	}
	if (mo->reactiontime <= 0)
	{
		mo->momx = mo->momy = mo->momz = 0;
		P_SetMobjState(mo, mo->info->deathstate);
		S_StartSound(mo,mo->info->deathsound);
		/*P_ExplodeMissile(mo);
		mo->flags &= ~MF_BOUNCES;*/
	}
}

void A_Rocketpuff(mobj_t *smoke)
{
	mobj_t *mpuff;
	mpuff = P_SpawnMobj(smoke->x, smoke->y, smoke->z, MT_MISSILESMOKE);
	S_StartSound(mpuff, sfx_rflite);
}

void A_FireFlameThrower(player_t *player, pspdef_t *psp)
{
 mobj_t *missile;
 int random;
  player->ammo[weaponinfo[player->readyweapon].ammo]--;
	psp->sx = ((S_Random()&3)-2)*FRACUNIT;
	psp->sy = WEAPONTOP+(S_Random()&3)*FRACUNIT;
	missile = P_SPMAngle(player->mo, MT_SFIREBALL, player->mo->angle
		+(((S_Random()&7)-4)<<24));
	missile->momz = JUMPGRAVITY/2;
	P_NoiseAlert(player->mo, player->mo);

	random = S_Random();

	if(random < 128)
	player->mo->angle += ANG90 / 40;
	else player->mo->angle -= ANG90 / 40;
}

void A_FireRaise(mobj_t *mo)
{

		mo->flags = MF_NOGRAVITY;
		mo->momz = FRACUNIT*((S_Random()%50)/16);
}

void A_SpawnGrenadeFire(mobj_t *mo)
{
	mobj_t *mpuff;
	mpuff = P_SpawnMobj(mo->x, mo->y, mo->z, MT_PFLAME);
}

void A_GPFireTic(mobj_t *mo)
{
	mo->reactiontime--;
	mo->reactiontime--;
	if (mo->reactiontime <= 0)
	{
		P_SetMobjState(mo, S_PFLAMEEND1);
	}
}

void A_BurnSpread(mobj_t *mo)
{
	mobj_t *thing;
	mo->momx += ((P_Random(pr_movefirethingy)%3)-1) << FRACBITS;
	mo->momy += ((P_Random(pr_movefirethingy)%3)-1) << FRACBITS;
	if (!(mo->flags & MF_DROPPED))
	{

		S_StartSound(mo, sfx_lgfire);
		thing = P_SpawnMobj(mo->x, mo->y,mo->z, MT_PFLAME);
	if (thing)
	{
		thing->momx = (S_Random()-128)<<11;
		thing->momy = (S_Random()-128)<<11;
		thing->target = mo;
		thing->flags |= MF_DROPPED;
		thing->reactiontime = (S_Random() % 16);
	}
	}
}

void A_Burn(mobj_t *thingy)
{
  P_RadiusAttack( thingy, thingy->target, 64 );
}

void A_TorpedoSound(player_t *player, pspdef_t *psp)
{
  S_StartSound(player->mo, sfx_proton);
  P_NoiseAlert(player->mo, player->mo);
}

void A_FireTorpedo(player_t *player, pspdef_t *psp)
{
	angle_t	angle;
    player->ammo[weaponinfo[player->readyweapon].ammo] -= BFGCELLS;
	if(player->health > 10)
	{
		player->health -= 10;
	}
	player->armorpoints -= 10;
    P_SpawnPlayerMissile(player->mo, MT_TORPEDO);
	angle = player->mo->angle+ANG180;
	angle >>= ANGLETOFINESHIFT;
	player->mo->momx += FixedMul(8*FRACUNIT, finecosine[angle]);
	player->mo->momy += FixedMul(8*FRACUNIT, finesine[angle]);
}

void A_TorpedoExplode(mobj_t *spread)
{
	int i;
	angle_t angle;
	mobj_t *shard;

	for(i = 0; i < 128; i++)
	{
		shard = P_SpawnMobj(spread->x, spread->y, spread->z, MT_TORPEDOSPREAD);
		angle += i*ANG4;
		shard->target = spread->target;
		shard->angle = angle;
		angle >>= ANGLETOFINESHIFT;
		shard->momx = FixedMul(shard->info->speed, finecosine[angle]);
		shard->momy = FixedMul(shard->info->speed, finesine[angle]);
	}
}

void A_GlassShatter(mobj_t *actor)
{
	int i;
	int r;
	mobjtype_t type;
	
	
	for(i = 0; i < 32; i++)
	{
	r = S_Random();
  if ( r<64 )
    type = MT_GLASS2;
  else if (r<128)
    type = MT_GLASS3;
  else
    type = MT_GLASS4;

  P_TossUpMobj(actor,type);
	}
}

void P_UpdateSigilWeapon(player_t *player)
{
	if(player->sigilowned == sl_sigil5)
		player->sigilowned = sl_sigil1;
	else
	player->sigilowned += 1;
}

void A_Sigil_A_Action(mobj_t* mo)
{
	mo->momx += ((P_Random(pr_sigil_a)%3)-1) << FRACBITS;
	mo->momy += ((P_Random(pr_sigil_a)%3)-1) << FRACBITS;
	mo->reactiontime--;
	if(mo->reactiontime <= 0)
	{
		P_SetMobjState(mo,mo->info->deathstate);
	}
}

void A_Sigil_A_Attack(mobj_t* mo)
{
	mobj_t* lightning1;
	mobj_t* lightning2;
	fixed_t x;
	fixed_t y;

	x = mo->x+((S_Random()&191)-64)*FRACUNIT;
	y = mo->y+((S_Random()&191)-64)*FRACUNIT;

	lightning1 = P_SpawnMobj(mo->x,mo->y,ONCEILINGZ,MT_SIGIL_A_ZAP_LEFT);
	lightning1->flags &= ~MF_NOGRAVITY;
	if(mo->flags & MF_BOSS)
	{
		lightning1->flags &= ~MF_GHOST;
		lightning1->flags |= MF_BOSS;	//give boss flag, remove ghost flag. - kaiser
	}
	lightning1->momx = 1;
	lightning1->momy = 1;
	P_CheckMissileSpawn(lightning1);

	lightning2 = P_SpawnMobj(x,y,ONCEILINGZ,MT_SIGIL_A_ZAP_RIGHT);
	lightning2->flags &= ~MF_NOGRAVITY;
	if(mo->flags & MF_BOSS)
	{
		lightning2->flags &= ~MF_GHOST;
		lightning2->flags |= MF_BOSS;
	}
	lightning1->momx = 1;
	lightning1->momy = 1;
	P_CheckMissileSpawn(lightning2);
}

void A_SigilSound(player_t *player, pspdef_t *psp)
{
	S_StartSound(player->mo,sfx_sigil);
}

void A_SigilBeginFire(player_t *player, pspdef_t *psp)
{
	P_SetPsprite (player, ps_flash, S_SIGIL_F1);
}

void A_FireSigilWeapon(player_t *player, pspdef_t *psp)
{
  mobj_t *sigil;
  angle_t angle;
  mobj_t *sigilspread;
  int i;

  angle = player->mo->angle+ANG180;
  angle >>= ANGLETOFINESHIFT;
  player->mo->momx += FixedMul(4*FRACUNIT, finecosine[angle]);
  player->mo->momy += FixedMul(4*FRACUNIT, finesine[angle]);
  P_NoiseAlert(player->mo, player->mo);

  if(player->readyweapon == wp_sigil && player->sigilowned == sl_sigil5)
  {
	  P_SpawnPlayerMissile(player->mo,MT_SIGIL_E_SHOT);
  }

  else if(player->readyweapon == wp_sigil && player->sigilowned == sl_sigil4)
  {
	  P_SpawnPlayerMissile(player->mo,MT_SIGIL_D_SHOT);
  }

  else if(player->readyweapon == wp_sigil && player->sigilowned == sl_sigil3)
  {
	  for(i = 0; i < 20; i++)
	{
		sigilspread = P_SPMAngle(player->mo, MT_SIGIL_C_SHOT,
			player->mo->angle+ANG180+ANG90);
		angle = sigilspread->angle += i*(ANG4*4);
		angle >>= ANGLETOFINESHIFT;
		sigilspread->momx = FixedMul(sigilspread->info->speed, finecosine[angle]);
		sigilspread->momy = FixedMul(sigilspread->info->speed, finesine[angle]);
	}
  }

  else if(player->readyweapon == wp_sigil && player->sigilowned == sl_sigil2)
  {
	  P_SpawnPlayerMissile(player->mo,MT_SIGIL_B_SHOT);
  }
  else
  {
  sigil = P_SPMAngle(player->mo, MT_SIGIL_A_GROUND, player->mo->angle);
  if(sigil)
	{
		sigil->z = sigil->floorz;
		if(!linetarget)
		{
			sigil->x = player->mo->x;
			sigil->y = player->mo->y;
			sigil->momx = 0;
			sigil->momy = 0;
		}
		else
		{
			sigil->x = linetarget->x;
			sigil->y = linetarget->y;
			sigil->momx = 0;
			sigil->momy = 0;
		}
	}
  }
}

#define INVERSECOLORMAP 32
void A_SigilShock(player_t *player, pspdef_t *psp)
{
	player->misctime = 10;
	P_DamageMobj(player->mo,NULL,NULL,5);
	player->fixedcolormap = INVERSECOLORMAP;
}

void A_SigilTrail(mobj_t *actor)
{
	mobj_t *mo;

	mo = P_SpawnMobj(actor->x,actor->y,actor->z,MT_SIGIL_TRAIL);
	mo->angle = actor->angle;
}

void A_KeepSigilDrawing(player_t *player, pspdef_t *psp)
{
	P_DrawWhichSigil(player,psp);
}

void A_Sigil_E_Action(mobj_t *actor)
{
	angle_t angle;
	mobj_t *mo;
	int dir;

	dir = actor->angle;

	mo = P_SpawnMobj(actor->x, actor->y, actor->z, MT_SIGIL_E_OFFSHOOT);
	if(actor->flags & (MF_BOSS))
	{
		mo->flags |= MF_BOSS;
		mo->flags &= ~MF_GHOST;
	}
	angle = mo->angle = dir+ANG90;
	mo->target = actor->target;
	angle >>= ANGLETOFINESHIFT;
	mo->momx = FixedMul(mo->info->speed, finecosine[angle]);
	mo->momy = FixedMul(mo->info->speed, finesine[angle]);

	mo = P_SpawnMobj(actor->x, actor->y, actor->z, MT_SIGIL_E_OFFSHOOT);
	if(actor->flags & (MF_BOSS))
	{
		mo->flags |= MF_BOSS;
		mo->flags &= ~MF_GHOST;
	}
	angle = mo->angle = dir-ANG90;
	mo->target = actor->target;
	angle  >>= ANGLETOFINESHIFT;
	mo->momx = FixedMul(mo->info->speed, finecosine[angle]);
	mo->momy = FixedMul(mo->info->speed, finesine[angle]);

	mo = P_SpawnMobj(actor->x, actor->y, actor->z, MT_SIGIL_E_OFFSHOOT);
	if(actor->flags & (MF_BOSS))
	{
		mo->flags |= MF_BOSS;
		mo->flags &= ~MF_GHOST;
	}
	angle = mo->angle = dir;
	mo->target = actor->target;
	angle  >>= ANGLETOFINESHIFT;
	mo->momx = FixedMul(mo->info->speed, finecosine[angle]);
	mo->momy = FixedMul(mo->info->speed, finesine[angle]);
}

//
// P_SetupPsprites
// Called at start of level for each player.
//

void P_SetupPsprites(player_t *player)
{
  int i;

  // remove all psprites
  for (i=0; i<NUMPSPRITES; i++)
    player->psprites[i].state = NULL;

  // spawn the gun
  player->pendingweapon = player->readyweapon;
  P_BringUpWeapon(player);
}

//
// P_MovePsprites
// Called every tic by player thinking routine.
//

void P_MovePsprites(player_t *player)
{
  pspdef_t *psp = player->psprites;
  int i;

  // a null state means not active
  // drop tic count and possibly change state
  // a -1 tic count never changes

  for (i=0; i<NUMPSPRITES; i++, psp++)
    if (psp->state && psp->tics != -1 && !--psp->tics)
      P_SetPsprite(player, i, psp->state->nextstate);

  player->psprites[ps_flash].sx = player->psprites[ps_weapon].sx;
  player->psprites[ps_flash].sy = player->psprites[ps_weapon].sy;
}
