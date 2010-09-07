/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 * $Id: p_enemy.c,v 1.11.2.1 2002/07/20 18:08:36 proff_fs Exp $
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
 *      Enemy thinking, AI.
 *      Action Pointer Functions
 *      that are associated with states/frames.
 *
 *-----------------------------------------------------------------------------*/

static const char
rcsid[] = "$Id: p_enemy.c,v 1.11.2.1 2002/07/20 18:08:36 proff_fs Exp $";

#include "doomstat.h"
#include "m_random.h"
#include "r_main.h"
#include "p_maputl.h"
#include "p_map.h"
#include "p_setup.h"
#include "p_spec.h"
#include "s_sound.h"
#include "sounds.h"
#include "p_inter.h"
#include "g_game.h"
#include "p_enemy.h"
#include "p_tick.h"
#include "m_bbox.h"
#include "lprintf.h"
#include "p_user.h"	//kaiser
#include "d_main.h"	//kaiser

static boolean waitForAttack = false;	//kaiser
static mobj_t *current_actor;

typedef enum {
  DI_EAST,
  DI_NORTHEAST,
  DI_NORTH,
  DI_NORTHWEST,
  DI_WEST,
  DI_SOUTHWEST,
  DI_SOUTH,
  DI_SOUTHEAST,
  DI_NODIR,
  NUMDIRS
} dirtype_t;

void A_Fall(mobj_t *actor);
void A_FaceTarget(mobj_t *actor);
static void P_NewChaseDir(mobj_t *actor);
void P_ZBumpCheck(mobj_t *);

//
// ENEMY THINKING
// Enemies are allways spawned
// with targetplayer = -1, threshold = 0
// Most monsters are spawned unaware of all players,
// but some can be made preaware
//

//
// Called by P_NoiseAlert.
// Recursively traverse adjacent sectors,
// sound blocking lines cut off traversal.
//
// killough 5/5/98: reformatted, cleaned up

static void P_RecursiveSound(sector_t *sec, int soundblocks,
           mobj_t *soundtarget)
{
  int i;

  // wake up all monsters in this sector
  if (sec->validcount == validcount && sec->soundtraversed <= soundblocks+1)
    return;             // already flooded

  sec->validcount = validcount;
  sec->soundtraversed = soundblocks+1;
  P_SetTarget(&sec->soundtarget, soundtarget);

  for (i=0; i<sec->linecount; i++)
    {
      sector_t *other;
      line_t *check = sec->lines[i];

      if (!(check->flags & ML_TWOSIDED))
        continue;

      P_LineOpening(check);

      if (openrange <= 0)
        continue;       // closed door

      other=sides[check->sidenum[sides[check->sidenum[0]].sector==sec]].sector;

      if (!(check->flags & ML_SOUNDBLOCK))
        P_RecursiveSound(other, soundblocks, soundtarget);
      else
        if (!soundblocks)
          P_RecursiveSound(other, 1, soundtarget);
    }
}

//
// P_NoiseAlert
// If a monster yells at a player,
// it will alert other monsters to the player.
//
void P_NoiseAlert(mobj_t *target, mobj_t *emitter)
{
  validcount++;
  P_RecursiveSound(emitter->subsector->sector, 0, target);
}

//
// P_CheckMeleeRange
//

static boolean P_CheckMeleeRange(mobj_t *actor)
{
  mobj_t *pl = actor->target;

  return  // killough 7/18/98: friendly monsters don't attack other friends
    pl && !(actor->flags & pl->flags & MF_FRIEND) &&
    (P_AproxDistance(pl->x-actor->x, pl->y-actor->y) <
     MELEERANGE - 20*FRACUNIT + pl->info->radius) &&
    P_CheckSight(actor, actor->target);
}

//
// P_HitFriend()
//
// killough 12/98
// This function tries to prevent shooting at friends

static boolean P_HitFriend(mobj_t *actor)
{
  return actor->flags & MF_FRIEND && actor->target &&
    (P_AimLineAttack(actor,
         R_PointToAngle2(actor->x, actor->y,
             actor->target->x, actor->target->y),
         P_AproxDistance(actor->x-actor->target->x,
             actor->y-actor->target->y), 0),
     linetarget) && linetarget != actor->target &&
    !((linetarget->flags ^ actor->flags) & MF_FRIEND);
}

//
// P_CheckMissileRange
//
boolean P_CheckMissileRange(mobj_t *actor)
{
  fixed_t dist;

  if (!P_CheckSight(actor, actor->target))
    return false;

  if (actor->flags & MF_JUSTHIT)
    {      // the target just hit the enemy, so fight back!
      actor->flags &= ~MF_JUSTHIT;

      /* killough 7/18/98: no friendly fire at corpses
       * killough 11/98: prevent too much infighting among friends
       * cph - yikes, talk about fitting everything on one line... */

      return
  !(actor->flags & MF_FRIEND) ||
  (actor->target->health > 0 &&
   (!(actor->target->flags & MF_FRIEND) ||
    (actor->target->player ?
     monster_infighting || P_Random(pr_defect) >128 :
     !(actor->target->flags & MF_JUSTHIT) && P_Random(pr_defect) >128)));
    }

  /* killough 7/18/98: friendly monsters don't attack other friendly
   * monsters or players (except when attacked, and then only once)
   */
  if (actor->flags & actor->target->flags & MF_FRIEND)
    return false;

  if (actor->reactiontime)
    return false;       // do not attack yet

  // OPTIMIZE: get this from a global checksight
  dist = P_AproxDistance ( actor->x-actor->target->x,
                           actor->y-actor->target->y) - 64*FRACUNIT;

  if (!actor->info->meleestate)
    dist -= 128*FRACUNIT;       // no melee attack, so fire more

  dist >>= FRACBITS;

  if (dist > 200)
    dist = 200;

  if(actor->type == MT_ENTITY)	//shoots from far away
	  dist >>= 2;

  if(actor->type == MT_DOGS)
	  dist >>= 1;

  if (P_Random(pr_missrange) < dist)
    return false;

  if (P_HitFriend(actor))
    return false;

  return true;
}

/*
 * P_IsOnLift
 *
 * killough 9/9/98:
 *
 * Returns true if the object is on a lift. Used for AI,
 * since it may indicate the need for crowded conditions,
 * or that a monster should stay on the lift for a while
 * while it goes up or down.
 */

static boolean P_IsOnLift(const mobj_t *actor)
{
  const sector_t *sec = actor->subsector->sector;
  line_t line;
  int l;

  // Short-circuit: it's on a lift which is active.
  if (sec->floordata && ((thinker_t *) sec->floordata)->function==T_PlatRaise)
    return true;

  // Check to see if it's in a sector which can be activated as a lift.
  if ((line.tag = sec->tag))
    for (l = -1; (l = P_FindLineFromLineTag(&line, l)) >= 0;)
      switch (lines[l].special)
  {
  case  10: case  14: case  15: case  20: case  21: case  22:
  case  47: case  53: case  62: case  66: case  67: case  68:
  case  87: case  88: case  95: case 120: case 121: case 122:
  case 123: case 143: case 162: case 163:
  case 144: case 148: case 149: /*case 211:*/ case 227:
  case 236:
    return true;
  }

  return false;
}

/*
 * P_IsUnderDamage
 *
 * killough 9/9/98:
 *
 * Returns nonzero if the object is under damage based on
 * their current position. Returns 1 if the damage is moderate,
 * -1 if it is serious. Used for AI.
 */

static int P_IsUnderDamage(mobj_t *actor)
{
  const struct msecnode_s *seclist;
  const ceiling_t *cl;             // Crushing ceiling
  int dir = 0;
  for (seclist=actor->touching_sectorlist; seclist; seclist=seclist->m_tnext)
    if ((cl = seclist->m_sector->ceilingdata) &&
  cl->thinker.function == T_MoveCeiling)
      dir |= cl->direction;
  return dir;
}

//
// P_Move
// Move in the current direction,
// returns false if the move is blocked.
//

static fixed_t xspeed[8] = {FRACUNIT,47000,0,-47000,-FRACUNIT,-47000,0,47000};
static fixed_t yspeed[8] = {0,47000,FRACUNIT,47000,0,-47000,-FRACUNIT,-47000};

// 1/11/98 killough: Limit removed on special lines crossed
extern  line_t **spechit;          // New code -- killough
extern  int    numspechit;

static boolean P_Move(mobj_t *actor, boolean dropoff) /* killough 9/12/98 */
{
  fixed_t tryx, tryy, deltax, deltay, origx, origy;
  boolean try_ok;
  int movefactor = ORIG_FRICTION_FACTOR;    // killough 10/98
  int friction = ORIG_FRICTION;
  int speed;

  if (actor->movedir == DI_NODIR)
    return false;

#ifdef RANGECHECK
  if ((unsigned)actor->movedir >= 8)
    I_Error ("P_Move: Weird actor->movedir!");
#endif

  // killough 10/98: make monsters get affected by ice and sludge too:

  if (monster_friction)
    movefactor = P_GetMoveFactor(actor, &friction);

  speed = actor->info->speed;

  if (friction < ORIG_FRICTION &&     // sludge
      !(speed = ((ORIG_FRICTION_FACTOR - (ORIG_FRICTION_FACTOR-movefactor)/2)
     * speed) / ORIG_FRICTION_FACTOR))
    speed = 1;      // always give the monster a little bit of speed

  tryx = (origx = actor->x) + (deltax = speed * xspeed[actor->movedir]);
  tryy = (origy = actor->y) + (deltay = speed * yspeed[actor->movedir]);

  try_ok = P_TryMove(actor, tryx, tryy, dropoff);

  // killough 10/98:
  // Let normal momentum carry them, instead of steptoeing them across ice.

  if (try_ok && friction > ORIG_FRICTION)
    {
      actor->x = origx;
      actor->y = origy;
      movefactor *= FRACUNIT / ORIG_FRICTION_FACTOR / 4;
      actor->momx += FixedMul(deltax, movefactor);
      actor->momy += FixedMul(deltay, movefactor);
    }

  if (!try_ok)
    {      // open any specials
      int good;

      if (actor->flags & MF_FLOAT && floatok && actor->type != MT_STALKER)	//kaiser
        {
          if (actor->z < tmfloorz)          // must adjust height
            actor->z += FLOATSPEED;
          else
            actor->z -= FLOATSPEED;

          actor->flags |= MF_INFLOAT;

    return true;
        }

      if (!numspechit)
        return false;

      actor->movedir = DI_NODIR;

      /* if the special is not a door that can be opened, return false
       *
       * killough 8/9/98: this is what caused monsters to get stuck in
       * doortracks, because it thought that the monster freed itself
       * by opening a door, even if it was moving towards the doortrack,
       * and not the door itself.
       *
       * killough 9/9/98: If a line blocking the monster is activated,
       * return true 90% of the time. If a line blocking the monster is
       * not activated, but some other line is, return false 90% of the
       * time. A bit of randomness is needed to ensure it's free from
       * lockups, but for most cases, it returns the correct result.
       *
       * Do NOT simply return false 1/4th of the time (causes monsters to
       * back out when they shouldn't, and creates secondary stickiness).
       */

      for (good = false; numspechit--; )
        if (P_UseSpecialLine(actor, spechit[numspechit], 0))
    good |= spechit[numspechit] == blockline ? 1 : 2;

      /* cph - compatibility maze here
       * Boom v2.01 and orig. Doom return "good"
       * Boom v2.02 and LxDoom return good && (P_Random(pr_trywalk)&3)
       * MBF plays even more games
       */
      if (!good || comp[comp_doorstuck]) return good;
      if (!mbf_features)
  return (P_Random(pr_trywalk)&3); /* jff 8/13/98 */
      else /* finally, MBF code */
  return ((P_Random(pr_opendoor) >= 230) ^ (good & 1));
    }
  else
    actor->flags &= ~MF_INFLOAT;

  /* killough 11/98: fall more slowly, under gravity, if felldown==true */
  /*if (!(actor->flags & MF_FLOAT) &&
      (!felldown || !mbf_features))
    actor->z = actor->floorz;*/	//removed, all enemies in strife fall gradually - kaiser

  return true;
}

/*
 * P_SmartMove
 *
 * killough 9/12/98: Same as P_Move, except smarter
 */

static boolean P_SmartMove(mobj_t *actor)
{
  mobj_t *target = actor->target;
  int on_lift, dropoff = false, under_damage;

  /* killough 9/12/98: Stay on a lift if target is on one */
  on_lift = !comp[comp_staylift]
    && target && target->health > 0
    && target->subsector->sector->tag==actor->subsector->sector->tag &&
    P_IsOnLift(actor);

  under_damage = monster_avoid_hazards && P_IsUnderDamage(actor);

  // killough 10/98: allow dogs to drop off of taller ledges sometimes.
  // dropoff==1 means always allow it, dropoff==2 means only up to 128 high,
  // and only if the target is immediately on the other side of the line.

#ifdef DOGS
  if (actor->type == MT_DOGS && target && dog_jumping &&
      !((target->flags ^ actor->flags) & MF_FRIEND) &&
      P_AproxDistance(actor->x - target->x,
          actor->y - target->y) < FRACUNIT*144 &&
      P_Random(pr_dropoff) < 235)
    dropoff = 2;
#endif

  if (!P_Move(actor, dropoff))
    return false;

  // killough 9/9/98: avoid crushing ceilings or other damaging areas
  if (
      (on_lift && P_Random(pr_stayonlift) < 230 &&      // Stay on lift
       !P_IsOnLift(actor))
      ||
      (monster_avoid_hazards && !under_damage &&  // Get away from damage
       (under_damage = P_IsUnderDamage(actor)) &&
       (under_damage < 0 || P_Random(pr_avoidcrush) < 200))
      )
    actor->movedir = DI_NODIR;    // avoid the area (most of the time anyway)

  return true;
}

//
// TryWalk
// Attempts to move actor on
// in its current (ob->moveangle) direction.
// If blocked by either a wall or an actor
// returns FALSE
// If move is either clear or blocked only by a door,
// returns TRUE and sets...
// If a door is in the way,
// an OpenDoor call is made to start it opening.
//

boolean P_TryWalk(mobj_t *actor)
{
  if (!P_SmartMove(actor))
    return false;
  actor->movecount = P_Random(pr_trywalk)&15;
  return true;
}

//
// P_DoNewChaseDir
//
// killough 9/8/98:
//
// Most of P_NewChaseDir(), except for what
// determines the new direction to take
//

static void P_DoNewChaseDir(mobj_t *actor, fixed_t deltax, fixed_t deltay)
{
  dirtype_t xdir, ydir, tdir;
  dirtype_t olddir = actor->movedir;
  dirtype_t turnaround = olddir;

  if (turnaround != DI_NODIR)         // find reverse direction
    turnaround ^= 4;

  xdir =
    deltax >  10*FRACUNIT ? DI_EAST :
    deltax < -10*FRACUNIT ? DI_WEST : DI_NODIR;

  ydir =
    deltay < -10*FRACUNIT ? DI_SOUTH :
    deltay >  10*FRACUNIT ? DI_NORTH : DI_NODIR;

  // try direct route
  if (xdir != DI_NODIR && ydir != DI_NODIR && turnaround !=
      (actor->movedir = deltay < 0 ? deltax > 0 ? DI_SOUTHEAST : DI_SOUTHWEST :
       deltax > 0 ? DI_NORTHEAST : DI_NORTHWEST) && P_TryWalk(actor))
    return;

  // try other directions
  if (P_Random(pr_newchase) > 200 || abs(deltay)>abs(deltax))
    tdir = xdir, xdir = ydir, ydir = tdir;

  if ((xdir == turnaround ? xdir = DI_NODIR : xdir) != DI_NODIR &&
      (actor->movedir = xdir, P_TryWalk(actor)))
    return;         // either moved forward or attacked

  if ((ydir == turnaround ? ydir = DI_NODIR : ydir) != DI_NODIR &&
      (actor->movedir = ydir, P_TryWalk(actor)))
    return;

  // there is no direct path to the player, so pick another direction.
  if (olddir != DI_NODIR && (actor->movedir = olddir, P_TryWalk(actor)))
    return;

  // randomly determine direction of search
  if (P_Random(pr_newchasedir) & 1)
    {
      for (tdir = DI_EAST; tdir <= DI_SOUTHEAST; tdir++)
        if (tdir != turnaround && (actor->movedir = tdir, P_TryWalk(actor)))
    return;
    }
  else
    for (tdir = DI_SOUTHEAST; tdir != DI_EAST-1; tdir--)
      if (tdir != turnaround && (actor->movedir = tdir, P_TryWalk(actor)))
  return;

  if ((actor->movedir = turnaround) != DI_NODIR && !P_TryWalk(actor))
    actor->movedir = DI_NODIR;
}

//
// killough 11/98:
//
// Monsters try to move away from tall dropoffs.
//
// In Doom, they were never allowed to hang over dropoffs,
// and would remain stuck if involuntarily forced over one.
// This logic, combined with p_map.c (P_TryMove), allows
// monsters to free themselves without making them tend to
// hang over dropoffs.

static fixed_t dropoff_deltax, dropoff_deltay, floorz;

static boolean PIT_AvoidDropoff(line_t *line)
{
  if (line->backsector                          && // Ignore one-sided linedefs
      tmbbox[BOXRIGHT]  > line->bbox[BOXLEFT]   &&
      tmbbox[BOXLEFT]   < line->bbox[BOXRIGHT]  &&
      tmbbox[BOXTOP]    > line->bbox[BOXBOTTOM] && // Linedef must be contacted
      tmbbox[BOXBOTTOM] < line->bbox[BOXTOP]    &&
      P_BoxOnLineSide(tmbbox, line) == -1)
    {
      fixed_t front = line->frontsector->floorheight;
      fixed_t back  = line->backsector->floorheight;
      angle_t angle;

      // The monster must contact one of the two floors,
      // and the other must be a tall dropoff (more than 24).

      if (back == floorz && front < floorz - FRACUNIT*24)
  angle = R_PointToAngle2(0,0,line->dx,line->dy);   // front side dropoff
      else
  if (front == floorz && back < floorz - FRACUNIT*24)
    angle = R_PointToAngle2(line->dx,line->dy,0,0); // back side dropoff
  else
    return true;

      // Move away from dropoff at a standard speed.
      // Multiple contacted linedefs are cumulative (e.g. hanging over corner)
      dropoff_deltax -= finesine[angle >> ANGLETOFINESHIFT]*32;
      dropoff_deltay += finecosine[angle >> ANGLETOFINESHIFT]*32;
    }
  return true;
}

//
// Driver for above
//

static fixed_t P_AvoidDropoff(mobj_t *actor)
{
  int yh=((tmbbox[BOXTOP]   = actor->y+actor->radius)-bmaporgy)>>MAPBLOCKSHIFT;
  int yl=((tmbbox[BOXBOTTOM]= actor->y-actor->radius)-bmaporgy)>>MAPBLOCKSHIFT;
  int xh=((tmbbox[BOXRIGHT] = actor->x+actor->radius)-bmaporgx)>>MAPBLOCKSHIFT;
  int xl=((tmbbox[BOXLEFT]  = actor->x-actor->radius)-bmaporgx)>>MAPBLOCKSHIFT;
  int bx, by;

  floorz = actor->z;            // remember floor height

  dropoff_deltax = dropoff_deltay = 0;

  // check lines

  validcount++;
  for (bx=xl ; bx<=xh ; bx++)
    for (by=yl ; by<=yh ; by++)
      P_BlockLinesIterator(bx, by, PIT_AvoidDropoff);  // all contacted lines

  return dropoff_deltax | dropoff_deltay;   // Non-zero if movement prescribed
}

//
// P_NewChaseDir
//
// killough 9/8/98: Split into two functions
//

static void P_NewChaseDir(mobj_t *actor)
{
  mobj_t *target = actor->target;
  fixed_t deltax = target->x - actor->x;
  fixed_t deltay = target->y - actor->y;

  // killough 8/8/98: sometimes move away from target, keeping distance
  //
  // 1) Stay a certain distance away from a friend, to avoid being in their way
  // 2) Take advantage over an enemy without missiles, by keeping distance

  actor->strafecount = 0;

  if (mbf_features) {
    if (actor->floorz - actor->dropoffz > FRACUNIT*24 &&
  actor->z <= actor->floorz &&
  !(actor->flags & (MF_DROPOFF|MF_FLOAT)) &&
  !comp[comp_dropoff] &&
  P_AvoidDropoff(actor)) /* Move away from dropoff */
      {
  P_DoNewChaseDir(actor, dropoff_deltax, dropoff_deltay);

  // If moving away from dropoff, set movecount to 1 so that
  // small steps are taken to get monster away from dropoff.

  actor->movecount = 1;
  return;
      }
    else
      {
  fixed_t dist = P_AproxDistance(deltax, deltay);

  // Move away from friends when too close, except
  // in certain situations (e.g. a crowded lift)

  if (actor->flags & target->flags & MF_FRIEND &&
      distfriend << FRACBITS > dist &&
      !P_IsOnLift(target) && !P_IsUnderDamage(actor))
  {
    deltax = -deltax, deltay = -deltay;
  } else
    if (target->health > 0 && (actor->flags ^ target->flags) & MF_FRIEND)
      {   // Live enemy target
        if (monster_backing &&
      actor->info->missilestate /*&& actor->type != MT_SKULL*/ &&
      ((!target->info->missilestate && dist < MELEERANGE*2) ||
       (target->player && dist < MELEERANGE*3 &&
        (target->player->readyweapon == wp_fist))))
    {       // Back away from melee attacker
      actor->strafecount = P_Random(pr_enemystrafe) & 15;
      deltax = -deltax, deltay = -deltay;
    }
      }
      }
  }

  P_DoNewChaseDir(actor, deltax, deltay);

  // If strafing, set movecount to strafecount so that old Doom
  // logic still works the same, except in the strafing part

  if (actor->strafecount)
    actor->movecount = actor->strafecount;
}

//
// P_IsVisible
//
// killough 9/9/98: whether a target is visible to a monster
//

static boolean P_IsVisible(mobj_t *actor, mobj_t *mo, boolean allaround)
{
  if (!allaround)
    {
      angle_t an = R_PointToAngle2(actor->x, actor->y,
           mo->x, mo->y) - actor->angle;
      if (an > ANG90 && an < ANG270 &&
    P_AproxDistance(mo->x-actor->x, mo->y-actor->y) > MELEERANGE)
  return false;
    }
  return P_CheckSight(actor, mo);
}

//
// PIT_FindTarget
//
// killough 9/5/98
//
// Finds monster targets for other monsters
//

static int current_allaround;

static boolean PIT_FindTarget(mobj_t *mo)
{
  mobj_t *actor = current_actor;

  if (!((mo->flags ^ actor->flags) & MF_FRIEND &&        // Invalid target
  mo->health > 0 && (mo->flags & MF_COUNTKILL /*|| mo->type == MT_SKULL*/)))
    return true;

  // If the monster is already engaged in a one-on-one attack
  // with a healthy friend, don't attack around 60% the time
  {
    const mobj_t *targ = mo->target;
    if (targ && targ->target == mo &&
  P_Random(pr_skiptarget) > 100 &&
  (targ->flags ^ mo->flags) & MF_FRIEND &&
  targ->health*2 >= targ->info->spawnhealth)
      return true;
  }

  if (!P_IsVisible(actor, mo, current_allaround))
    return true;

  P_SetTarget(&actor->lastenemy, actor->target);  // Remember previous target
  P_SetTarget(&actor->target, mo);                // Found target

  // Move the selected monster to the end of its associated
  // list, so that it gets searched last next time.

  {
    thinker_t *cap = &thinkerclasscap[mo->flags & MF_FRIEND ?
             th_friends : th_enemies];
    (mo->thinker.cprev->cnext = mo->thinker.cnext)->cprev = mo->thinker.cprev;
    (mo->thinker.cprev = cap->cprev)->cnext = &mo->thinker;
    (mo->thinker.cnext = cap)->cprev = &mo->thinker;
  }

  return false;
}

//
// P_LookForPlayers
// If allaround is false, only look 180 degrees in front.
// Returns true if a player is targeted.
//

static boolean P_LookForPlayers(mobj_t *actor, boolean allaround)
{
  player_t *player;
  int stop, stopc, c;

  if (actor->flags & MF_FRIEND)
    {  // killough 9/9/98: friendly monsters go about players differently
      int anyone;

#if 0
      if (!allaround) // If you want friendly monsters not to awaken unprovoked
  return false;
#endif

      // Go back to a player, no matter whether it's visible or not
      for (anyone=0; anyone<=1; anyone++)
  for (c=0; c<MAXPLAYERS; c++)
    if (playeringame[c] && players[c].playerstate==PST_LIVE &&
        (anyone || P_IsVisible(actor, players[c].mo, allaround)))
      {
        P_SetTarget(&actor->target, players[c].mo);

        // killough 12/98:
        // get out of refiring loop, to avoid hitting player accidentally

        if (actor->info->missilestate)
    {
      P_SetMobjState(actor, actor->info->seestate);
      actor->flags &= ~MF_JUSTHIT;
    }

        return true;
      }

      return false;
    }

  // Change mask of 3 to (MAXPLAYERS-1) -- killough 2/15/98:
  stop = (actor->lastlook-1)&(MAXPLAYERS-1);

  c = 0;

  stopc = !mbf_features &&
    !demo_compatibility && monsters_remember ?
    MAXPLAYERS : 2;       // killough 9/9/98

  for (;; actor->lastlook = (actor->lastlook+1)&(MAXPLAYERS-1))
    {
      if (!playeringame[actor->lastlook])
  continue;

      // killough 2/15/98, 9/9/98:
      if (c++ == stopc || actor->lastlook == stop)  // done looking
  return false;

      player = &players[actor->lastlook];

      if (player->health <= 0)
  continue;               // dead

      if (!P_IsVisible(actor, player->mo, allaround))
  continue;

      P_SetTarget(&actor->target, player->mo);

      /* killough 9/9/98: give monsters a threshold towards getting players
       * (we don't want it to be too easy for a player with dogs :)
       */
      if (!comp[comp_pursuit])
  actor->threshold = 60;

      return true;
    }
}

//
// Friendly monsters, by Lee Killough 7/18/98
//
// Friendly monsters go after other monsters first, but
// also return to owner if they cannot find any targets.
// A marine's best friend :)  killough 7/18/98, 9/98
//

static boolean P_LookForMonsters(mobj_t *actor, boolean allaround)
{
  thinker_t *cap, *th;

  if (demo_compatibility)
    return false;

  if(gamemap == 7 && waitForAttack == false)	//rebels in map07 won't attack until you make noise
	  return false;

  if (actor->lastenemy && actor->lastenemy->health > 0 && monsters_remember &&
      !(actor->lastenemy->flags & actor->flags & MF_FRIEND)) // not friends
    {
      P_SetTarget(&actor->target, actor->lastenemy);
      P_SetTarget(&actor->lastenemy, NULL);
      return true;
    }

  /* Old demos do not support monster-seeking bots */
  if (!mbf_features)
    return false;

  // Search the threaded list corresponding to this object's potential targets
  cap = &thinkerclasscap[actor->flags & MF_FRIEND ? th_enemies : th_friends];

  // Search for new enemy

  if (cap->cnext != cap)        // Empty list? bail out early
    {
      int x = (actor->x - bmaporgx)>>MAPBLOCKSHIFT;
      int y = (actor->y - bmaporgy)>>MAPBLOCKSHIFT;
      int d;

      current_actor = actor;
      current_allaround = allaround;

      // Search first in the immediate vicinity.

      if (!P_BlockThingsIterator(x, y, PIT_FindTarget))
  return true;

      for (d=1; d<5; d++)
  {
    int i = 1 - d;
    do
      if (!P_BlockThingsIterator(x+i, y-d, PIT_FindTarget) ||
    !P_BlockThingsIterator(x+i, y+d, PIT_FindTarget))
        return true;
    while (++i < d);
    do
      if (!P_BlockThingsIterator(x-d, y+i, PIT_FindTarget) ||
    !P_BlockThingsIterator(x+d, y+i, PIT_FindTarget))
        return true;
    while (--i + d >= 0);
  }

      {   // Random number of monsters, to prevent patterns from forming
  int n = (P_Random(pr_friends) & 31) + 15;

  for (th = cap->cnext; th != cap; th = th->cnext)
    if (--n < 0)
      {
        // Only a subset of the monsters were searched. Move all of
        // the ones which were searched so far, to the end of the list.

        (cap->cnext->cprev = cap->cprev)->cnext = cap->cnext;
        (cap->cprev = th->cprev)->cnext = cap;
        (th->cprev = cap)->cnext = th;
        break;
     }
    else
      if (!PIT_FindTarget((mobj_t *) th))   // If target sighted
        return true;
      }
    }

  return false;  // No monster found
}

//
// P_LookForTargets
//
// killough 9/5/98: look for targets to go after, depending on kind of monster
//

boolean P_LookForTargets(mobj_t *actor, int allaround)
{
  return actor->flags & MF_FRIEND ?
    P_LookForMonsters(actor, allaround) || P_LookForPlayers (actor, allaround):
    P_LookForPlayers (actor, allaround) || P_LookForMonsters(actor, allaround);
}

//
// P_HelpFriend
//
// killough 9/8/98: Help friends in danger of dying
//

static boolean P_HelpFriend(mobj_t *actor)
{
  thinker_t *cap, *th;

  // If less than 33% health, self-preservation rules
  if (actor->health*3 < actor->info->spawnhealth)
    return false;

  current_actor = actor;
  current_allaround = true;

  // Possibly help a friend under 50% health
  cap = &thinkerclasscap[actor->flags & MF_FRIEND ? th_friends : th_enemies];

  for (th = cap->cnext; th != cap; th = th->cnext)
    if (((mobj_t *) th)->health*2 >= ((mobj_t *) th)->info->spawnhealth)
      {
  if (P_Random(pr_helpfriend) < 180)
    break;
      }
    else
      if (((mobj_t *) th)->flags & MF_JUSTHIT &&
    ((mobj_t *) th)->target &&
    ((mobj_t *) th)->target != actor->target &&
    !PIT_FindTarget(((mobj_t *) th)->target))
  {
    // Ignore any attacking monsters, while searching for friend
    actor->threshold = BASETHRESHOLD;
    return true;
  }

  return false;
}


//
// ACTION ROUTINES
//

//
// A_Look
// Stay in state until a player is sighted.
//

void A_Look(mobj_t *actor)
{
  int sound;
  mobj_t *targ = actor->subsector->sector->soundtarget;
  actor->threshold = 0; // any shot will wake up

  /* killough 7/18/98:
   * Friendly monsters go after other monsters first, but
   * also return to player, without attacking them, if they
   * cannot find any targets. A marine's best friend :)
   */
  actor->pursuecount = 0;

  if (!(actor->flags & MF_FRIEND && P_LookForTargets(actor, false)) &&
      !((targ = actor->subsector->sector->soundtarget) &&
  targ->flags & MF_SHOOTABLE &&
  (P_SetTarget(&actor->target, targ),
   !(actor->flags & MF_AMBUSH) || P_CheckSight(actor, targ))) &&	//kaiser
      (actor->flags & MF_FRIEND || !P_LookForTargets(actor, false)))
    return;

  // go into chase state

  if (actor->info->seesound)
    {
      sound = actor->info->seesound;
      if (actor->flags & MF_BOSS)	//kaiser 9-04-05
        S_StartSound(NULL, sound);          // full volume
      else
        S_StartSound(actor, sound);
    }
  P_SetMobjState(actor, actor->info->seestate);
  if(!(actor->flags & MF_PEASANT))
  actor->disable_npc = true;
}

//
// A_KeepChasing
//
// killough 10/98:
// Allows monsters to continue movement while attacking
//

void A_KeepChasing(mobj_t *actor)
{
  if (actor->movecount)
    {
      actor->movecount--;
      if (actor->strafecount)
  actor->strafecount--;
      P_SmartMove(actor);
    }
}

//
// A_Chase
// Actor has a melee attack,
// so it tries to close as fast as possible
//

void A_Chase(mobj_t *actor)
{
	int sound;		//kaiser
	actor->flags |= MF_NOALERT;

	if(actor->flags & MF_FRIEND && gamemap == 7)
		waitForAttack = true;
	/*Inquisitor Stuff
	 -kaiser*/
	//int an;
	if(actor->type == MT_INQUISITOR && actor->z != actor->floorz)
		return;	//don't move if not on floor

	if(actor->type == MT_INQUISITOR)
	{
		actor->flags |= MF_FLOAT; //wouldn't this work if he already had this set?
		actor->info->speed = 12;	//give him speed!
		actor->z = actor->floorz;
	}
  if (actor->reactiontime)
    actor->reactiontime--;

  if(!(actor->flags & MF_PEASANT))
  {
	if(actor->disable_npc == false)
	actor->disable_npc = true;
  }

  if (actor->threshold) { /* modify target threshold */
    if (!actor->target || actor->target->health <= 0)
      actor->threshold = 0;
    else
      actor->threshold--;
  }

  /* turn towards movement direction if not there yet
   * killough 9/7/98: keep facing towards target if strafing or backing out
   */

  if (actor->strafecount)
    A_FaceTarget(actor);
  else if (actor->movedir < 8)
    {
      int delta = (actor->angle &= (7<<29)) - (actor->movedir << 29);
      if (delta > 0)
        actor->angle -= ANG90/2;
      else
        if (delta < 0)
          actor->angle += ANG90/2;
    }

  if (!actor->target || !(actor->target->flags&MF_SHOOTABLE))
    {
      if (!P_LookForTargets(actor,true)) // look for a new target
  P_SetMobjState(actor, actor->info->spawnstate); // no new target
      return;
    }

  // do not attack twice in a row
  if (actor->flags & MF_JUSTATTACKED)
    {
      actor->flags &= ~MF_JUSTATTACKED;
      if (gameskill != sk_nightmare && !fastparm)
        P_NewChaseDir(actor);
      return;
    }

  // check for melee attack
  if (actor->info->meleestate && P_CheckMeleeRange(actor))
    {
      if (actor->info->attacksound)
        S_StartSound(actor, actor->info->attacksound);
      P_SetMobjState(actor, actor->info->meleestate);
      /* killough 8/98: remember an attack
      * cph - DEMOSYNC? */
      if (!actor->info->missilestate)
  actor->flags |= MF_JUSTHIT;
      return;
    }

  // check for missile attack
  if (actor->info->missilestate)
    if (!(gameskill < sk_nightmare && !fastparm && actor->movecount))
      if (P_CheckMissileRange(actor))
        {
          P_SetMobjState(actor, actor->info->missilestate);
          actor->flags |= MF_JUSTATTACKED;
          return;
        }

  if (!actor->threshold) {
    if (!mbf_features)
      {   /* killough 9/9/98: for backward demo compatibility */
  if (netgame && !P_CheckSight(actor, actor->target) &&
      P_LookForPlayers(actor, true))
    return;
      }
    /* killough 7/18/98, 9/9/98: new monster AI */
    else if (help_friends && P_HelpFriend(actor))
      return;      /* killough 9/8/98: Help friends in need */
    /* Look for new targets if current one is bad or is out of view */
    else if (actor->pursuecount)
      actor->pursuecount--;
    else {
  /* Our pursuit time has expired. We're going to think about
   * changing targets */
  actor->pursuecount = BASETHRESHOLD;

  /* Unless (we have a live target
   *         and it's not friendly
   *         and we can see it)
   *  try to find a new one; return if sucessful */

  if (!(actor->target && actor->target->health > 0 &&
        ((comp[comp_pursuit] && !netgame) ||
         (((actor->target->flags ^ actor->flags) & MF_FRIEND ||
     (!(actor->flags & MF_FRIEND) && monster_infighting)) &&
    P_CheckSight(actor, actor->target))))
      && P_LookForTargets(actor, true))
        return;

  /* (Current target was good, or no new target was found.)
   *
   * If monster is a missile-less friend, give up pursuit and
   * return to player, if no attacks have occurred recently.
   */

  if (!actor->info->missilestate && actor->flags & MF_FRIEND) {
    if (actor->flags & MF_JUSTHIT)          /* if recent action, */
      actor->flags &= ~MF_JUSTHIT;          /* keep fighting */
    else if (P_LookForPlayers(actor, true)) /* else return to player */
      return;
  }
    }
  }

  if (actor->strafecount)
    actor->strafecount--;

  // chase towards player
  if (--actor->movecount<0 || !P_SmartMove(actor))
    P_NewChaseDir(actor);

  // make active sound
  if (actor->info->activesound && P_Random(pr_see)<3)
    S_StartSound(actor, actor->info->activesound);

  switch (actor->info->activesound)		//kaiser
    {
    case 0:
      return;

    case sfx_agrac1:
    case sfx_agrac2:
    case sfx_agrac3:
    case sfx_agrac4:
      sound = sfx_agrac1 + P_Random(pr_agrdact)%4;
      break;

    default:
      sound = actor->info->activesound;
      break;
    }
}

//
// A_FaceTarget
//
void A_FaceTarget(mobj_t *actor)
{
  if (!actor->target)
    return;
  actor->flags &= ~MF_AMBUSH;
  actor->angle = R_PointToAngle2(actor->x, actor->y,
                                 actor->target->x, actor->target->y);
  if (actor->target->flags & MF_SHADOW)
    { // killough 5/5/98: remove dependence on order of evaluation:
      int t = P_Random(pr_facetarget);
      actor->angle += (t-P_Random(pr_facetarget))<<21;
    }
}

int     TRACEANGLE = 0xc000000;

void A_Scream(mobj_t *actor)
{
  int sound;
  switch (actor->info->deathsound)
    {
    case 0:
      return;

    default:
      sound = actor->info->deathsound;
      break;
    }

  //added 10-13-05	-kaiser
  actor->flags & MF_BOSS ? S_StartSound(NULL, sound) : S_StartSound(actor, sound);
}

void A_XScream(mobj_t *actor)
{
  S_StartSound(actor, sfx_slop);
}

void A_Pain(mobj_t *actor)
{
	int sound;
	if (actor->info->painsound)
	{
      switch (actor->info->painsound)
        {
        case sfx_pespna:
        case sfx_pespnb:
        case sfx_pespnc:
		case sfx_pespnd:
          sound = sfx_pespna+S_Random()%4;
          break;
		default:
			sound = actor->info->painsound;
	  }
    S_StartSound(actor, sound);
	}
}

void A_Fall(mobj_t *actor)
{
  // actor is on ground, it can be walked over
  actor->flags &= ~MF_SOLID;
  actor->flags &= ~MF_BOSS;	//kaiser
}

//
// A_Explode
//
void A_Explode(mobj_t *thingy)
{
  if(thingy->type != MT_C_MISSILE)
  P_RadiusAttack( thingy, thingy->target, 64 );
  if(!thingy->target || !thingy->target->player)
	  return;
  P_NoiseAlert(thingy->target->player->mo, thingy);
}

void A_PlayerScream(mobj_t *mo)
{
  int sound = sfx_pldeth;  // Default death sound.
  if (gamemode != shareware && mo->health < -50)
    sound = sfx_plxdth;   // IF THE PLAYER DIES LESS THAN -50% WITHOUT GIBBING -kaiser
  S_StartSound(mo, sound);
}

/* cph - MBF-added codepointer functions */

// killough 11/98: kill an object
void A_Die(mobj_t *actor)
{
  P_DamageMobj(actor, NULL, NULL, actor->health);
}

//
// A_Detonate
// killough 8/9/98: same as A_Explode, except that the damage is variable
//

void A_Detonate(mobj_t *mo)
{
  P_RadiusAttack(mo, mo->target, mo->info->damage);
}

//
// killough 9/98: a mushroom explosion effect, sorta :)
// Original idea: Linguica
//

void A_Mushroom(mobj_t *actor)
{
  int i, j, n = actor->info->damage;

  A_Explode(actor);  // First make normal explosion

  // Now launch mushroom cloud
  for (i = -n; i <= n; i += 8)
    for (j = -n; j <= n; j += 8)
      {
  mobj_t target = *actor, *mo;
  target.x += i << FRACBITS;    // Aim in many directions from source
  target.y += j << FRACBITS;
  target.z += P_AproxDistance(i,j) << (FRACBITS+2); // Aim up fairly high
  mo = P_SpawnMissile(actor, &target, MT_MINIMISSILE);  // Launch fireball
  mo->momx >>= 1;
  mo->momy >>= 1;                                   // Slow it down a bit
  mo->momz >>= 1;
  mo->flags &= ~MF_NOGRAVITY;   // Make debris fall under gravity
      }
}

//
// killough 11/98
//
// The following were inspired by Len Pitre
//
// A small set of highly-sought-after code pointers
//

void A_Spawn(mobj_t *mo)
{
  if (mo->state->misc1)
    {
      /* mobj_t *newmobj = */
      P_SpawnMobj(mo->x, mo->y, (mo->state->misc2 << FRACBITS) + mo->z,
      mo->state->misc1 - 1);
      /* CPhipps - no friendlyness (yet)
   newmobj->flags = (newmobj->flags & ~MF_FRIEND) | (mo->flags & MF_FRIEND);
      */
    }
}

void A_Turn(mobj_t *mo)
{
  mo->angle += (unsigned int)(((uint_64_t) mo->state->misc1 << 32) / 360);
}

void A_Face(mobj_t *mo)
{
  mo->angle = (unsigned int)(((uint_64_t) mo->state->misc1 << 32) / 360);
}

void A_Scratch(mobj_t *mo)
{
  mo->target && (A_FaceTarget(mo), P_CheckMeleeRange(mo)) ?
    mo->state->misc2 ? S_StartSound(mo, mo->state->misc2) : (void) 0,
    P_DamageMobj(mo->target, mo, mo, mo->state->misc1) : (void) 0;
}

void A_PlaySound(mobj_t *mo)
{
  S_StartSound(mo->state->misc2 ? NULL : mo, mo->state->misc1);
}

void A_RandomJump(mobj_t *mo)
{
  if (P_Random(pr_randomjump) < mo->state->misc2)
    P_SetMobjState(mo, mo->state->misc1);
}

//
// This allows linedef effects to be activated inside deh frames.
//

void A_LineEffect(mobj_t *mo)
{
  line_t junk = *lines;
  player_t player;
  player_t *oldplayer = mo->player;
  mo->player = &player;
  player.health = 100;
  junk.special = (short)mo->state->misc1;
  if (!junk.special)
    return;
  junk.tag = (short)mo->state->misc2;
  if (!P_UseSpecialLine(mo, &junk, 0))
    P_CrossSpecialLine(&junk, 0, mo);
  mo->state->misc1 = junk.special;
  mo->player = oldplayer;
}

//=================Strife Action Defs===============-kaiser

void P_RandomChaseDirection (mobj_t *actor)
{
	dirtype_t olddir = actor->movedir;
	dirtype_t turnaround = olddir;
	dirtype_t tdir;

	if (turnaround != DI_NODIR)         // find reverse direction
    turnaround ^= 4;

	if (P_Random(pr_newchasedir) & 1)
    {
      for (tdir = DI_EAST; tdir <= DI_SOUTHEAST; tdir++)
        if (tdir != turnaround && (actor->movedir = tdir, P_TryWalk(actor)))
    return;
    }
  else
    for (tdir = DI_SOUTHEAST; tdir != DI_EAST-1; tdir--)
      if (tdir != turnaround && (actor->movedir = tdir, P_TryWalk(actor)))
  return;

	if (turnaround != DI_NODIR)
	{
		actor->movedir = turnaround;
		if (P_TryWalk (actor))
		{
			actor->movecount = P_Random(pr_newchasedir) & 15;
			return;
		}
	}

	actor->movedir = DI_NODIR;	// cannot move
}

void A_Wander (mobj_t *actor)
{
	int delta;
	actor->threshold = 0;

	if(actor->istalking)
		return;
	if(actor->talktics)
		return;
	if (actor->flags & MF_STAND)
		return;

	// turn towards movement direction if not there yet
	if (actor->movedir < DI_NODIR)
	{
		actor->angle &= (angle_t)(7<<29);
		delta = actor->angle - (actor->movedir << 29);
		if (delta > 0)
		{
			actor->angle -= ANG90/2;
		}
		else if (delta < 0)
		{
			actor->angle += ANG90/2;
		}
	}

	if (--actor->movecount < 0 || !P_Move (actor, 0))
	{
		P_RandomChaseDirection (actor);
		actor->movecount += 5;
	}
}

void A_Patrol (mobj_t *actor)
{
	int delta;
	mobj_t *targ = actor->subsector->sector->soundtarget;
	actor->threshold = 0;
	targ = actor->subsector->sector->soundtarget;

	

	if (actor->flags & MF_STAND)
		return;

	if (actor->threshold != 0)
	{
		actor->threshold--;
		return;
	}
	if(actor->istalking)
		return;
	if(actor->talktics)
		return;

	// turn towards movement direction if not there yet
	if (actor->movedir < DI_NODIR)
	{
		actor->angle &= (angle_t)(7<<29);
		delta = actor->angle - (actor->movedir << 29);
		if (delta > 0)
		{
			actor->angle -= ANG90/2;
		}
		else if (delta < 0)
		{
			actor->angle += ANG90/2;
		}
	}

	if (--actor->movecount < 0 || !P_Move (actor, 0))
	{
		P_RandomChaseDirection (actor);
		actor->movecount += 5;
	}
	if (targ && (targ->flags & MF_SHOOTABLE) )
	{
		if (!((actor->flags & MF_FRIEND) != (targ->flags & MF_FRIEND)))
		{
			actor->target = targ;
			if(actor->flags & MF_AMBUSH)
			{
				if(P_CheckSight(actor, actor->target))
					goto hearTarget;
				else return;
			}
			else goto hearTarget;
		}
hearTarget:
		P_SetMobjState(actor, actor->info->seestate);
			if(!(actor->flags & MF_PEASANT))
				actor->disable_npc = true;
	}
}

//
// A_Explode2
//
void A_Explode2(mobj_t *thingy)
{
  P_RadiusAttack( thingy, thingy->target, 144 );
  if(thingy->type != MT_PGRENADE)
	  if(!thingy->target || !thingy->target->player)
	  return;
  P_NoiseAlert(thingy->target->player->mo, thingy);
}

//
// A_Explode3
//
void A_Explode3(mobj_t *thingy)
{
  P_RadiusAttack( thingy, thingy->target, 96 );
  if(!thingy->target || !thingy->target->player)
	  return;
  P_NoiseAlert(thingy->target->player->mo, thingy);
  
}


void A_Chant(mobj_t *actor)
{
	S_StartSound(actor, sfx_chant);
}

//===========================================================================
// A_Stand allows a mobj to remain in its spawnstate until player fires
// a weapon other than the fist.
// An mobj will not respond until it is hit, or a weapon is fired. -kaiser
//===========================================================================
void A_Stand(mobj_t *actor)		//kaiser
{
	mobj_t *targ;

	actor->threshold = 0;	// any shot will wake up
	targ = actor->subsector->sector->soundtarget;

	if (targ && (targ->flags & MF_SHOOTABLE) )
	{
		if ((actor->flags & MF_FRIEND) != (targ->flags & MF_FRIEND)/* ||
			gamemap == 3 ||
			gamemap == 34*/)	//moved to mobj_c...kaiser
		{
			actor->target = targ;
			if (actor->flags & MF_AMBUSH)
			{
				if(P_CheckSight(actor, actor->target))
				{
					actor->threshold = 10;
					P_SetMobjState(actor, actor->info->seestate);
					if(!(actor->flags & MF_PEASANT))
						actor->disable_npc = true;
				}
				return;
			}
			else
			{
				goto hearTarget;
			}
		}
		else
		{
			if (P_LookForTargets(actor, true))
			{
hearTarget:
				P_SetMobjState(actor, actor->info->seestate);
				if(!(actor->flags & MF_PEASANT))
				{
					actor->disable_npc = true;
				}
				actor->flags |= MF_NOALERT;
				P_SetTarget(&actor->target, targ);
				return;
			}
		}
	}
	if(actor->type != MT_TURRET)
	{
  		if (!(actor->flags & MF_STAND) && P_Random(pr_stand) < 40)
	{
	  P_SetMobjState(actor, actor->info->spawnstate + 4);
	}
	if (P_Random(pr_stand) < 30)
	{
		P_SetMobjState(actor, actor->info->spawnstate + (P_Random(pr_stand) & 1) + 1);
	}
  }
}

void A_QuestUpdate1(mobj_t *tokn)
{
	player_t *player = &players[consoleplayer];
	mobj_t* mo;
	int quest = MT_TOKEN_QUEST1 - 1 + tokn->info->speed;
	mo = P_SpawnMobj(player->mo->x,player->mo->y,player->mo->z, quest);
	mo->flags |= MF_BOSS;
	P_TouchSpecialThing(mo,player->mo);
}

void A_ObjectWatch(mobj_t *actor)
{
	mobj_t *targ = actor->subsector->sector->soundtarget;
	targ = actor->subsector->sector->soundtarget;
	if(targ)
	{
		actor->target = targ;
	if (actor->info->seesound)
    {
      int sound;
      
          sound = actor->info->seesound;
    }
  P_SetMobjState(actor, actor->info->seestate);
	}
}

void A_ObjectChase(mobj_t *actor)
{
  if (actor->reactiontime)
    actor->reactiontime--;

  if (actor->threshold) { /* modify target threshold */
    if (!actor->target || actor->target->health <= 0)
      actor->threshold = 0;
    else
      actor->threshold--;
  }

  if (!actor->target || !(actor->target->flags&MF_SHOOTABLE))
    {
      if (!P_LookForTargets(actor,true)) // look for a new target
  P_SetMobjState(actor, actor->info->spawnstate); // no new target
      return;
    }

  // do not attack twice in a row
  if (actor->flags & MF_JUSTATTACKED)
    {
      actor->flags &= ~MF_JUSTATTACKED;
      return;
    }

  // check for missile attack
  if (actor->info->missilestate)
      if (P_CheckMissileRange(actor))
        {
          P_SetMobjState(actor, actor->info->missilestate);
          actor->flags |= MF_JUSTATTACKED;
          return;
        }
}

void A_TurretReFire(mobj_t *actor)
{
  // keep firing unless target got out of sight
  A_FaceTarget(actor);

  if (!actor->target || actor->target->health <= 0
      || !P_CheckSight(actor, actor->target))
	P_SetMobjState(actor, actor->info->seestate);
}

void A_BackSound(mobj_t *actor)
{
	int sound;
	// make active sound
  if (actor->info->activesound && P_Random(pr_see)<3)
    S_StartSound(actor, actor->info->activesound);
    sound = actor->info->activesound;

}

void A_Alarm(mobj_t *actor)
{
	if(actor->reactiontime == 0)
	{
		P_SetMobjState(actor,actor->info->spawnstate);
		actor->reactiontime = 60;
	}
	S_StartSound(actor, sfx_alarm);
	actor->reactiontime--;
}

void A_AgrdAttack (mobj_t *actor)
{
	int pitch, angle, damage, t;
	angle = actor->angle;

	if (actor->target == NULL)
		return;

	t = P_Random(pr_agrdatk);
	damage = (P_Random(pr_agrdatk)%5 + 1)*3;
	angle += (t - P_Random(pr_agrdatk)) << 19;
	S_StartSound(actor, sfx_rifle);
	A_FaceTarget(actor);
	pitch = P_AimLineAttack (actor, actor->angle, MISSILERANGE,0);
	PuffType = MT_STRIFEPUFF;
	P_LineAttack (actor, actor->angle, MISSILERANGE, pitch, damage);

	
}

void A_BodyParts (mobj_t *actor)
{
	mobjtype_t bodypart;
	mobj_t *mo;

	if(actor->flags & MF_NOBLOOD)
		bodypart = MT_JUNK;
	else
		bodypart = MT_MEAT;
	
	
  mo = P_TossUpMobj(actor, bodypart);
  P_SetMobjState(mo, mo->info->spawnstate+(S_Random()%20));
}

void A_BurnScream(mobj_t *actor)	//kaiser
{
  S_StartSound(actor, sfx_burnme);
}

void A_DropBurnFlesh(mobj_t *actor)
{
	P_TossUpMobj(actor,MT_BURNDROP);
}

void A_DropItem(mobj_t *actor)
{
	P_DropItem(actor);
}

void A_BeGone(mobj_t *actor)
{
	S_StartSound(actor, sfx_pgrdth);	//temp
}

void A_ShadowOn(mobj_t *actor)
{
	actor->flags |= MF_TRANSLUCENT;
}

void A_ShadowOff(mobj_t *actor)
{
	actor->flags &= ~MF_TRANSLUCENT;
}

void A_PeasAttack(mobj_t *actor)
{
  if (!actor->target)
    return;
  A_FaceTarget(actor);
  if (P_CheckMeleeRange(actor))
    {
      int damage = ((P_Random(pr_sargattack)%7)+1)*2;
	  S_StartSound(actor, sfx_meatht);
      P_DamageMobj(actor->target, actor, actor, damage);
    }
}

void A_PeasFall(mobj_t *actor)
{
	mobj_t *mo;
	int i;

	for(i = 0; i < 16; i++)
	{
	mo = P_SpawnMobj(actor->x, actor->y,
		actor->z+(actor->height>>1), MT_BLOOD);
	if (mo)
	{
		mo->momx = (S_Random()-128)<<11;
		mo->momy = (S_Random()-128)<<11;
		mo->momz = FRACUNIT*10+(S_Random()<<10);
		mo->target = actor;
	}
}
	if ((S_Random() % 5) == 0)
	{
		A_Pain(actor);
		actor->health--;
	}
	if (actor->health <= 0)
	{
		P_SetMobjState (actor, actor->info->deathstate);
	}
}

void A_AguardPain(mobj_t *actor)
{
	P_NoiseAlert(actor, actor);
  if (actor->info->painsound)
  {
    S_StartSound(actor, actor->info->painsound);
  }
}

void A_PgrdAttack(mobj_t *actor)
{
  if (!actor->target)
    return;
  A_FaceTarget(actor);
  if (P_CheckMeleeRange(actor))
    {
      int damage = ((P_Random(pr_sargattack)%10)+1)*4;
      P_DamageMobj(actor->target, actor, actor, damage);
    }
}

void A_PgrdShoot(mobj_t* actor)
{
  int i, bangle, slope;

  if (!actor->target)
    return;
  S_StartSound(actor, sfx_pgrdat);
  A_FaceTarget(actor);
  bangle = actor->angle;
  slope = P_AimLineAttack(actor, bangle, MISSILERANGE, 0); /* killough 8/2/98 */
  for (i=0; i<3; i++)
    {  // killough 5/5/98: remove dependence on order of evaluation:
      int t = P_Random(pr_sposattack);
      int angle = bangle + ((t - P_Random(pr_sposattack))<<20);
      int damage = ((P_Random(pr_sposattack)%5)+1)*3;
	  PuffType = MT_STRIFEPUFF3;
      P_LineAttack(actor, angle, MISSILERANGE, slope, damage);
    }
}

void A_ReavShoot(mobj_t* actor)
{
  int i, bangle, slope;

  if (!actor->target)
    return;
  S_StartSound(actor, sfx_reavat);
  A_FaceTarget(actor);
  bangle = actor->angle;
  slope = P_AimLineAttack(actor, bangle, MISSILERANGE, 0); /* killough 8/2/98 */
  for (i=0; i<3; i++)
    {  // killough 5/5/98: remove dependence on order of evaluation:
      int t = P_Random(pr_sposattack);
      int angle = bangle + ((t - P_Random(pr_sposattack))<<20);
      int damage = ((P_Random(pr_sposattack)%5)+1)*3;
	  PuffType = MT_STRIFEPUFF;
      P_LineAttack(actor, angle, MISSILERANGE, slope, damage);
    }
}

void A_ReavAttack(mobj_t *actor)
{
  if (!actor->target)
    return;
  A_FaceTarget(actor);
  if (P_CheckMeleeRange(actor))
    {
      int damage = ((P_Random(pr_sargattack)%10)+1)*4;
      P_DamageMobj(actor->target, actor, actor, damage);
	  S_StartSound(actor, sfx_revbld);
    }
}

void A_StalkerThinker(mobj_t *actor)
{
	actor->flags |= (MF_INFLOAT|MF_FLOAT);
	if (actor->flags&MF_NOGRAVITY)
	{
		if (actor->ceilingz - actor->height > actor->z)
		{
			P_SetMobjState(actor, S_STLK_FALL1);
		}
	}
	else
	{	
		P_SetMobjState(actor, S_STLK_FRUN1);
	}
}

void A_StalkerDrop(mobj_t *actor)
{
	actor->flags &= ~(MF_NOGRAVITY|MF_INFLOAT);
	S_StartSound(actor, sfx_spdwlk);
	actor->info->painchance = 0;
}

void A_StalkerAttack(mobj_t *actor)
{
 if (!actor->target)
    return;
  A_FaceTarget(actor);
  if (actor->flags&MF_NOGRAVITY)
		{
			P_SetMobjState(actor, S_STLK_FALL1);
		}
  else
  {
  if (P_CheckMeleeRange(actor))
    {
      int damage=(S_Random() & 7) * 2;
      P_DamageMobj(actor->target, actor, actor, damage);
    }
  }
}

void A_StalkerSetLook(mobj_t *actor)
{
	if (actor->flags&MF_NOGRAVITY)
	{
		P_SetMobjState(actor, S_STLK_STND2);	//ceiling
	}
	else
	{
		P_SetMobjState(actor, S_STLK_STND3);	//floor
	}
}

void A_StalkerWalk(mobj_t *actor)
{
	S_StartSound(actor, sfx_spdwlk);
}



void A_BarrelExplode(mobj_t *thingy)
{
  P_RadiusAttack( thingy, thingy->target, 64 );
  P_NoiseAlert(thingy->target, thingy);
}

void A_ActorRefire(mobj_t *actor)	//janis
{
	if (!actor->target)
		return;
	// keep firing unless target got out of sight
	A_FaceTarget(actor);

	if (S_Random() < 30)
		return;

	//Random attacks, could attack twice or more in a row. - kaiser
	if (!actor->target || actor->target->health <= 0
      || !P_CheckSight(actor, actor->target) || S_Random() < 40)
	{
		P_SetMobjState(actor, actor->info->seestate);
	}
}

void A_SentinelAttack(mobj_t *actor)	//janis/kaiser
{
	int i;
	int    an;
	mobj_t *missile;
	mobj_t *trail;

	if(!actor->target)
		return;

	A_FaceTarget(actor);
	actor->momx = 0;
	actor->momy = 0;
	//P_SpawnMissile(actor, actor->target, MT_L_LASER);
	missile = P_SpawnMissile(actor,actor->target,MT_L_LASER);
	an = missile->angle >> ANGLETOFINESHIFT;
	if (missile && (missile->momx || missile->momy))
	{
		for (i = 8; i > 1; i--)
		{
			trail = P_SpawnMobj(missile->x + FixedMul(missile->radius * i, finecosine[an]),
			missile->y + FixedMul(missile->radius * i, finesine[an]),
			missile->z + (missile->momz / 4 * i), MT_R_LASER);
			if (trail)
			{
				P_SetTarget(&trail->target, trail);
				trail->momx = missile->momx;
				trail->momy = missile->momy;
				trail->momz = missile->momz;
				P_CheckMissileSpawn(trail);
			}
		}
		missile->z += missile->momz >> 2;
	}
}

void A_Levitate(mobj_t *actor)
{
	/*
	my own version of a_levitate. now useless	-kaiser
	  int r;

	r = S_Random();
  if ( r<64 )
    actor->momz += FRACUNIT;
  else if (r<128)
    actor->momz -= FRACUNIT;*/
	int minz, maxz;

	if (actor->threshold || actor->flags & MF_INFLOAT)
		return;

	maxz = actor->ceilingz - actor->height - 16*FRACUNIT;
	minz = actor->floorz + 96*FRACUNIT;
	if (minz > maxz)
	{
		minz = maxz;
	}
	if (minz < actor->z)
	{
		actor->momz -= FRACUNIT;
	}
	else
	{
		actor->momz += FRACUNIT;
	}
	actor->reactiontime = (minz == actor->z) ? 4 : 0;
}

static boolean P_CheckCrusaderRange(mobj_t *actor)
{
  mobj_t *pl = actor->target;

  return
    pl && !(actor->flags & pl->flags & MF_FRIEND) &&
    (P_AproxDistance(pl->x-actor->x, pl->y-actor->y) <
     CRUSADERRANGE - 20*FRACUNIT + pl->info->radius) &&
    P_CheckSight(actor, actor->target);
}

#define CRUSADERSPREAD1		(ANG90 / 32)
#define CRUSADERSPREAD2		(ANG90 / 16)
void A_CrusaderAttack(mobj_t *actor)	//based on Janis' Vavoom code. re-written by kaiser
{
	mobj_t *missile1;
	mobj_t *missile2;
	mobj_t *missile3;
	int	an;
	if(!actor->target)
		return;

	if (P_CheckCrusaderRange(actor))
	{
		A_FaceTarget(actor);
		actor->angle = actor->angle - ANG90/4;
		P_SpawnFixedZMissile(actor->z + 40, actor,actor->target,MT_C_FLAME);
	}
	else
	{
		if (P_CheckMissileRange(actor))
		{
			A_FaceTarget(actor);
			missile1 = P_SpawnFixedZMissile(actor->z + 56*FRACUNIT, actor,actor->target,MT_C_MISSILE);
			actor->angle -= CRUSADERSPREAD1;
			missile1->momz += FRACUNIT;
			missile1->angle += CRUSADERSPREAD1;
			an = missile1->angle >> ANGLETOFINESHIFT;
			missile1->momx = FixedMul(missile1->info->speed, finecosine[an]);
			missile1->momy = FixedMul(missile1->info->speed, finesine[an]);

			missile2 = P_SpawnFixedZMissile(actor->z + 40*FRACUNIT, actor,actor->target,MT_C_MISSILE);
			actor->angle += CRUSADERSPREAD2;
			missile2->angle += CRUSADERSPREAD2;
			an = missile1->angle >> ANGLETOFINESHIFT;
			missile2->momx = FixedMul(missile2->info->speed, finecosine[an]);
			missile2->momy = FixedMul(missile2->info->speed, finesine[an]);

			missile3 = P_SpawnFixedZMissile(actor->z + 40*FRACUNIT, actor,actor->target,MT_C_MISSILE);
			actor->angle -= CRUSADERSPREAD2;
			missile3->angle += CRUSADERSPREAD2;
			an = missile3->angle >> ANGLETOFINESHIFT;
			missile3->momx = FixedMul(missile3->info->speed, finecosine[an]);
			missile3->momy = FixedMul(missile3->info->speed, finesine[an]);
			
			actor->reactiontime += 15;
		}
		P_SetMobjState(actor, actor->info->seestate);
	}
}

void A_CrusaderLeft(mobj_t *actor)
{
	mobj_t *misl;
	int an;

	actor->angle = actor->angle + ANG90/8;
	misl = P_SpawnFixedZMissile(actor->z + 48*FRACUNIT, actor,actor->target,MT_C_FLAME);
	if (misl)
	{
		misl->momz = 1 * 35;
		misl->angle = actor->angle;
		an = misl->angle >> ANGLETOFINESHIFT;
		misl->momx = FixedMul(misl->info->speed, finecosine[an]);
		misl->momy = FixedMul(misl->info->speed, finesine[an]);
	}
}

void A_CrusaderRight(mobj_t *actor)
{
	mobj_t *misl;
	int an;

	actor->angle = actor->angle - ANG90/8;
	misl = P_SpawnFixedZMissile(actor->z + 48*FRACUNIT, actor,actor->target,MT_C_FLAME);
	if (misl)
	{
		misl->momz = 1 * 35;
		misl->angle = actor->angle;
		an = misl->angle >> ANGLETOFINESHIFT;
		misl->momx = FixedMul(misl->info->speed, finecosine[an]);
		misl->momy = FixedMul(misl->info->speed, finesine[an]);
	}
}

void A_ActorRepeatFire(mobj_t *actor)
{
	if (!actor->target)
		return;
	// keep firing unless target got out of sight
	A_FaceTarget(actor);

	if (!actor->target || actor->target->health <= 0
      || !P_CheckSight(actor, actor->target))
	{
		P_SetMobjState(actor, actor->info->seestate);
	}

	else
		P_SetMobjState(actor, actor->info->missilestate);
}

void A_ProgrammerAttack(mobj_t *actor)
{
	mobj_t *mo;
	if(!actor->target)
		return;

	A_FaceTarget(actor);
	mo = P_SpawnMissile(actor->target,actor->target,MT_SIGIL_A_GROUND);
	if(mo)
	{
		mo->z = mo->floorz;
		mo->momx = 0;
		mo->momy = 0;
		mo->momz = 0;
		mo->flags |= MF_BOSS;	//give the ground the boss flag.
	}
}

void A_ProgrammerDie(mobj_t *actor)
{
	P_TossUpMobj(actor, MT_PROGRAMMERBASE);
	actor->target->player->weaponowned[wp_sigil] = true;
//	P_UpdateSigilWeapon(actor->target->player);
	actor->target->player->pendingweapon = wp_sigil;
	S_StartSound(actor->target->player,sfx_siglup);
	//P_SetMessage(actor->target->player,"You've Killed the Programmer!");
}

void A_ProgrammerGrab(mobj_t *actor)
{
	angle_t	angle;
	if (!actor->target)
    return;
  A_FaceTarget(actor);
  if (P_CheckMeleeRange(actor))
    {
      int damage = ((P_Random(pr_sargattack)%10)+1)*4;
	  actor->target->reactiontime = 20;
      P_DamageMobj(actor->target, actor, actor, damage);
	  angle = actor->target->angle+ANG180;
	  angle >>= ANGLETOFINESHIFT;
	  actor->target->momx += FixedMul(8*FRACUNIT, finecosine[angle]);
	  actor->target->momy += FixedMul(8*FRACUNIT, finesine[angle]);
    }
}

void A_ProgrammerSpecial(mobj_t *actor)
{
	//G_DeferedInitNew(gameskill, 10);
	G_ExitLevel(10,-999);
}

void A_ChainTend(mobj_t *actor)
{
	int an;
	int i;
	an = actor->angle >> ANGLETOFINESHIFT;
	for(i = 4; i > 1; i--)
	{
		P_SpawnMobj(actor->x - FixedMul(actor->radius * i, finecosine[an]),
		actor->y - FixedMul(actor->radius * i, finesine[an]),
		actor->z - (actor->momz / 4 * i), MT_CHAINSHOT);
	}

	S_StartSound(actor,actor->info->activesound);


}

void A_PriestAttack(mobj_t *actor)
{
  if (!actor->target)
    return;
  A_FaceTarget(actor);
  P_SpawnMissile(actor, actor->target, MT_HOOKSHOT);
}

void A_SpectureMelee(mobj_t *actor)
{
  if (!actor->target)
    return;
  A_FaceTarget(actor);
  if (P_CheckMeleeRange(actor))
    {
      int damage = ((P_Random(pr_sargattack)%10)+1)*12;
      P_DamageMobj(actor->target, actor, actor, damage);
    }
}

void A_PriestDie(mobj_t *actor)
{
	mobj_t *mo;
	int i;

	for(i = 0; i < 4; i++)
	{
	mo = P_SpawnMobj(actor->x, actor->y,
		actor->z+(actor->height>>1), MT_BLOOD);
	if (mo)
	{
		mo->momx = (S_Random()-128)<<11;
		mo->momy = (S_Random()-128)<<11;
		mo->momz = FRACUNIT*10+(S_Random()<<10);
		mo->target = actor;
	}
	}
	mo = P_SpawnMobj(actor->x,actor->y,actor->z+actor->height,MT_SPECTURE_E);
	mo->momz += 2*FRACUNIT;
	mo->angle = actor->angle;
}

void A_SpectureAttack(mobj_t *actor)
{
	mobj_t *mo;
	if(!actor->target)
		return;
	A_FaceTarget(actor);

	mo = P_SpawnMissile(actor, actor->target, MT_SIGIL_SB_SHOT);
	mo->flags |= MF_BOSS;
	mo->flags &= ~MF_GHOST;
}

void A_NodeChunk(mobj_t *actor)
{
	P_TossUpMobj(actor,MT_NODE);
}

void A_HeadChunk(mobj_t *actor)
{
	P_TossUpMobj(actor,MT_SPECTUREHEAD);
}

void A_BishopAttack(mobj_t *actor)
{
	mobj_t *mo;
	A_FaceTarget(actor);
	if(!actor->target)
		return;
	mo = P_SpawnFixedZMissile(actor->z + 64*FRACUNIT,actor,actor->target,MT_SEEKMISSILE);
		if(mo)
		{
			P_SetTarget(&mo->tracer, actor->target);
			mo->momz -= 2*FRACUNIT;
		}
}

int SEEKANGLE = 0xc000000;

void A_Seeker(mobj_t *actor)
{
  angle_t       exact;
  fixed_t       dist;
  fixed_t       slope;
  mobj_t        *dest;

  // adjust direction
  dest = actor->tracer;

  if (!dest || dest->health <= 0)
    return;

  // change angle
  exact = R_PointToAngle2(actor->x, actor->y, dest->x, dest->y);

  if (exact != actor->angle) {
    if (exact - actor->angle > 0x80000000)
      {
        actor->angle -= TRACEANGLE;
        if (exact - actor->angle < 0x80000000)
          actor->angle = exact;
      }
    else
      {
        actor->angle += TRACEANGLE;
        if (exact - actor->angle > 0x80000000)
          actor->angle = exact;
      }
  }

  exact = actor->angle>>ANGLETOFINESHIFT;
  actor->momx = FixedMul(actor->info->speed, finecosine[exact]);
  actor->momy = FixedMul(actor->info->speed, finesine[exact]);

  // change slope
  dist = P_AproxDistance(dest->x - actor->x, dest->y - actor->y);

  dist = dist / actor->info->speed;

  if (dist < 1)
    dist = 1;

  slope = (dest->z+40*FRACUNIT - actor->z) / dist;

  if (slope < actor->momz)
    actor->momz -= FRACUNIT/8;
  else
    actor->momz += FRACUNIT/8;
}

void A_DropInqArm(mobj_t *actor)
{
	P_TossUpMobj(actor, MT_INQARM);
}

void A_InqShootGun(mobj_t *actor)
{
	int i, bangle, slope;

  if (!actor->target)
    return;
  if(actor->z != actor->floorz)
	return;
  S_StartSound(actor, sfx_reavat);
  A_FaceTarget(actor);
  bangle = actor->angle;
  slope = P_AimLineAttack(actor, bangle, MISSILERANGE, 0);
  for (i=0; i<3; i++)
    {
      int t = P_Random(pr_sposattack);
      int angle = bangle + ((t - P_Random(pr_sposattack))<<20);
      int damage = ((P_Random(pr_sposattack)%5)+1)*3;
	  PuffType = MT_STRIFEPUFF;
      P_LineAttack(actor, angle, MISSILERANGE, slope, damage);
    }
}

void A_InqThinker(mobj_t *actor)
{
	if(actor->z != actor->floorz)
	return;
	S_StartSound(actor, sfx_inqact);

	if(!P_CheckSight(actor, actor->target))
		return;

	if (!P_CheckCrusaderRange(actor))
	{
		P_SetMobjState(actor,actor->info->missilestate + 4);
	}
	else
	{
		P_SetMobjState(actor,actor->info->missilestate + 1);
	}
}

void A_InqGrenade(mobj_t *actor)
{
	mobj_t *mo;
	mobj_t *mo2;
	int an;
	if(actor->z != actor->floorz)
	return;

	A_FaceTarget(actor);
	mo = P_SpawnFixedZMissile(actor->z + 76*FRACUNIT,actor,actor->target,MT_INQGRENADE);
	mo->angle += (ANG90 / 32);
	an = mo->angle >> ANGLETOFINESHIFT;
	mo->momx = FixedMul(mo->info->speed, finecosine[an]);
	mo->momy = FixedMul(mo->info->speed, finesine[an]);
	mo->momz += 15*FRACUNIT;	//weeeeeeeeeee!!!!

	mo2 = P_SpawnFixedZMissile(actor->z + 76*FRACUNIT,actor,actor->target,MT_INQGRENADE);
	mo2->angle -= (ANG90 / 32);
	an = mo2->angle >> ANGLETOFINESHIFT;
	mo2->momx = FixedMul(mo2->info->speed, finecosine[an]);
	mo2->momy = FixedMul(mo2->info->speed, finesine[an]);
	mo2->momz += 4*FRACUNIT;
}

void A_InqFlyCheck(mobj_t *actor)
{
	int faller;
	if(!actor->target)
		return;

	faller = abs(actor->momz);

	if(actor->z != actor->target->z && actor->z < actor->target->z )
	{
		A_FaceTarget(actor);
		if (actor->z + actor->height + 54*FRACUNIT < actor->ceilingz)
		P_SetMobjState(actor,actor->info->missilestate +7);
		else actor->z = actor->floorz;	//don't let him jump while under a low ceiling
	}
	else if(actor->z != actor->target->z && actor->z != actor->floorz
		&& (faller > 21*FRACUNIT))	//fly when he is falling
	{
		A_FaceTarget(actor);
		P_SetMobjState(actor,actor->info->missilestate +7);
	}
	else
		return;
}

#define INQHOVER	(8*FRACUNIT)
void A_InqTakeOff(mobj_t *actor)
{
	angle_t an;
	mobj_t *dest;
	int dist;

	if (!actor->target)
    return;

	dest = actor->target;

	actor->flags |= MF_NOGRAVITY;
	A_FaceTarget(actor);
	actor->z += 64*FRACUNIT;
	an = actor->angle >> ANGLETOFINESHIFT;
	actor->momx = FixedMul(INQHOVER, finecosine[an]);
	actor->momy = FixedMul(INQHOVER, finesine[an]);
	dist = P_AproxDistance(dest->x-actor->x, dest->y-actor->y);
	dist = dist/(INQHOVER);
	if(dist < 1)
	{
		dist = 1;
	}
	actor->momz = (dest->z-actor->z)/dist;
	actor->reactiontime = 60;
}

void A_InqFly(mobj_t *actor)
{
  int moverx;
  int movery;
  actor->reactiontime--;
  moverx = abs(actor->momx);
  movery = abs(actor->momy);
  S_StartSound(actor, sfx_inqjmp);
  actor->target->z > actor->target->z ? actor->z++ : actor->z--;
  if(actor->momx == 0 && actor->momy == 0 || actor->z <= actor->floorz
	  || moverx > INQHOVER && movery > INQHOVER || actor->reactiontime < 0
	  || actor->z >= actor->target->z + actor->height / 6)
  {
	  P_SetMobjState(actor,actor->info->seestate + 1);
	  actor->flags &= ~MF_NOGRAVITY;
	  actor->reactiontime = 0;
  }
}

void A_InqGrenadeExplode(mobj_t *thingy)
{
  P_RadiusAttack( thingy, thingy->target, 128 );
}

void A_SpectureBReveal(mobj_t *actor)
{
	mobj_t *mo;

	mo = P_SpawnMobj(actor->x,actor->y,actor->z+actor->height,MT_SPECTURE_B);
	mo->momz += 2*FRACUNIT;
	mo->angle = actor->angle;
}

void A_SpectureDReveal(mobj_t *actor)
{
	mobj_t *mo;

	mo = P_SpawnMobj(actor->x,actor->y,actor->z+actor->height,MT_SPECTURE_D);
	mo->momz += 2*FRACUNIT;
	mo->angle = actor->angle;
}

void A_SpectureCAttack(mobj_t *actor)
{
	mobj_t *spread;
	angle_t angle;
	int i;

	if(!actor->target)
		return;

	A_FaceTarget(actor);

	for(i = 0; i < 20; i++)
	{
		spread = P_SpawnMissile(actor,actor->target,MT_SIGIL_SC_SHOT);
		spread->angle += ANG180+ANG90;
		spread->flags |= MF_BOSS;
		spread->flags &= ~MF_GHOST;
		angle = spread->angle += i*(ANG4*4);
		angle >>= ANGLETOFINESHIFT;
		spread->momx = FixedMul(spread->info->speed, finecosine[angle]);
		spread->momy = FixedMul(spread->info->speed, finesine[angle]);
	}
	spread = P_SpawnMobj(actor->x,actor->y,actor->z-actor->height,MT_SIGIL_A_ZAP_LEFT);
	spread->flags &= ~MF_NOGRAVITY;
	spread->flags &= ~MF_GHOST;
	spread->flags |= MF_BOSS;
	spread->momx = 1;
	spread->momy = 1;
	P_CheckMissileSpawn(spread);
}

void A_OracleSpecial(mobj_t *actor)
{
	mobj_t *mo;
	thinker_t *think;

	for(think = thinkercap.next; think != &thinkercap;
		think = think->next)
	{
		if(think->function != P_MobjThinker)
		{ // Not a mobj thinker
			continue;
		}
		mo = (mobj_t *)think;
		if((mo->flags&MF_COUNTKILL) && (mo->health > 0) && mo->type == MT_SPECTURE_C)
		{
			P_LookForTargets(mo,true);
			P_SetMobjState(mo,mo->info->seestate);
		}
	}
}

void A_ActorSpecialEvent(mobj_t* mo)
{
  thinker_t *th;
  line_t   junk;
  player_t *player = &players[consoleplayer];

  if(mo->type == MT_SPECTURE_B || mo->type == MT_SPECTURE_C
	  || mo->type == MT_SPECTURE_D || mo->type == MT_SPECTURE_E)
  {

	player->weaponowned[wp_sigil] = true;
	P_UpdateSigilWeapon(player);
	player->pendingweapon = wp_sigil;
	S_StartSound(player,sfx_siglup);
		
	switch(mo->type)
	{
	case MT_SPECTURE_B:
			A_QuestUpdate1(mo);
			player->objectives = 74;
			S_StartVoice(player->mo,"VOC74");
			break;
	case MT_SPECTURE_C:
			A_QuestUpdate1(mo);
			if (player->quest & (1 << (21)))
			{ // If the Bishop is dead, set quest item 22
				player->quest |= 1 << (22);
			}
			if (!(player->quest & (1 << (25))))
			{
				player->objectives = 87;
				S_StartVoice(player->mo,"VOC87");
			}
			else
			{
				player->objectives = 85;
				S_StartVoice(player->mo,"VOC85");
			}
			break;
	case MT_SPECTURE_D:
			//player->quest |= 1 << (24);
			A_QuestUpdate1(mo);
		if (!(player->quest & (1 << (25))))
		{
			player->objectives = 79;
			S_StartVoice(player->mo,"VOC79");
		}
		else
		{
			player->objectives = 106;
			S_StartVoice(player->mo,"VOC106");
		}
			break;
	case MT_SPECTURE_E:
			//player->quest |= 1 << (24);
			A_QuestUpdate1(mo);
			player->accuracy += 10;
			player->stamina += 10;
			if(player->weaponowned[wp_sigil] = true && player->sigilowned == sl_sigil5)
			{
				player->objectives = 85;
				S_StartVoice(player->mo,"VOC85");
			}
			else
			{
				player->objectives = 83;
				S_StartVoice(player->mo,"VOC83");
			}
			break;
	}
  }
  else if(mo->type == MT_SPECTURE_A)
  {
  A_Fall(mo);

  for (th = thinkercap.next ; th != &thinkercap ; th=th->next)
    if (th->function == P_MobjThinker)
      {
        mobj_t *mo2 = (mobj_t *) th;
        if (mo2 != mo && mo2->type == mo->type && mo2->health > 0)
          return;
      }

  junk.tag = 999;
  EV_DoFloor(&junk,lowerFloorToLowest);
  }

  else if(mo->type == MT_CRUSADER)
  {
  A_Fall(mo);

  for (th = thinkercap.next ; th != &thinkercap ; th=th->next)
    if (th->function == P_MobjThinker)
      {
        mobj_t *mo2 = (mobj_t *) th;
        if (mo2 != mo && mo2->type == mo->type && mo2->health > 0)
          return;
      }

  junk.tag = 667;
  EV_DoFloor(&junk,lowerFloorToLowest);
  }

  else if(mo->type == MT_SUBENTITY)
  {
	  A_Fall(mo);

	for (th = thinkercap.next ; th != &thinkercap ; th=th->next)
    if (th->function == P_MobjThinker)
      {
        mobj_t *mo2 = (mobj_t *) th;
        if (mo2 != mo && mo2->type == mo->type && mo2->health > 0)
          return;
      }
	if(finaletype == noend)
	{
		if(player->quest & (1 << 24) || player->quest & (1 << 26))	//added 9-04-05 -kaiser
			finaletype = happy;
		else
		finaletype = sad;
	}
	D_StartEndPic();
  }
}

void A_SpectureDAttack(mobj_t *actor)
{
	mobj_t *mo;
	if(!actor->target)
		return;
	A_FaceTarget(actor);

	mo = P_SpawnMissile(actor, actor->target, MT_SIGIL_SD_SHOT);
	mo->flags |= MF_BOSS;
	mo->flags &= ~MF_GHOST;
}

void A_ZombieInSpecialSector(mobj_t *actor)
{
	sector_t*   sector;

  sector = actor->subsector->sector;

  if (actor->z != sector->floorheight)
    return;

  if(actor->z == sector->floorheight)
  {
  switch(sector->special)
  {
  case 15:
	  P_DamageMobj (actor, NULL, NULL, 1000000);
	  break;
  case 18:
	  P_MoveObjectInSector(actor);
	  break;
  }
  }
}

void A_SpawnZombie(mobj_t *actor)
{
	P_SpawnMobj(actor->x,actor->y,actor->z,MT_ZOMBIE);
}

void A_SpectureEAttack(mobj_t *actor)
{
	mobj_t *mo;
	if(!actor->target)
		return;
	A_FaceTarget(actor);

	mo = P_SpawnMissile(actor, actor->target, MT_SIGIL_SE_SHOT);
	mo->flags |= MF_BOSS;
	mo->flags &= ~MF_GHOST;
}

void A_EntityChooseAttack(mobj_t *actor)
{
	int r;
	if(!actor->target)
		return;

	r = S_Random();
	A_FaceTarget(actor);

	if(r < 64)
		A_SpectureCAttack(actor);
	else if(r < 96)
		A_ProgrammerAttack(actor);
	else if(r < 128)
		A_SpectureDAttack(actor);
	else if(r < 160)
		A_SpectureEAttack(actor);
	else
		A_SpectureAttack(actor);
}

//global variables for entity's position	-kaiser
int	entitydestx = 0;
int	entitydesty = 0;
int	entitydestz = 0;

void A_EntityBorn(mobj_t *actor)
{
	mobj_t *mo;

	mo = P_SpawnMobj(actor->x,actor->y,actor->z+actor->height,MT_ENTITY);
	if(mo)
	{
		mo->momz = 5*FRACUNIT;
		entitydestx = mo->x;
		entitydesty = mo->y;
		entitydestz = (mo->z+actor->height);
	}
}

void A_SubEntitySpawn(mobj_t *actor)
{
	mobj_t *sub;
	/*doom_printf("Okay...Entity's spawnpoint pos is:\n"\
	"x is %d,\n"\
	"y is %d,\n"\
	"z is %d", entitydestx >> FRACBITS,
	entitydesty >> FRACBITS,
	entitydestz >> FRACBITS);*/

	//of course this isn't 100% accurate, but it does its job		-kaiser
	sub = P_SpawnMobj(entitydestx-(240)*FRACUNIT,
		entitydesty-(S_Random()&192*FRACUNIT),entitydestz,MT_SUBENTITY);

	sub = P_SpawnMobj(entitydestx-(S_Random()&192)*FRACUNIT,entitydesty+(240)*FRACUNIT,
		entitydestz,MT_SUBENTITY);

	sub = P_SpawnMobj(entitydestx+(S_Random()&192)*FRACUNIT,entitydesty-(240)*FRACUNIT,
		entitydestz,MT_SUBENTITY);

	sub->angle = actor->angle;
}

void A_DegninExplode(mobj_t *thingy)
{
	line_t   junk;
	switch(thingy->type)
	{
	case MT_DEGNINORE:
			A_Scream(thingy);
			P_RadiusAttack2( thingy, thingy->target, 128 );
			P_NoiseAlert(thingy, thingy);
		break;
	case MT_FIELDGUARD:
			junk.tag = thingy->subsector->sector->tag;
			/*doom_printf("sector tag has %i..correct?",junk.tag);*/
			EV_DestoryLineShield(&junk,0);
		break;
	}
}

void A_QuestExplode(mobj_t *actor)
{
	mobj_t *mo;
	fixed_t x;
	fixed_t y;

	x = actor->x+((S_Random()&127)-64)*FRACUNIT;
	y = actor->y+((S_Random()&127)-64)*FRACUNIT;
	mo = P_SpawnMobj(x,y,actor->z,MT_JUNK);
	if(mo)
	{
		P_SetMobjState(mo,S_GRENADE_HE_X2);
		mo->flags |= MF_NOGRAVITY;
		mo->momz += FRACUNIT;
		S_StartSound(mo,sfx_explod);
	}
}

void A_CloseShop(mobj_t *shop)
{
//	mobj_t* mo;
	line_t junk;
	player_t *player = &players[consoleplayer];
	if(!shop->target)
		return;
	if (shop->target && shop->target->player)
	{
		P_NoiseAlert(shop->target, shop);
	}
	junk.tag = 999;
	junk.special = 16209;
	P_CrossSpecialLine(&junk,0,player->mo);
	//mo = P_SpawnMobj(shop->x, shop->y,shop->z,MT_TOKEN_SHOPCLOSE);
	//P_TouchSpecialThing(mo,player->mo);
}

void A_ClearSoundTarget(mobj_t *actor)
{
	actor->subsector->sector->soundtarget = NULL;
}

void A_CrystalExplode(mobj_t *thingy)
{
	sector_t *sector;
	P_RadiusAttack( thingy, thingy->target, 512 );
	sector = thingy->subsector->sector;
	sector->lightlevel = 0;
	sector->floorheight = P_FindLowestFloorSurrounding(sector);
}

void A_CrystalChuck(mobj_t *actor)
{
	int i;
	for(i = 0; i < 8; i++)
	{
		P_TossUpMobj(actor,MT_RUBBLE7);
	}
}

void A_AcolyteSpecial(mobj_t* mo)
{
  thinker_t *th;
  mobj_t* quest;
  player_t *player = &players[consoleplayer];

  if(mo->type != MT_GUARD8)
	  return;
  A_Fall(mo);

  for (th = thinkercap.next ; th != &thinkercap ; th=th->next)
    if (th->function == P_MobjThinker)
      {
        mobj_t *mo2 = (mobj_t *) th;
        if (mo2 != mo && mo2->type == mo->type && mo2->health > 0)
          return;
      }
	quest = P_SpawnMobj(player->mo->x,player->mo->y,player->mo->z, MT_TOKEN_QUEST7);
	quest->silenceditem = true;
	P_TouchSpecialThing(quest,player->mo);
}

void A_NewWorldWarp(mobj_t* mo)
{
  A_Fall(mo);
  killedprog = true;
  //doom_printf("test");
}

void A_TBeacon(mobj_t* mo)	//took samples from p_telept.c and used it here
{
	mobj_t* thing;
	angle_t an;

	thing = P_SpawnMobj(mo->x, mo->y, ONFLOORZ, MT_REBEL1);
	if(!P_TryMove(thing, thing->x, thing->y, true))
	{
		P_RemoveMobj(thing);
		return;
	}
	mo->flags &= ~MF_SPECIAL;
	thing->threshold = 100;
	thing->target = NULL;
	thing->flags |= MF_FRIEND;
	P_SetMobjState(thing, thing->info->seestate);
	thing->angle = mo->angle;
	an = mo->angle >> ANGLETOFINESHIFT;
	S_StartSound(thing, sfx_telept);
	P_SpawnMobj(thing->x + 20*finecosine[an], thing->y + 20*finesine[an],
		thing->z + 4*FRACUNIT, MT_TFOG);
	if(--mo->health < 0)
	{
		P_RemoveMobj(mo);
	}
}



/***** Start of new functions for Andy Baker's stealth monsters ******/

void P_BecomeVisible(mobj_t* actor)
{
  actor->invisible = false;
  actor->flags &= ~MF_TRANSLUCBITS;
};

void P_IncreaseVisibility(mobj_t* actor)
{
  if (actor->invisible) {
      actor->invisible = false;
      actor->flags |= MF_TRANSLUC25;
  } else switch (actor->flags & MF_TRANSLUCBITS) {
    case MF_TRANSLUC25:
      actor->flags &= ~MF_TRANSLUCBITS;
      actor->flags |= MF_TRANSLUC50;
      break;
    case MF_TRANSLUC50:
      actor->flags &= ~MF_TRANSLUCBITS;
      actor->flags |= MF_TRANSLUC25;
      actor->flags |= MF_TRANSLUC50;
      break;
    case MF_TRANSLUC75:
      actor->flags &= ~MF_TRANSLUCBITS;
      break;
  }
}

void P_DecreaseVisibility(mobj_t* actor)
{
  if (actor->invisible)
    return;     // already invisible

  switch (actor->flags & MF_TRANSLUCBITS) {
    case 0:
      actor->flags &= ~MF_TRANSLUCBITS;
      actor->flags |= MF_TRANSLUC75;
      break;
    case MF_TRANSLUC75:
      actor->flags &= ~MF_TRANSLUCBITS;
      actor->flags |= MF_TRANSLUC50;
      break;
    case MF_TRANSLUC50:
      actor->flags &= ~MF_TRANSLUCBITS;
      actor->flags |= MF_TRANSLUC25;
      break;
    case MF_TRANSLUC25:
      actor->flags &= ~MF_TRANSLUCBITS;
      actor->invisible = true;
  }
}
/***** End of new functions for Andy Baker's stealth monsters ******/

