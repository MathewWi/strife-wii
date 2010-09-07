/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 * $Id: p_doors.c,v 1.6.2.2 2002/07/20 18:08:36 proff_fs Exp $
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
 *   Door animation code (opening/closing)
 *
 *-----------------------------------------------------------------------------*/

static const char
rcsid[] = "$Id: p_doors.c,v 1.6.2.2 2002/07/20 18:08:36 proff_fs Exp $";

#include "doomstat.h"
#include "p_spec.h"
#include "p_tick.h"
#include "s_sound.h"
#include "sounds.h"
#include "r_main.h"
#include "dstrings.h"
#include "d_deh.h"  // Ty 03/27/98 - externalized
#include "lprintf.h"

///////////////////////////////////////////////////////////////
//
// Door action routines, called once per tick
//
///////////////////////////////////////////////////////////////
static char *SlideDoorNames[] =	//int Texture names for sliding doors..mainly for closing sounds -kaiser
	{
		"DORALN01",
		"DORCHN01",
		"DORCRG01",
		"DORIRS01",
		"DORQTR01",
		"DORSTN01",
		"SIGLDR01"
	};
static int slideDoorSounds[7] =
	{
		sfx_airlck,
		sfx_drchno,
		sfx_drsmto,
		sfx_airlck,
		sfx_airlck,
		sfx_drston,
		sfx_drlmto
	};

//simplified/made static 10-12-05	returns the sound being played depending on
//certain door texture -kaiser
static int GetDoorSound(line_t* line, boolean open)
{
	int sound;
	int i, j, k;
	char namebuf[9];
	int texNum[3];
	char *texNames[4] = {"DORML0%i","DORMS0%i","DORWL0%i","DORWS0%i"};
	int texSounds[8] = {sfx_drlmto,sfx_drsmto,sfx_drlwud,sfx_drlwud,sfx_drlmtc,sfx_drsmtc,
	sfx_drlwud,sfx_drswud};
	int doorNums[4] = {9, 7, 4, 8};
	texNum[0] = sides[line->sidenum[0]].toptexture;
    texNum[1] = sides[line->sidenum[0]].midtexture;
    texNum[2] = sides[line->sidenum[0]].bottomtexture;
	for(i = 0; i < 3; i++)
	{
		if(texNum[i] == 0)
		{
			sound = open ? sfx_drsmto : sfx_drsmtc;
		}
	}
	for(k = 0; k < 4; k++)
	{
		for(i = 1; i < doorNums[k]; i++)
		{
			sprintf(namebuf, texNames[k], i);
			for(j = 0; j < 3; j++)
			{
				if(texNum[j] == R_TextureNumForName(namebuf))
				{
					sound = open ? texSounds[k] : texSounds[k + 4];
				}
			}
		}
	}
  if(!sound)
  {
	  sound = open ? sfx_drsmto : sfx_drsmtc;
  }
	return sound;
}

// My own version of the Sliding Door. Implented directly in T_VerticalDoor	-kaiser
/////////////////////////////////////////////////////////
//////	P_SpawnSlidingDoor	/////////////////////////////
/////////////////////////////////////////////////////////

static void P_SpawnSlidingDoor(vldoor_t* door)
{
	int doorNames[7];
	int doorSounds[7] =
	{
		sfx_airlck,
		sfx_drchnc,
		sfx_drsmtc,
		sfx_airlck,
		sfx_airlck,
		sfx_drston,
		sfx_drlmtc
	};
	int i, closesound, texTop;
	texTop = sides[door->line->sidenum[0]].toptexture;
	closesound = sfx_bdcls;	//default for custom slide doors -kaiser
	for(i = 0; i < 7; i++)
	{
		doorNames[i] = R_TextureNumForName(SlideDoorNames[i]);
		if(texTop == doorNames[i])
			closesound = doorSounds[i];
	}
	if(door->topcountdown == 130)
	{
		door->line->flags &= ~ML_BLOCKALL;
		door->line->flags &= ~ML_BLOCKING;
	}
	if(door->topcountdown == 40)
	{
		S_StartSound((mobj_t *)&door->sector->soundorg,closesound);
		door->line->flags |= ML_BLOCKALL;
	}
	//use topcountdown to determine the frames. Next frame is expected to be the next pic
	//in the index. Pics must be in order.
	if(door->topcountdown > 40 && door->topcountdown <= 160)
	{
		if((160 - door->topcountdown / 5 - 128) <= 7)
		{
			sides[door->line->sidenum[0]].midtexture =
				texTop + (160 - door->topcountdown / 5 - 128);
			sides[door->line->sidenum[1]].midtexture =
				texTop + (160 - door->topcountdown / 5 - 128);
		}
	}
	else if(door->topcountdown < 40 && door->topcountdown >= 10)
	{
		if(160 + door->topcountdown / 5 - 128 - 34 >= 0)
		{
			sides[door->line->sidenum[0]].midtexture =
				texTop+ 160 + door->topcountdown / 5 - 128 - 34;
			sides[door->line->sidenum[1]].midtexture =
				texTop+ 160 + door->topcountdown / 5 - 128 - 34;	
		}
	}
}

//
// T_VerticalDoor
//
// Passed a door structure containing all info about the door.
// See P_SPEC.H for fields.
// Returns nothing.
//
// jff 02/08/98 all cases with labels beginning with gen added to support
// generalized line type behaviors.

void T_VerticalDoor (vldoor_t* door)
{
  result_e  res;
  // Is the door waiting, going up, or going down?
  //animation frames for linedefs 207 and 144. each frames moves on to the next texture
  //in the current slot. -kaiser
  switch(door->direction)
  {
    case 0:
		if(!strifedemo)
		{
		switch(door->line->special)
		{
		case 207:	//EV_SlidingDoor
		case 144:	//EV_SLidingDoorNoTag
		case 229:
		//10-10-05 moved this code here. It was causing the other doors to not lower
		//when a thing was under it!
		if(door->sector->thinglist != NULL)	//wait for the player to get out of the way
											//before closing - kaiser
		{
			door->topcountdown = 125;
		}
			//updated 10-10-05	moved sliding door code into its own function	-kaiser
			P_SpawnSlidingDoor(door);
		break;
		}
		}
      // Door is waiting
      if (!--door->topcountdown)  // downcount and check
      {
        switch(door->type)
        {
		  case instantRaise:	//kaiser
			  door->direction = -1;
			  break;
          case blazeRaise:
          case genBlazeRaise:
            door->direction = -1; // time to go back down
            S_StartSound((mobj_t *)&door->sector->soundorg,sfx_bdcls);
            break;

          case normal:
          case genRaise:
            door->direction = -1; // time to go back down
            S_StartSound((mobj_t *)&door->sector->soundorg,GetDoorSound(door->line,false));	//kaiser
            break;

          case close30ThenOpen:
          case genCdO:
            door->direction = 1;  // time to go back up
            S_StartSound((mobj_t *)&door->sector->soundorg,GetDoorSound(door->line,true));	//kaiser
            break;

          case genBlazeCdO:
            door->direction = 1;  // time to go back up
            S_StartSound((mobj_t *)&door->sector->soundorg,sfx_bdopn);
            break;

          default:
            break;
        }
      }
      break;

    case 2:
      // Special case for sector type door that opens in 5 mins
      if (!--door->topcountdown)  // 5 minutes up?
      {
        switch(door->type)
        {
          case raiseIn5Mins:
            door->direction = 1;  // time to raise then
            door->type = normal;  // door acts just like normal 1 DR door now
            S_StartSound((mobj_t *)&door->sector->soundorg,GetDoorSound(door->line,true));	//kaiser
            break;

          default:
            break;
        }
      }
      break;

    case -1:
      // Door is moving down
      res = T_MovePlane
            (
              door->sector,
              door->speed,
              door->sector->floorheight,
              false,
              1,
              door->direction
            );

      /* killough 10/98: implement gradual lighting effects */
      if (door->lighttag && door->topheight - door->sector->floorheight)
        EV_LightTurnOnPartway(door->line,
                              FixedDiv(door->sector->ceilingheight -
                                       door->sector->floorheight,
                                       door->topheight -
                                       door->sector->floorheight));

      // handle door reaching bottom
      if (res == pastdest)
      {
        switch(door->type)
        {
          // regular open and close doors are all done, remove them
          case blazeRaise:
		  case instantRaise:	//kaiser
          case blazeClose:
		  case instantClose:	//kaiser
          case genBlazeRaise:
          case genBlazeClose:
            door->sector->ceilingdata = NULL;  //jff 2/22/98
            P_RemoveThinker (&door->thinker);  // unlink and free
            // killough 4/15/98: remove double-closing sound of blazing doors
            if (comp[comp_blazing])
              S_StartSound((mobj_t *)&door->sector->soundorg,sfx_bdcls);
					switch(door->line->special)
		{
		case 144:
			door->line->useon = 0;	//player is now allowed to use the manual slide door - kaiser
			break;
		}
            break;

          case normal:
          case close:
          case genRaise:
          case genClose:
            door->sector->ceilingdata = NULL; //jff 2/22/98
            P_RemoveThinker (&door->thinker);  // unlink and free
            break;

          // close then open doors start waiting
          case close30ThenOpen:
            door->direction = 0;
            door->topcountdown = TICRATE*30;
            break;

          case genCdO:
          case genBlazeCdO:
            door->direction = 0;
            door->topcountdown = door->topwait; // jff 5/8/98 insert delay
            break;

          default:
            break;
        }
      }
      /* jff 1/31/98 turn lighting off in tagged sectors of manual doors
       * killough 10/98: replaced with gradual lighting code
       */
      else if (res == crushed) // handle door meeting obstruction on way down
      {
        switch(door->type)
        {
		  case instantClose:		//kaiser
          case genClose:
          case genBlazeClose:
          case blazeClose:
		  
          case close:          // Close types do not bounce, merely wait
            break;

          default:             // other types bounce off the obstruction
            door->direction = 1;
            S_StartSound((mobj_t *)&door->sector->soundorg,GetDoorSound(door->line,true));	//kaiser
            break;
        }
      }
      break;

    case 1:
      // Door is moving up
      res = T_MovePlane
            (
              door->sector,
              door->speed,
              door->topheight,
              false,
              1,
              door->direction
            );

      /* killough 10/98: implement gradual lighting effects */
      if (door->lighttag && door->topheight - door->sector->floorheight)
        EV_LightTurnOnPartway(door->line,
                              FixedDiv(door->sector->ceilingheight -
                                       door->sector->floorheight,
                                       door->topheight -
                                       door->sector->floorheight));

      // handle door reaching the top
      if (res == pastdest)
      {
        switch(door->type)
        {
          
		  case instantRaise:	//kaiser
				door->direction = 0;
            door->topcountdown = 160;	//skip topwait and began the countdown
										//that way there won't be a big delay when the
										//animation starts -kaiser
				break;
		  case blazeRaise:       // regular open/close doors start waiting
          case normal:
          case genRaise:
          case genBlazeRaise:
            door->direction = 0; // wait at top with delay
            door->topcountdown = door->topwait;
            break;

          case close30ThenOpen:  // close and close/open doors are done
          case blazeOpen:
		  
          case open:
          case genBlazeOpen:
          case genOpen:
          case genCdO:
          case genBlazeCdO:
            door->sector->ceilingdata = NULL; //jff 2/22/98
            P_RemoveThinker (&door->thinker); // unlink and free
            break;
		  case instantOpen:	//kaiser
			  door->direction = 0;
			  door->topcountdown = 160;
			  if(door->topcountdown <= 100)
			  {
				  door->sector->ceilingdata = NULL;
				  P_RemoveThinker (&door->thinker);
				  
			  }
			  break;

          default:
            break;
        }

        /* jff 1/31/98 turn lighting on in tagged sectors of manual doors
   * killough 10/98: replaced with gradual lighting code */
      }
      break;
  }
}

///////////////////////////////////////////////////////////////
//
// Door linedef handlers
//
///////////////////////////////////////////////////////////////

//
// EV_DoLockedDoor
//
// Handle opening a tagged locked door
//
// Passed the line activating the door, the type of door,
// and the thing that activated the line
// Returns true if a thinker created
//

boolean GetKeyType(mobj_t* user, int lock)
{
	if (!user->player)
	{
		return false;
	}
	if (!lock)
	{
		return true;
	}

	if (lock <= 32)
	{
		//	Regular keys.
		if (!(user->player->keys & (1 << (lock))))
		{
			S_StartSound(user->player->mo,sfx_noway);
			return false;
		}
	}

	return true;
}

int EV_DoLockedDoor
( line_t* line,
  vldoor_e  type,
  mobj_t* thing )
{
  player_t* p;

  // only players can open locked doors
  p = thing->player;
  if (!p)
    return 0;

  // check type of linedef, and if key is possessed to open it
  switch(line->special)
  {
	case 99:
	case 133:
		if(!GetKeyType(p->mo,key_IDBadge))
		{
			p->message = LOCK_BADGE;
			return 0;
		}
		break;
	case 134:
	case 135:
		if(!GetKeyType(p->mo,key_IDCard))
		{
			p->message = LOCK_ID;
			return 0;
		}
		break;
	case 136:
	case 137:
		if(!GetKeyType(p->mo,key_Passcard))
		{
			p->message = LOCK_PASS;
			return 0;
		}
		break;
	case 152:
	case 162:
		if(!GetKeyType(p->mo,key_BrassKey))
		{
			p->message = LOCK_BRASS;
			return 0;
		}
		break;
	case 153:
	case 163:
		if(!GetKeyType(p->mo,key_SilverKey))
		{
			p->message = LOCK_SILVER;
			return 0;
		}
		break;
	case 151:
	case 164:
		if(!GetKeyType(p->mo,key_GoldKey))
		{
			p->message = LOCK_GOLD;
			return 0;
		}
		break;
	case 223:
		if(!GetKeyType(p->mo,key_MineKey))
		{
			p->message = LOCK_DEFAULT;
			return 0;
		}
		break;
	case 168:
		if(!GetKeyType(p->mo,key_SeveredHand))
		{
			p->message = LOCK_HAND;
			return 0;
		}
	case 167:
		if(!GetKeyType(p->mo,key_SeveredHand))
		{
			p->message = LOCK_HAND;
			return 0;
		}
		break;
	case 191:
		if(!GetKeyType(p->mo,key_MilitaryID))
		{
			p->message = LOCK_DEFAULT;
			return 0;
		}
		break;
/*	case 169:
		if(!GetKeyType(p->mo,key_BaseKey))
		{
			p->message = LOCK_DEFAULT;
			return 0;
		}*/
	case 172:
		if(!GetKeyType(p->mo,key_Power1Key))
		{
			p->message = LOCK_DEFAULT;
			return 0;
		}
		break;
	case 173:
		if(!GetKeyType(p->mo,key_Power2Key))
		{
			p->message = LOCK_DEFAULT;
			return 0;
		}
		break;
	case 176:
		if(!GetKeyType(p->mo,key_Power3Key))
		{
			p->message = LOCK_DEFAULT;
			return 0;
		}
		break;
  }

  // got the key, so open the door
  return EV_DoDoor(line,type);
}


//
// EV_DoDoor
//
// Handle opening a tagged door
//
// Passed the line activating the door and the type of door
// Returns true if a thinker created
//
int EV_DoDoor
( line_t* line,
  vldoor_e  type )
{
  int   secnum,rtn;
  sector_t* sec;
  vldoor_t* door;

  secnum = -1;
  rtn = 0;

  // open all doors with the same tag as the activating line
  while ((secnum = P_FindSectorFromLineTag(line,secnum)) >= 0)
  {
    sec = &sectors[secnum];
    // if the ceiling already moving, don't start the door action
    if (P_SectorActive(ceiling_special,sec)) //jff 2/22/98
        continue;

    // new door thinker
    rtn = 1;
    door = Z_Malloc (sizeof(*door), PU_LEVSPEC, 0);
    P_AddThinker (&door->thinker);
    sec->ceilingdata = door; //jff 2/22/98

    door->thinker.function = T_VerticalDoor;
    door->sector = sec;
    door->type = type;
    door->topwait = VDOORWAIT;
    door->speed = VDOORSPEED;
    door->line = line; // jff 1/31/98 remember line that triggered us
    door->lighttag = 0; /* killough 10/98: no light effects with tagged doors */

    // setup door parameters according to type of door
    switch(type)
    {
      case blazeClose:
        door->topheight = P_FindLowestCeilingSurrounding(sec);
        door->topheight -= 4*FRACUNIT;
        door->direction = -1;
        door->speed = VDOORSPEED * 4;
        S_StartSound((mobj_t *)&door->sector->soundorg,sfx_bdcls);
        break;

      case close:
        door->topheight = P_FindLowestCeilingSurrounding(sec);
        door->topheight -= 4*FRACUNIT;
        door->direction = -1;
        S_StartSound((mobj_t *)&door->sector->soundorg,GetDoorSound(door->line,false));	//kaiser
        break;

      case close30ThenOpen:
        door->topheight = sec->ceilingheight;
        door->direction = -1;
        S_StartSound((mobj_t *)&door->sector->soundorg,GetDoorSound(door->line,false));	//kaiser
        break;

      case blazeRaise:
      case blazeOpen:
        door->direction = 1;
        door->topheight = P_FindLowestCeilingSurrounding(sec);
        door->topheight -= 4*FRACUNIT;
        door->speed = VDOORSPEED * 4;
        if (door->topheight != sec->ceilingheight)
          S_StartSound((mobj_t *)&door->sector->soundorg,sfx_bdopn);
        break;

      case normal:
      case open:
        door->direction = 1;
        door->topheight = P_FindLowestCeilingSurrounding(sec);
        door->topheight -= 4*FRACUNIT;
        if (door->topheight != sec->ceilingheight)
          S_StartSound((mobj_t *)&door->sector->soundorg,GetDoorSound(door->line,true));	//kaiser
        break;

      default:
        break;
    }
  }
  return rtn;
}


//
// EV_VerticalDoor
//
// Handle opening a door manually, no tag value
//
// Passed the line activating the door and the thing activating it
// Returns true if a thinker created
//
// jff 2/12/98 added int return value, fixed all returns
//
int EV_VerticalDoor
( line_t* line,
  mobj_t* thing )
{
  player_t* player;
  int   secnum;
  sector_t* sec;
  vldoor_t* door;

  //  Check for locks
  player = thing->player;

  switch(line->special)
  {
	case 26:
	case 32:
		if(!GetKeyType(player->mo,key_IDBadge))
		{
			player->message = LOCK_BADGE;
			return 0;
		} else line->special = 1;
		break;
	case 28:
	case 33:
		if(!GetKeyType(player->mo,key_IDCard))
		{
			player->message = LOCK_ID;
			return 0;
		} else line->special = 1;
		break;
	case 27:
	case 34:
		if(!GetKeyType(player->mo,key_Passcard))
		{
			player->message = LOCK_PASS;
			return 0;
		} else line->special = 1;
		break;
	case 156:
		if(!GetKeyType(player->mo,key_BrassKey))
		{
			player->message = LOCK_BRASS;
			return 0;
		} else line->special = 1;
		break;
	case 160:
	case 157:
		if(!GetKeyType(player->mo,key_SilverKey))
		{
			player->message = LOCK_SILVER;
			return 0;
		} else line->special = 1;
		break;
	case 159:
	case 158:
		if(!GetKeyType(player->mo,key_GoldKey))
		{
			player->message = LOCK_GOLD;
			return 0;
		} else line->special = 1;
		break;
	case 225:
		if(!GetKeyType(player->mo,key_CatacombKey))
		{
			player->message = LOCK_DEFAULT;
			return 0;
		} else line->special = 1;
		break;
	case 224:
		if(!GetKeyType(player->mo,key_ChapelKey))
		{
			player->message = LOCK_DEFAULT;
			return 0;
		} else line->special = 1;
		break;
	case 217:
		if(!GetKeyType(player->mo,key_CoreKey))
		{
			player->message = LOCK_DEFAULT;
			return 0;
		} else line->special = 1;
		break;
	case 166:
		if(!GetKeyType(player->mo,key_SeveredHand))
		{
			player->message = LOCK_HAND;
			return 0;
		} else line->special = 1;
		break;
	case 221:
		if(!GetKeyType(player->mo,key_MaulerKey))
		{
			player->message = LOCK_DEFAULT;
			return 0;
		} else line->special = 1;
		break;
	case 190:
		if(!GetKeyType(player->mo,key_MilitaryID))
		{
			player->message = LOCK_DEFAULT;
			return 0;
		} else line->special = 1;
		break;
	case 232:
		if(!GetKeyType(player->mo,key_OracleKey))
		{
			player->message = LOCK_DEFAULT;
			return 0;
		} else line->special = 1;
		break;
	case 161:
		if(!GetKeyType(player->mo,key_BrassKey))
		{
			player->message = LOCK_DEFAULT;
			return 0;
		} else line->special = 1;
		break;
	case 169:
		if(!GetKeyType(player->mo,key_BaseKey))
		{
			player->message = LOCK_DEFAULT;
			return 0;
		} else line->special = 1;
		break;
	case 170:
		if(!GetKeyType(player->mo,key_GovsKey))
		{
			player->message = LOCK_DEFAULT;
			return 0;
		}
		else line->special = 1;
		break;
	case 213:
		if(!player->inventory.mobjItem[MT_INV_CHALICE])
		{
			player->message = LOCK_CHALICE;
			return 0;
		}
		else line->special = 1;
		break;


    default:
      break;
  }

  // if the wrong side of door is pushed, give oof sound
  if (line->sidenum[1]==-1)                     // killough
  {
    S_StartSound(player->mo,sfx_oof);           // killough 3/20/98
    return 0;
  }

  // get the sector on the second side of activating linedef
  sec = sides[line->sidenum[1]].sector;
  secnum = sec-sectors;

  /* if door already has a thinker, use it
   * cph 2001/04/05 -
   * Ok, this is a disaster area. We're assuming that sec->ceilingdata
   *  is a vldoor_t! What if this door is controlled by both DR lines
   *  and by switches? I don't know how to fix that.
   * Secondly, original Doom didn't distinguish floor/lighting/ceiling
   *  actions, so we need to do the same in demo compatibility mode.
   */
  door = sec->ceilingdata;
  if (demo_compatibility) {
    if (!door) door = sec->floordata;
    if (!door) door = sec->lightingdata;
  }
  if (door)
  {
    switch(line->special)
    {
      case  1: // only for "raise" doors, not "open"s
      case 26:
	case 28:
	case 27:
	case 160:
	case 159:
	case 217:
	case 166:
	case 221:
	case 190:
	case 232:
	case 161:
	case 169:
	case 170:
	case 213:
      case  117:
        if (door->direction == -1)
          door->direction = 1;  // go back up
        else
        {
          if (!thing->player)
            return 0;           // JDC: bad guys never close doors

          door->direction = -1; // start going down immediately
        }
        return 1;
    }
  }

  // emit proper sound
  switch(line->special)
  {
    case 117: // blazing door raise
    case 118: // blazing door open
      S_StartSound((mobj_t *)&sec->soundorg,sfx_bdopn);
      break;

    case 1:   // normal door sound
    case 31:
      S_StartSound((mobj_t *)&sec->soundorg,GetDoorSound(line,true));	//kaiser
      break;

    default:  // locked door sound
      S_StartSound((mobj_t *)&sec->soundorg,GetDoorSound(door->line,true));	//kaiser
      break;
  }

  // new door thinker
  door = Z_Malloc (sizeof(*door), PU_LEVSPEC, 0);
  P_AddThinker (&door->thinker);
  sec->ceilingdata = door; //jff 2/22/98
  door->thinker.function = T_VerticalDoor;
  door->sector = sec;
  door->direction = 1;
  door->speed = VDOORSPEED;
  door->topwait = VDOORWAIT;
  door->line = line; // jff 1/31/98 remember line that triggered us

  /* killough 10/98: use gradual lighting changes if nonzero tag given */
  door->lighttag = comp[comp_doorlight] ? 0 : line->tag;

  // set the type of door from the activating linedef type
  switch(line->special)
  {
    case 1:
	case 26:
	case 28:
	case 27:
	case 160:
	case 159:
	case 217:
	case 166:
	case 221:
	case 190:
	case 232:
	case 161:
	case 169:
	case 170:
	case 213:
      door->type = normal;
      break;


	case 32:
	case 33:
	case 34:
	case 156:
	case 157:
	case 158:
	case 225:
	case 224:
      door->type = open;
      line->special = 0;
      break;

    case 117: // blazing door raise
      door->type = blazeRaise;
      door->speed = VDOORSPEED*4;
      break;
    case 118: // blazing door open
      door->type = blazeOpen;
      line->special = 0;
      door->speed = VDOORSPEED*4;
      break;

    default:
      door->lighttag = 0;   // killough 10/98
      break;
  }

  // find the top and bottom of the movement range
  door->topheight = P_FindLowestCeilingSurrounding(sec);
  door->topheight -= 4*FRACUNIT;
  return 1;
}




///////////////////////////////////////////////////////////////
//
// Sector type door spawners
//
///////////////////////////////////////////////////////////////

//
// P_SpawnDoorCloseIn30()
//
// Spawn a door that closes after 30 seconds (called at level init)
//
// Passed the sector of the door, whose type specified the door action
// Returns nothing
//
void P_SpawnDoorCloseIn30 (sector_t* sec)
{
  vldoor_t* door;

  door = Z_Malloc ( sizeof(*door), PU_LEVSPEC, 0);

  P_AddThinker (&door->thinker);

  sec->ceilingdata = door; //jff 2/22/98
  sec->special = 0;

  door->thinker.function = T_VerticalDoor;
  door->sector = sec;
  door->direction = 0;
  door->type = normal;
  door->speed = VDOORSPEED;
  door->topcountdown = 30 * 35;
  door->line = NULL; // jff 1/31/98 remember line that triggered us
  door->lighttag = 0; /* killough 10/98: no lighting changes */
}

//
// P_SpawnDoorRaiseIn5Mins()
//
// Spawn a door that opens after 5 minutes (called at level init)
//
// Passed the sector of the door, whose type specified the door action
// Returns nothing
//
void P_SpawnDoorRaiseIn5Mins
( sector_t* sec,
  int   secnum )
{
  vldoor_t* door;

  door = Z_Malloc ( sizeof(*door), PU_LEVSPEC, 0);

  P_AddThinker (&door->thinker);

  sec->ceilingdata = door; //jff 2/22/98
  sec->special = 0;

  door->thinker.function = T_VerticalDoor;
  door->sector = sec;
  door->direction = 2;
  door->type = raiseIn5Mins;
  door->speed = VDOORSPEED;
  door->topheight = P_FindLowestCeilingSurrounding(sec);
  door->topheight -= 4*FRACUNIT;
  door->topwait = VDOORWAIT;
  door->topcountdown = 5 * 60 * 35;
  door->line = NULL; // jff 1/31/98 remember line that triggered us
  door->lighttag = 0; /* killough 10/98: no lighting changes */
}


//===========================================================
//EV_SlidingDoor
//
//This is just like EV_DoDoor -kaiser
//===========================================================


int EV_SlidingDoor
( line_t* line,
  vldoor_e  type )
{
  int   secnum,rtn,i, j;
  sector_t* sec;
  vldoor_t* door;
  line_t *secline;
  int     texTop;
  int doorSound;

  

  secnum = -1;
  rtn = 0;

  // open all doors with the same tag as the activating line
  while ((secnum = P_FindSectorFromLineTag(line,secnum)) >= 0)
  {
    sec = &sectors[secnum];
    // if the ceiling already moving, don't start the door action
    if (P_SectorActive(ceiling_special,sec)) //jff 2/22/98
        continue;

	for (i = 0; i < sec->linecount; i++)	//counts all lines activated, not just the one
											//the player used. -kaiser
		{
			secline = sec->lines[i];
			if (!secline->tag)
				continue;
    // new door thinker
    rtn = 1;
    door = Z_Malloc (sizeof(*door), PU_LEVSPEC, 0);
    P_AddThinker (&door->thinker);
    sec->ceilingdata = door; //jff 2/22/98

    door->thinker.function = T_VerticalDoor;
    door->sector = sec;
    door->type = type;
    door->topwait = VDOORWAIT;
    door->speed = VDOORSPEED;
    door->line = secline; // effect all lines under sector - kaiser
    door->lighttag = 0; /* killough 10/98: no light effects with tagged doors */
	texTop = sides[secline->sidenum[0]].toptexture;	//began checking for the toptexture
    // setup door parameters according to type of door
    switch(type)
    {

	  case instantClose:
        door->topheight = P_FindLowestCeilingSurrounding(sec);
        door->topheight -= 4*FRACUNIT;
        door->direction = -1;
        door->speed = VDOORSPEED * 256;
        //S_StartSound((mobj_t *)&door->sector->soundorg,sfx_bdcls);
        break;

	  case instantRaise:
      case instantOpen:
        door->direction = 1;
        door->topheight = P_FindLowestCeilingSurrounding(sec);
        door->speed = VDOORSPEED * 256;

		doorSound = sfx_bdopn;
		for(j = 0; j < 7; j++)
		{
			if(texTop == R_TextureNumForName(SlideDoorNames[j]))
			doorSound = slideDoorSounds[j];
		}

		S_StartSound((mobj_t *)&door->sector->soundorg,doorSound);

			sides[secline->sidenum[0]].midtexture = texTop;
			sides[secline->sidenum[1]].midtexture = texTop;
			door->line->flags |= ML_BLOCKALL;
			//sec->special = 19;		unused for now. -kaiser
        break;

      default:
        break;
    }
  }
  }
  return rtn;
}

int EV_SlidingDoorNoTag		//manual sliding door -kaiser
( line_t* line,
  mobj_t* thing )
{
  player_t* player;
  int   secnum;
  sector_t* sec;
  vldoor_t* door;
  line_t *secline;
  int     texTop;
  int		i, j;
  int doorSound;

  player = thing->player;

  if (line->sidenum[1]==-1)   
  {
    S_StartSound(player->mo,sfx_oof); 
    return 0;
  }

  sec = sides[line->sidenum[1]].sector;
  secnum = sec-sectors;
  for (i = 0; i < sec->linecount; i++)
		{
			secline = sec->lines[i];
			if (!secline->special)
				continue;

  door = sec->ceilingdata;
  if (demo_compatibility) {
    if (!door) door = sec->floordata;
    if (!door) door = sec->lightingdata;
  }

  if(secline->useon != 1)		//we are allowed to use it -kaiser
  {
  switch(line->special)
  {
    case 144:
		doorSound = sfx_bdopn;
		for(j = 0; j < 7; j++)
		{
			if(texTop == R_TextureNumForName(SlideDoorNames[j]))
			doorSound = slideDoorSounds[j];
		}
		S_StartSound((mobj_t *)&sec->soundorg,doorSound);
  }

  door = Z_Malloc (sizeof(*door), PU_LEVSPEC, 0);
  P_AddThinker (&door->thinker);
  sec->ceilingdata = door;
  door->thinker.function = T_VerticalDoor;
  door->sector = sec;
  door->direction = 1;
  door->speed = VDOORSPEED;
  door->topwait = VDOORWAIT;
  door->line = secline;
  texTop = sides[secline->sidenum[0]].toptexture;

  door->lighttag = comp[comp_doorlight] ? 0 : line->tag;
  switch(line->special)
  {
    case 144: 
      door->type = instantRaise;
      door->speed = VDOORSPEED*256;
	  sides[secline->sidenum[0]].midtexture = texTop;
			sides[secline->sidenum[1]].midtexture = texTop;
			door->line->flags |= ML_BLOCKALL;
			secline->useon = 1;	//can't use it again until its done closing -kaiser
      break;

    default:
      door->lighttag = 0; 
      break;
  }
  door->topheight = P_FindLowestCeilingSurrounding(sec);
  }
  }
  return 1;
}

//==========================================================================
//
//  EV_DoSplitDoor
//
//==========================================================================
int EV_DoSplitDoor(line_t * line, int ftype, int ctype, int amount)
{
	boolean floor;
	boolean ceiling;
	int secnum;
	sector_t *sec;

	floor = EV_DoPlat(line, ftype, amount);
	secnum = -1;
	while ((secnum = P_FindSectorFromLineTag(line,secnum)) >= 0)
	{
		sec = &sectors[secnum];
			sec->specialdata = 0;
	}
	ceiling = EV_DoDoor(line, ctype);
	return floor || ceiling;
}































///////////////////////////////////////////////////////////////////////////////////////
//don't mind me, just experimenting... Sliding Door code - DOESN'T WORK.. PERIOD
///////////////////////////////////////////////////////////////////////////////////////

/*#define MAXSLIDEDOORS 8
#define SLIDEFRAMES 8
#define SDOORWAIT		35*3
#define SWAITTICS		4

static char animateDoors[MAXSLIDEDOORS][10] =
{
	"SIGLDR0%d",
	"DORSTN0%d",
	"DORQTR0%d",
	"DORCRG0%d",
	"DORCHN0%d",
	"DORIRS0%d",
	"DORALN0%d",
	"\0"
};

static int slideFrames[MAXSLIDEDOORS][SLIDEFRAMES];

void P_InitSlidingDoorFrames(void)
{
	char texname[9];
	int i, j;

	memset (slideFrames, -1, sizeof(slideFrames));

	for (i = 0; i < MAXSLIDEDOORS; ++i)
	{
		if (!animateDoors[i][0])
			break;

		for (j = 0; j < SLIDEFRAMES; ++j)
		{
			sprintf (texname, animateDoors[i], j+1);
			slideFrames[i][j] = R_TextureNumForName (texname);
		}
	}
}

//
// Return index into "slideFrames" array
// for which door type to use

//slightly modified for strife		-kaiser
int P_FindSlidingDoorType(line_t*	line)
{
    int		i;
    int		val;

	val = sides[line->sidenum[0]].toptexture;
    for (i = 0;i < MAXSLIDEDOORS;i++)
    {
	//val = sides[line->sidenum[0]].midtexture;
	if (val == slideFrames[i][0])
	    return i;
    }
	
    return -1;
}

void T_SlidingDoor (slidedoor_t*	door)
{
    switch(door->status)
    {
	case sd_unactive:
		door->frontsector->ceilingdata = NULL;
		P_RemoveThinker(&door->thinker);
		break;

      case sd_opening:
	if (!door->timer--)
	{
	    if (++door->frame == SLIDEFRAMES)
	    {
		// IF DOOR IS DONE OPENING...
		//sides[door->line->sidenum[0]].midtexture = 0;
		//sides[door->line->sidenum[1]].midtexture = 0;
		//door->line->flags &= ML_BLOCKING^0xff;
		door->line1->flags &= ~ML_BLOCKING;
		door->line2->flags &= ~ML_BLOCKING;
					
		if (door->delay == 0)
		{
		    door->frontsector->specialdata = NULL;
		    P_RemoveThinker (&door->thinker);
		    break;
		}
					
		door->timer = SDOORWAIT;
		door->status = sd_waiting;
	    }
	    else
	    {
		// IF DOOR NEEDS TO ANIMATE TO NEXT FRAME...
		door->timer = SWAITTICS;
					
		sides[door->line1->sidenum[0]].midtexture =
				sides[door->line1->sidenum[1]].midtexture =
					slideFrames[door->whichDoorIndex][door->frame];
				sides[door->line2->sidenum[0]].midtexture =
				sides[door->line2->sidenum[1]].midtexture =
					slideFrames[door->whichDoorIndex][door->frame];
	    }
	}
	break;
			
      case sd_waiting:
	// IF DOOR IS DONE WAITING...
	if (!door->timer--)
	{
	    // CAN DOOR CLOSE?
	    if (door->frontsector->thinglist != NULL ||
		door->backsector->thinglist != NULL)
	    {
		door->timer = SDOORWAIT;
		break;
	    }

		if(T_MovePlane(door->frontsector,2048*FRACUNIT,
			door->frontsector->floorheight, false, 1, -1)  == crushed)
		{
			door->timer = SDOORWAIT;
			break;
		}

		T_MovePlane(door->frontsector, 2048*FRACUNIT,door->frontsector->ceilingheight,
		false, 1, 1);
		door->line1->flags |= ML_BLOCKING;
		door->line2->flags |= ML_BLOCKING;
	    door->status = sd_closing;
	    door->timer = SWAITTICS;
	}
	break;
			
      case sd_closing:
	if (!door->timer--)
	{
	    if (--door->frame < 0)
	    {
		// IF DOOR IS DONE CLOSING...
		T_MovePlane(door->frontsector,2048*FRACUNIT,
			door->frontsector->floorheight, false, 1, -1);
		door->frontsector->ceilingdata = NULL;
		//P_RemoveThinker (&door->thinker);
		break;
	    }
	    else
	    {
		// IF DOOR NEEDS TO ANIMATE TO NEXT FRAME...
		door->timer = SWAITTICS;
					
		sides[door->line1->sidenum[0]].midtexture =
				sides[door->line1->sidenum[1]].midtexture =
					slideFrames[door->whichDoorIndex][door->frame];
				sides[door->line2->sidenum[0]].midtexture =
				sides[door->line2->sidenum[1]].midtexture =
					slideFrames[door->whichDoorIndex][door->frame];
	    }
	}
	break;
    }
}

int
EV_SlidingDoor2
( line_t*	line,
  mobj_t*	thing )
{
    sector_t*		sec;
    slidedoor_t*	door;
	int i;
    
    // Make sure door isn't already being animated
    sec = line->backsector;
    door = NULL;
    if (sec->ceilingdata)
    {
	if (!thing->player)
	    return 0;
			
	door = sec->ceilingdata;
	    if (door->status == sd_waiting)
		{
			door->timer = SWAITTICS;
			door->status = sd_closing;
		}
	else
	    return 0;
    }
    
    // Init sliding door vars
	door = Z_Malloc (sizeof(*door), PU_LEVSPEC, 0);
	P_AddThinker (&door->thinker);
	sec->ceilingdata = door;
		
//	door->type = sdt_openAndClose;
	door->whichDoorIndex = P_FindSlidingDoorType(line);

	if (door->whichDoorIndex < 0)
	    I_Error("EV_SlidingDoor: Can't use texture for sliding door!");

	door->line1 = line;
	door->line2 = line;

	for (i = 0; i < sec->linecount; ++i)
	{
		if (sec->lines[i] == line)
			continue;

		if (sides[sec->lines[i]->sidenum[0]].toptexture == sides[line->sidenum[0]].toptexture)
		{
			door->line2 = sec->lines[i];
			break;
		}
	}
	sides[door->line1->sidenum[0]].midtexture = sides[door->line1->sidenum[0]].toptexture;
	sides[door->line2->sidenum[0]].midtexture = sides[door->line2->sidenum[0]].toptexture;
			
	door->status = sd_opening;
	door->speed = VDOORSPEED;
	door->delay = 16*FRACUNIT;
	door->frontsector = sec;
	door->backsector = line->backsector;
	door->thinker.function = T_SlidingDoor;
	door->timer = SWAITTICS;
	door->line1->flags |= ML_BLOCKING;
	door->line2->flags |= ML_BLOCKING;
	door->frame = 0;
	T_MovePlane(door->frontsector,2048*FRACUNIT,
			door->frontsector->ceilingheight, false, 1, 1);
	return 1;
}*/

