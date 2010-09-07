/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 * $Id: p_switch.c,v 1.5.2.1 2002/07/20 18:08:37 proff_fs Exp $
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
 *  Switches, buttons. Two-state animation. Exits.
 *
 *-----------------------------------------------------------------------------*/

static const char
rcsid[] = "$Id: p_switch.c,v 1.5.2.1 2002/07/20 18:08:37 proff_fs Exp $";

#include "doomstat.h"
#include "w_wad.h"
#include "r_main.h"
#include "p_spec.h"
#include "g_game.h"
#include "s_sound.h"
#include "sounds.h"
#include "lprintf.h"
#include "p_map.h"	//kaiser
#include "p_user.h"	//kaiser
#include "p_inter.h"	//kaiser
#include "d_englsh.h"
#include "d_main.h"
#include "st_stuff.h"	//kaiser

//redesigned switchlist_t (need to re-insert this into switches.lmp someday..)	-kaiser
switchlist_t oldalphSwitchList[] =
{
    // Strife switches
	{"SWALIN01","SWALIN02", sfx_swtchn},
	{"SWWALG03",  "SWWALG04", sfx_swtchn},
	{"SWBRIK01","SWBRIK02", sfx_swtchn},
	{"SWCHAP01","SWCHAP02", sfx_swtchn},
	{"SWCIT01","SWCIT02", sfx_swtchn},
	{"SWEXIT01","SWEXIT02", sfx_swtchn},
	{"SWFORC03","SWFORC04", sfx_swtchn},
	{"SWIRON01", "SWIRON02",  sfx_swtchn},
    {"SWIRON03", "SWIRON04",  sfx_swtchn},
	{"SWIRON07", "SWIRON08",  sfx_swtchn},
	{"SWITE03", "SWITE04",  sfx_swtchn},
	{"SWLEV01","SWLEV02", sfx_swtchn},
	{"SWMETL01","SWMETL02", sfx_swtchn},
	{"SWSLD01","SWSLD02", sfx_swtchn},
	{"SWTKBL01", "SWTKBL02",  sfx_swtchn},
	{"SWTRAM03","SWTRAM04", sfx_swtchn},
	{"SWWALG01", "SWWALG02",  sfx_swtchn},
	{"SWFORC01", "SWFORC02", sfx_swtchn},
	{"SWLITE01", "SWLITE02", sfx_swtchn},
	
	{"SWCARD01","SWCARD02", sfx_keycrd},
	{"SWIRON05","SWIRON06", sfx_keycrd},
	{"SWKEY01",  "SWKEY01",   sfx_keycrd},

	{"SWPALM01","SWPALM02", sfx_swscan},

	{"SWPLAS","SWPLAS02", sfx_keycrd},
	{"SWTELP01","SWTELP02", sfx_keycrd},
	{"SWSIGN01","SWSIGN02", sfx_keycrd},
	{"SWTRAM01", "SWTRAM02",  sfx_keycrd},

	{"SWCHN01","SWCHN02", sfx_pulchn},
	{"SWCHNX01","SWCHNX02", sfx_pulchn},
	{"SWRUST01","SWRUST02", sfx_pulchn},
	{"SWWOOD01","SWWOOD02", sfx_pulchn},

	{"SWINDT01","SWINDT02", sfx_swknob},
	{"SWKNOB01","SWKNOB02", sfx_swknob},
	{"SWKNOB03","SWKNOB04", sfx_swknob},

	{"SWTRMG01", "SWTRMG04",  sfx_swston},
	{"SWORC01","SWORC02", sfx_swston},

	{"SWPIP01","SWPIP02", sfx_valve},

	{"DORWS04",	"DORWS05", sfx_swbolt},

	{"SWBRN01", "SWBRN02",	sfx_swtchn},
	{"SWLEV03","SWLEV04",	sfx_swtchn},
	{"SWLEV05", "SWLEV06",	sfx_swtchn},
	{"ALTSW01", "ALTSW02", sfx_swtchn},
	{"CITADL04",	"CITADL05", sfx_swtchn},

	{"GODSCRN1",	"GODSCRN2", sfx_difool},

	{"DORSBK01",	"DORSBK02", sfx_swston},

	//Glass Shatters
    {"WALTEK09", "WALTEKB1",sfx_bglass},
	{"WALTEK10", "WALTEKB1",sfx_bglass},
	{"WALTEK15", "WALTEKB1",sfx_bglass},
	{"COMP01", "COMP04B",sfx_bglass},
	{"COMP05", "COMP12B",sfx_bglass},
	{"COMP09", "COMP12B",sfx_bglass},
	{"COMP13", "COMP12B",sfx_bglass},
	{"COMP17", "COMP20B",sfx_bglass},
	{"COMP21", "COMP20B",sfx_bglass},
	{"COMP25", "COMP20B",sfx_bglass},
	{"COMP29", "COMP20B",sfx_bglass},
	{"COMP33", "COMP50",sfx_bglass},
	{"COMP42", "COMP51",sfx_bglass},
	{"GLASS01", "GLASS02",sfx_bglass},
	{"GLASS03", "GLASS04",sfx_bglass},
	{"GLASS05", "GLASS06",sfx_bglass},
	{"GLASS07", "GLASS08",sfx_bglass},
	{"GLASS09", "GLASS10",sfx_bglass},
	{"GLASS11", "GLASS12",sfx_bglass},
	{"GLASS13", "GLASS14",sfx_bglass},
	{"GLASS15", "GLASS16",sfx_bglass},
	{"GLASS17", "GLASS18",sfx_bglass},
	{"GLASS19", "GLASS20",sfx_bglass},
	{"BRNSCN01", "BRNSCN05",sfx_bglass},
	{"AZWAL21",	"AZWAL22",sfx_bglass},

	{"FORCE01", "SIGLDR08",	sfx_None},	//hack!

    {"\0",      "\0",           0}
};

#define MAXSWITCHES	71		//kaiser
int	switchlist[MAXSWITCHES * 4];
static int max_numswitches;                       // killough
static int numswitches;                           // killough

button_t  buttonlist[MAXBUTTONS];

//
// P_InitSwitchList()		-rewritten by kaiser 10-14-05
//
void P_InitSwitchList(void)
{
	int		i;
	int		index;
	switchlist_t   *alphSwitchList;
  alphSwitchList = oldalphSwitchList;
		
	for (index = 0,i = 0;i < MAXSWITCHES;i++)
	{
		if (!alphSwitchList[i].sound)
		{
			numswitches = index/2;
			switchlist[index] = -1;
			break;
		}
		
		switchlist[index++] = R_TextureNumForName(alphSwitchList[i].name1);
		switchlist[index++] = R_TextureNumForName(alphSwitchList[i].name2);
	}
}

//
// P_StartButton()
//
// Start a button (retriggerable switch) counting down till it turns off.
//
// Passed the linedef the button is on, which texture on the sidedef contains
// the button, the texture number of the button, and the time the button is
// to remain active in gametics.
// No return.
//
void P_StartButton
( line_t*       line,
  bwhere_e      w,
  int           texture,
  int           time )
{
  int           i;

  // See if button is already pressed
  for (i = 0;i < MAXBUTTONS;i++)
    if (buttonlist[i].btimer && buttonlist[i].line == line)
      return;

  for (i = 0;i < MAXBUTTONS;i++)
    if (!buttonlist[i].btimer)    // use first unused element of list
    {
      buttonlist[i].line = line;
      buttonlist[i].where = w;
      buttonlist[i].btexture = texture;
      buttonlist[i].btimer = time;
      buttonlist[i].soundorg = (mobj_t *)&line->frontsector->soundorg;
      return;
    }

  I_Error("P_StartButton: no button slots left!");
}

//
// P_ChangeSwitchTexture()
//
// Function that changes switch wall texture on activation.
//
// Passed the line which the switch is on, and whether its retriggerable.
// If not retriggerable, this function clears the line special to insure that
//
// No return
//
void P_ChangeSwitchTexture
( line_t*       line,
  int           useAgain )
{
  int     texTop;
  int     texMid;
  int     texBot;
  int     i;

  if (!useAgain)
    line->special = 0;

  texTop = sides[line->sidenum[0]].toptexture;
  texMid = sides[line->sidenum[0]].midtexture;
  texBot = sides[line->sidenum[0]].bottomtexture;

  for (i = 0;i < numswitches*2;i++)
  {
    if (switchlist[i] == texTop)     // if an upper texture
    {
	S_StartSound((mobj_t *)&line->frontsector->soundorg,oldalphSwitchList[i/2].sound);
      //S_StartSound(buttonlist->soundorg,switchlist[MAXSWITCHES * 2 + i]);     // switch activation sound
      sides[line->sidenum[0]].toptexture = switchlist[i^1];       //chg texture
	  if(texTop = sides[line->sidenum[1]].toptexture)
		  sides[line->sidenum[1]].toptexture = switchlist[i^1];

      if (useAgain)
        P_StartButton(line,top,switchlist[i],BUTTONTIME);         //start timer

      return;
    }
    else
    {
      if (switchlist[i] == texMid)   // if a normal texture
      {
		  S_StartSound((mobj_t *)&line->frontsector->soundorg,oldalphSwitchList[i/2].sound);
        //S_StartSound(buttonlist->soundorg,switchlist[MAXSWITCHES * 2 + i]);   // switch activation sound
        sides[line->sidenum[0]].midtexture = switchlist[i^1];     //chg texture
		if(texMid = sides[line->sidenum[1]].midtexture)
			sides[line->sidenum[1]].midtexture = switchlist[i^1];

        if (useAgain)
          P_StartButton(line, middle,switchlist[i],BUTTONTIME);   //start timer

        return;
      }
      else
      {
        if (switchlist[i] == texBot) // if a lower texture
        {
			S_StartSound((mobj_t *)&line->frontsector->soundorg,oldalphSwitchList[i/2].sound);
          //S_StartSound(buttonlist->soundorg,switchlist[MAXSWITCHES * 2 + i]); // switch activation sound
          sides[line->sidenum[0]].bottomtexture = switchlist[i^1];//chg texture
		  if(texBot = sides[line->sidenum[1]].bottomtexture)
			sides[line->sidenum[1]].bottomtexture = switchlist[i^1];

          if (useAgain)
            P_StartButton(line, bottom,switchlist[i],BUTTONTIME); //start timer

          return;
        }
      }
    }
  }
}


//==========================================================================
//
//  EV_DestoryLineShield
//
//	Any two sided line in tagged sector will have the blocking flags removed
//	Ability to remove textures are still depending. - kaiser
//==========================================================================
int EV_DestoryLineShield(line_t* line, int amount)
{
	int i;
	int secnum;
	sector_t *sec;
	line_t *secline;

	secnum = -1;
	while ((secnum = P_FindSectorFromLineTag(line,secnum)) >= 0)
	{
		sec = &sectors[secnum];
		for (i = 0; i < sec->linecount; i++)
		{
			secline = sec->lines[i];
			if (secline->special != 148)
				continue;
			if (!(secline->flags & ML_TWOSIDED))
				continue;

			sides[secline->sidenum[0]].midtexture = 0;
			sides[secline->sidenum[1]].midtexture = 0;
			secline->special = 0;
			secline->flags &= ~ML_BLOCKING;
		}
	}
	return true;
}

//
// P_UseSpecialLine
//
//
// Called when a thing uses (pushes) a special line.
// Only the front sides of lines are usable.
// Dispatches to the appropriate linedef function handler.
//
// Passed the thing using the line, the line being used, and the side used
// Returns true if a thinker was created
//
boolean
P_UseSpecialLine
( mobj_t*       thing,
  line_t*       line,
  int           side )
{
	int texTop;
	int texMid;
	int texBot;
	char chr[9];	//kaiser

  texTop = sides[line->sidenum[0]].toptexture;
  texMid = sides[line->sidenum[0]].midtexture;
  texBot = sides[line->sidenum[0]].bottomtexture;

  if(debug1)
  {
	  doom_printf("%i,%i,%i, special %i tag %i y %i floorpic is %i",texTop,texMid,texBot,line->special,
		  line->tag, sides[line->sidenum[0]].rowoffset >> FRACBITS, thing->subsector->sector->floorpic);
  }

  if (side) //jff 6/1/98 fix inadvertent deletion of side test
    return false;

  //jff 02/04/98 add check here for generalized floor/ceil mover
  if (!demo_compatibility)
  {
    // pointer to line function is NULL by default, set non-null if
    // line special is push or switch generalized linedef type
    int (*linefunc)(line_t *line)=NULL;

    // check each range of generalized linedefs
    if ((unsigned)line->special >= GenFloorBase)
    {
      if (!thing->player)
        if ((line->special & FloorChange) || !(line->special & FloorModel))
          return false; // FloorModel is "Allow Monsters" if FloorChange is 0
      if (!line->tag && ((line->special&6)!=6)) //jff 2/27/98 all non-manual
        return false;                         // generalized types require tag
      linefunc = EV_DoGenFloor;
    }
    else if ((unsigned)line->special >= GenCeilingBase)
    {
      if (!thing->player)
        if ((line->special & CeilingChange) || !(line->special & CeilingModel))
          return false;   // CeilingModel is "Allow Monsters" if CeilingChange is 0
      if (!line->tag && ((line->special&6)!=6)) //jff 2/27/98 all non-manual
        return false;                         // generalized types require tag
      linefunc = EV_DoGenCeiling;
    }
    else if ((unsigned)line->special >= GenDoorBase)
    {
      if (!thing->player)
      {
        if (!(line->special & DoorMonster))
          return false;   // monsters disallowed from this door
        if (line->flags & ML_SECRET) // they can't open secret doors either
          return false;
      }
      if (!line->tag && ((line->special&6)!=6)) //jff 3/2/98 all non-manual
        return false;                         // generalized types require tag
      linefunc = EV_DoGenDoor;
    }
    else if ((unsigned)line->special >= GenLockedBase)
    {
      if (!thing->player)
        return false;   // monsters disallowed from unlocking doors
      if (!P_CanUnlockGenDoor(line,thing->player))
        return false;
      if (!line->tag && ((line->special&6)!=6)) //jff 2/27/98 all non-manual
        return false;                         // generalized types require tag

      linefunc = EV_DoGenLockedDoor;
    }
    else if ((unsigned)line->special >= GenLiftBase)
    {
      if (!thing->player)
        if (!(line->special & LiftMonster))
          return false; // monsters disallowed
      if (!line->tag && ((line->special&6)!=6)) //jff 2/27/98 all non-manual
        return false;                         // generalized types require tag
      linefunc = EV_DoGenLift;
    }
    else if ((unsigned)line->special >= GenStairsBase)
    {
      if (!thing->player)
        if (!(line->special & StairMonster))
          return false; // monsters disallowed
      if (!line->tag && ((line->special&6)!=6)) //jff 2/27/98 all non-manual
        return false;                         // generalized types require tag
      linefunc = EV_DoGenStairs;
    }
    else if ((unsigned)line->special >= GenCrusherBase)
    {
      if (!thing->player)
        if (!(line->special & CrusherMonster))
          return false; // monsters disallowed
      if (!line->tag && ((line->special&6)!=6)) //jff 2/27/98 all non-manual
        return false;                         // generalized types require tag
      linefunc = EV_DoGenCrusher;
    }

    if (linefunc)
      switch((line->special & TriggerType) >> TriggerTypeShift)
      {
        case PushOnce:
          if (!side)
            if (linefunc(line))
              line->special = 0;
          return true;
        case PushMany:
          if (!side)
            linefunc(line);
          return true;
        case SwitchOnce:
          if (linefunc(line))
            P_ChangeSwitchTexture(line,0);
          return true;
        case SwitchMany:
          if (linefunc(line))
            P_ChangeSwitchTexture(line,1);
          return true;
        default:  // if not a switch/push type, do nothing here
          return false;
      }
  }

  // Switches that other things can activate.
  if (!thing->player)
  {
    // never open secret doors
    if (line->flags & ML_SECRET)
      return false;

    switch(line->special)
    {
      case 1:         // MANUAL DOOR RAISE
      //jff 3/5/98 add ability to use teleporters for monsters
      case 210:       // silent switch teleporters
      case 209:
        break;

      default:
        return false;
        break;
    }
  }
	if(line->special == 165)
	{
		if(thing->player)
		doom_printf(NOWORK);
		//P_SetMessage(thing->player, NOWORK);
	}

	if(line->special == 205)
	{
		if(thing->player)
		doom_printf(SHAREWARE);
		//P_SetMessage(thing->player, SHAREWARE);
	}

	if(line->special == 251)
	{
		  if(thing->player)
		  {
			  thing->player->message = GetLog(line->tag);
			  S_StartSound (thing->player, sfx_radio);
			  line->special = 0;
		  }
	}

  if (!P_CheckTag(line))  //jff 2/27/98 disallow zero tag on some types
    return false;

  // Dispatch to handler according to linedef type
  switch (line->special)
  {
    // Manual doors, push type with no tag
    case 1:             // Vertical Door

    case 31:            // Manual door open

    case 117:           // Blazing door raise
    case 118:           // Blazing door open
	case 26:
	case 32:
	case 28:
	case 33:
	case 27:
	case 34:
	case 156:
	case 160:
	case 157:
	case 159:
	case 158:
	case 225:
	case 224:
	case 217:
	case 166:
	case 221:
	case 190:
	case 232:
	case 161:
	case 169:
	case 170:
	case 213:
      EV_VerticalDoor (line, thing);
      break;

    // Switches (non-retriggerable)
    case 7:
      // Build Stairs
      if (EV_BuildStairs(line,build8))
        P_ChangeSwitchTexture(line,0);
      break;

    case 9:
      // Change Donut
      if (EV_DoDonut(line))
        P_ChangeSwitchTexture(line,0);
      break;

    case 11:
      /* Exit level
       * killough 10/98: prevent zombies from exiting levels
       */
      if (thing->player && thing->player->health <= 0 && !comp[comp_zombie])
      {
        S_StartSound(thing, sfx_noway);
        return false;
      }

      P_ChangeSwitchTexture(line,0);
	  if(!line->tag)
	  {
		  finaletype = sad;
		  D_StartEndPic();
	  }
	  else
      G_ExitLevel (line->tag/100,-1);
      break;

    case 14:
      // Raise Floor 32 and change texture
      if (EV_DoPlat(line,raiseAndChange,32))
        P_ChangeSwitchTexture(line,0);
      break;

    case 15:
      // Raise Floor 24 and change texture
      if (EV_DoPlat(line,raiseAndChange,24))
        P_ChangeSwitchTexture(line,0);
      break;

    case 18:
      // Raise Floor to next highest floor
      if (EV_DoFloor(line, raiseFloorToNearest))
        P_ChangeSwitchTexture(line,0);
      break;

    case 20:
      // Raise Plat next highest floor and change texture
      if (EV_DoPlat(line,raiseToNearestAndChange,0))
        P_ChangeSwitchTexture(line,0);
      break;

    case 21:
      // PlatDownWaitUpStay
      if (EV_DoPlat(line,downWaitUpStay,0))
        P_ChangeSwitchTexture(line,0);
      break;

    case 23:
      // Lower Floor to Lowest
      if (EV_DoFloor(line,lowerFloorToLowest))
        P_ChangeSwitchTexture(line,0);
      break;

    case 29:
      // Raise Door
      if (EV_DoDoor(line,normal))
        P_ChangeSwitchTexture(line,0);
      break;

    case 41:
      // Lower Ceiling to Floor
      if (EV_DoCeiling(line,lowerToFloor))
        P_ChangeSwitchTexture(line,0);
      break;

	case 199:
      // Lower Ceiling to Floor
      if (EV_DoCeiling(line,lowerToFloor))
	  {
		thing->player->quest |= 1 << (25);
		thing->player->accuracy += 10;
		thing->player->stamina += 10;
		thing->player->maxhealth += 10;
		doom_printf(CONVERTER);
        P_ChangeSwitchTexture(line,0);
	  }
      break;

    case 71:
      // Turbo Lower Floor
      if (EV_DoFloor(line,turboLower))
        P_ChangeSwitchTexture(line,0);
      break;

    case 49:
      // Ceiling Crush And Raise
      if (EV_DoCeiling(line,crushAndRaise))
        P_ChangeSwitchTexture(line,0);
      break;

    case 50:
      // Close Door
      if (EV_DoDoor(line,close))
        P_ChangeSwitchTexture(line,0);
      break;

    case 51:
      /* Secret EXIT
       * killough 10/98: prevent zombies from exiting levels
       */
      if (thing->player && thing->player->health <= 0 && !comp[comp_zombie])
      {
        S_StartSound(thing, sfx_noway);
        return false;
      }

      P_ChangeSwitchTexture(line,0);
      G_SecretExitLevel ();
      break;

    case 55:
      // Raise Floor Crush
      if (EV_DoFloor(line,raiseFloorCrush))
        P_ChangeSwitchTexture(line,0);
      break;

    case 101:
      // Raise Floor
      if (EV_DoFloor(line,raiseFloor))
        P_ChangeSwitchTexture(line,0);
      break;

    case 102:
      // Lower Floor to Surrounding floor height
      if (EV_DoFloor(line,lowerFloor))
        P_ChangeSwitchTexture(line,0);
      break;

    case 103:
      // Open Door
      if (EV_DoDoor(line,open))
        P_ChangeSwitchTexture(line,0);
      break;

    case 111:
      // Blazing Door Raise (faster than TURBO!)
      if (EV_DoDoor (line,blazeRaise))
        P_ChangeSwitchTexture(line,0);
      break;

    case 112:
      // Blazing Door Open (faster than TURBO!)
      if (EV_DoDoor (line,blazeOpen))
        P_ChangeSwitchTexture(line,0);
      break;

	case 233:
		if(EV_DoFloor(line, lowerFloorToLowest) && EV_DoDoor(line,open))
		{
			if(thing->player->communicator == true)
			{
				S_StartVoice(thing, "VOC70");
				thing->player->objectives = 70;
			}
			P_ChangeSwitchTexture(line,0);
		}
      break;

    case 113:
      // Blazing Door Close (faster than TURBO!)
      if (EV_DoDoor (line,blazeClose))
        P_ChangeSwitchTexture(line,0);
      break;

    case 122:
      // Blazing PlatDownWaitUpStay
      if (EV_DoPlat(line,blazeDWUS,0))
        P_ChangeSwitchTexture(line,0);
      break;

    case 127:
      // Build Stairs Turbo 16
      if (EV_BuildStairs(line,turbo16))
        P_ChangeSwitchTexture(line,0);
      break;

    case 131:
      // Raise Floor Turbo
      if (EV_DoFloor(line,raiseFloorTurbo))
        P_ChangeSwitchTexture(line,0);
      break;

    case 140:
      // Raise Floor 512
      if (EV_DoFloor(line,raiseFloor512))
        P_ChangeSwitchTexture(line,0);
      break;

      // killough 1/31/98: factored out compatibility check;
      // added inner switch, relaxed check to demo_compatibility

    default:
      if (!demo_compatibility)
        switch (line->special)
        {
          //jff 1/29/98 added linedef types to fill all functions out so that
          // all possess SR, S1, WR, W1 types

          case 158:
            // Raise Floor to shortest lower texture
            // 158 S1  EV_DoFloor(raiseToTexture), CSW(0)
            if (EV_DoFloor(line,raiseToTexture))
              P_ChangeSwitchTexture(line,0);
            break;

          case 159:
            // Raise Floor to shortest lower texture
            // 159 S1  EV_DoFloor(lowerAndChange)
            if (EV_DoFloor(line,lowerAndChange))
              P_ChangeSwitchTexture(line,0);
            break;

          case 160:
            // Raise Floor 24 and change
            // 160 S1  EV_DoFloor(raiseFloor24AndChange)
            if (EV_DoFloor(line,raiseFloor24AndChange))
              P_ChangeSwitchTexture(line,0);
            break;

          /*case 161:
            // Raise Floor 24
            // 161 S1  EV_DoFloor(raiseFloor24)
            if (EV_DoFloor(line,raiseFloor24))
              P_ChangeSwitchTexture(line,0);
            break;*/

          case 162:
            // Moving floor min n to max n
            // 162 S1  EV_DoPlat(perpetualRaise,0)
            if (EV_DoPlat(line,perpetualRaise,0))
              P_ChangeSwitchTexture(line,0);
            break;

          case 163:
            // Stop Moving floor
            // 163 S1  EV_DoPlat(perpetualRaise,0)
            EV_StopPlat(line);
            P_ChangeSwitchTexture(line,0);
            break;

          case 164:
            // Start fast crusher
            // 164 S1  EV_DoCeiling(fastCrushAndRaise)
            if (EV_DoCeiling(line,fastCrushAndRaise))
              P_ChangeSwitchTexture(line,0);
            break;

/*          case 165:
            // Start slow silent crusher
            // 165 S1  EV_DoCeiling(silentCrushAndRaise)
            if (EV_DoCeiling(line,silentCrushAndRaise))
              P_ChangeSwitchTexture(line,0);
            break;*/

          case 166:
            // Raise ceiling, Lower floor
            // 166 S1 EV_DoCeiling(raiseToHighest), EV_DoFloor(lowerFloortoLowest)
            if (EV_DoCeiling(line, raiseToHighest) ||
                EV_DoFloor(line, lowerFloorToLowest))
              P_ChangeSwitchTexture(line,0);
            break;

/*          case 167:
            // Lower floor and Crush
            // 167 S1 EV_DoCeiling(lowerAndCrush)
            if (EV_DoCeiling(line, lowerAndCrush))
              P_ChangeSwitchTexture(line,0);
            break;*/

          /*case 168:
            // Stop crusher
            // 168 S1 EV_CeilingCrushStop()
            if (EV_CeilingCrushStop(line))
              P_ChangeSwitchTexture(line,0);
            break;*/

          /*case 169:
            // Lights to brightest neighbor sector
            // 169 S1  EV_LightTurnOn(0)
            EV_LightTurnOn(line,0);
            P_ChangeSwitchTexture(line,0);
            break;*/

          /*case 170:
            // Lights to near dark
            // 170 S1  EV_LightTurnOn(35)
            EV_LightTurnOn(line,35);
            P_ChangeSwitchTexture(line,0);
            break;*/

         /* case 171:
            // Lights on full
            // 171 S1  EV_LightTurnOn(255)
            EV_LightTurnOn(line,255);
            P_ChangeSwitchTexture(line,0);
            break;*/

/*          case 172:
            // Start Lights Strobing
            // 172 S1  EV_StartLightStrobing()
            EV_StartLightStrobing(line);
            P_ChangeSwitchTexture(line,0);
            break;*/

          /*case 173:
            // Lights to Dimmest Near
            // 173 S1  EV_TurnTagLightsOff()
            EV_TurnTagLightsOff(line);
            P_ChangeSwitchTexture(line,0);
            break;*/

          /*case 174:
            // Teleport
            // 174 S1  EV_Teleport(side,thing)
            if (EV_Teleport(line,side,thing))
              P_ChangeSwitchTexture(line,0);
            break;*/

          case 175:
            // Close Door, Open in 30 secs
            // 175 S1  EV_DoDoor(close30ThenOpen)
            if (EV_DoDoor(line,close30ThenOpen))
              P_ChangeSwitchTexture(line,0);
            break;

          /*case 189: //jff 3/15/98 create texture change no motion type
            // Texture Change Only (Trigger)
            // 189 S1 Change Texture/Type Only
            if (EV_DoChange(line,trigChangeOnly))
              P_ChangeSwitchTexture(line,0);
            break;*/

          case 203:
            // Lower ceiling to lowest surrounding ceiling
            // 203 S1 EV_DoCeiling(lowerToLowest)
            if (EV_DoCeiling(line,lowerToLowest))
              P_ChangeSwitchTexture(line,0);
            break;

          /*case 204:
            // Lower ceiling to highest surrounding floor
            // 204 S1 EV_DoCeiling(lowerToMaxFloor)
            if (EV_DoCeiling(line,lowerToMaxFloor))
              P_ChangeSwitchTexture(line,0);
            break;*/

          case 209:
            // killough 1/31/98: silent teleporter
            //jff 209 S1 SilentTeleport
            if (EV_SilentTeleport(line, side, thing))
              P_ChangeSwitchTexture(line,0);
            break;

          case 241: //jff 3/15/98 create texture change no motion type
            // Texture Change Only (Numeric)
            // 241 S1 Change Texture/Type Only
            if (EV_DoChange(line,numChangeOnly))
              P_ChangeSwitchTexture(line,0);
            break;

          case 221:
            // Lower floor to next lowest floor
            // 221 S1 Lower Floor To Nearest Floor
            if (EV_DoFloor(line,lowerFloorToNearest))
              P_ChangeSwitchTexture(line,0);
            break;

          case 237:
            // Elevator to current floor
            // 237 S1 Elevator to current floor
            if (EV_DoElevator(line,elevateCurrent))
              P_ChangeSwitchTexture(line,0);
            break;


          // jff 1/29/98 end of added S1 linedef types

          //jff 1/29/98 added linedef types to fill all functions out so that
          // all possess SR, S1, WR, W1 types

          case 78: //jff 3/15/98 create texture change no motion type
            // Texture Change Only (Numeric)
            // 78 SR Change Texture/Type Only
            if (EV_DoChange(line,numChangeOnly))
              P_ChangeSwitchTexture(line,1);
            break;

          /*case 176:
            // Raise Floor to shortest lower texture
            // 176 SR  EV_DoFloor(raiseToTexture), CSW(1)
            if (EV_DoFloor(line,raiseToTexture))
              P_ChangeSwitchTexture(line,1);
            break;*/

/*          case 177:
            // Raise Floor to shortest lower texture
            // 177 SR  EV_DoFloor(lowerAndChange)
            if (EV_DoFloor(line,lowerAndChange))
              P_ChangeSwitchTexture(line,1);
            break;*/

          case 178:
            // Raise Floor 512
            // 178 SR  EV_DoFloor(raiseFloor512)
            if (EV_DoFloor(line,raiseFloor512))
              P_ChangeSwitchTexture(line,1);
            break;

		  case 180:
			  if(EV_DoFloor(line, raise512Change))
				P_ChangeSwitchTexture(line,1);
				break;


          case 190: //jff 3/15/98 create texture change no motion type
            // Texture Change Only (Trigger)
            // 190 SR Change Texture/Type Only
            if (EV_DoChange(line,trigChangeOnly))
              P_ChangeSwitchTexture(line,1);
            break;

          /*case 191:
            // Lower Pillar, Raise Donut
            // 191 SR  EV_DoDonut()
            if (EV_DoDonut(line))
              P_ChangeSwitchTexture(line,1);
            break;*/

          case 192:
            // Lights to brightest neighbor sector
            // 192 SR  EV_LightTurnOn(0)
            EV_LightTurnOn(line,0);
            P_ChangeSwitchTexture(line,1);
            break;

/*          case 193:
            // Start Lights Strobing
            // 193 SR  EV_StartLightStrobing()
            EV_StartLightStrobing(line);
            P_ChangeSwitchTexture(line,1);
            break;*/

/*          case 194:
            // Lights to Dimmest Near
            // 194 SR  EV_TurnTagLightsOff()
            EV_TurnTagLightsOff(line);
            P_ChangeSwitchTexture(line,1);
            break;*/

          case 196:
            // Close Door, Open in 30 secs
            // 196 SR  EV_DoDoor(close30ThenOpen)
            if (EV_DoDoor(line,close30ThenOpen))
              P_ChangeSwitchTexture(line,1);
            break;

          case 205:
            // Lower ceiling to lowest surrounding ceiling
            // 205 SR EV_DoCeiling(lowerToLowest)
            if (EV_DoCeiling(line,lowerToLowest))
              P_ChangeSwitchTexture(line,1);
            break;

          case 206:
            // Lower ceiling to highest surrounding floor
            // 206 SR EV_DoCeiling(lowerToMaxFloor)
            if (EV_DoCeiling(line,lowerToMaxFloor))
              P_ChangeSwitchTexture(line,1);
            break;

          case 210:
            // killough 1/31/98: silent teleporter
            //jff 210 SR SilentTeleport
            if (EV_SilentTeleport(line, side, thing))
              P_ChangeSwitchTexture(line,1);
            break;

/*          case 211: //jff 3/14/98 create instant toggle floor type
            // Toggle Floor Between C and F Instantly
            // 211 SR Toggle Floor Instant
            if (EV_DoPlat(line,toggleUpDn,0))
              P_ChangeSwitchTexture(line,1);
            break;*/

          case 222:
            // Lower floor to next lowest floor
            // 222 SR Lower Floor To Nearest Floor
            if (EV_DoFloor(line,lowerFloorToNearest))
              P_ChangeSwitchTexture(line,1);
            break;

          /*case 230:
            // Raise elevator next floor
            // 230 SR Raise Elevator next floor
            if (EV_DoElevator(line,elevateUp))
              P_ChangeSwitchTexture(line,1);
            break;*/

          /*case 234:
            // Lower elevator next floor
            // 234 SR Lower Elevator next floor
            if (EV_DoElevator(line,elevateDown))
              P_ChangeSwitchTexture(line,1);
            break;*/

          case 238:
            // Elevator to current floor
            // 238 SR Elevator to current floor
            if (EV_DoElevator(line,elevateCurrent))
              P_ChangeSwitchTexture(line,1);
            break;

          case 258:
            // Build stairs, step 8
            // 258 SR EV_BuildStairs(build8)
            if (EV_BuildStairs(line,build8))
              P_ChangeSwitchTexture(line,1);
            break;

          case 259:
            // Build stairs, step 16
            // 259 SR EV_BuildStairs(turbo16)
            if (EV_BuildStairs(line,turbo16))
              P_ChangeSwitchTexture(line,1);
            break;

          // 1/29/98 jff end of added SR linedef types

        }
      break;

    // Buttons (retriggerable switches)
    case 42:
      // Close Door
      if (EV_DoDoor(line,close))
        P_ChangeSwitchTexture(line,1);
      break;

    case 43:
      // Lower Ceiling to Floor
      if (EV_DoCeiling(line,lowerToFloor))
        P_ChangeSwitchTexture(line,1);
      break;

    case 45:
      // Lower Floor to Surrounding floor height
      if (EV_DoFloor(line,lowerFloor))
        P_ChangeSwitchTexture(line,1);
      break;

    case 60:
      // Lower Floor to Lowest
      if (EV_DoFloor(line,lowerFloorToLowest))
        P_ChangeSwitchTexture(line,1);
      break;

    case 61:
      // Open Door
      if (EV_DoDoor(line,open))
        P_ChangeSwitchTexture(line,1);
      break;

    case 62:
      // PlatDownWaitUpStay
      if (EV_DoPlat(line,downWaitUpStay,1))
        P_ChangeSwitchTexture(line,1);
      break;

    case 63:
      // Raise Door
      if (EV_DoDoor(line,normal))
        P_ChangeSwitchTexture(line,1);
      break;

    case 64:
      // Raise Floor to ceiling
      if (EV_DoFloor(line,raiseFloor))
        P_ChangeSwitchTexture(line,1);
      break;

    case 66:
      // Raise Floor 24 and change texture
      if (EV_DoPlat(line,raiseAndChange,24))
        P_ChangeSwitchTexture(line,1);
      break;

    case 67:
      // Raise Floor 32 and change texture
      if (EV_DoPlat(line,raiseAndChange,32))
        P_ChangeSwitchTexture(line,1);
      break;

    case 65:
      // Raise Floor Crush
      if (EV_DoFloor(line,raiseFloorCrush))
        P_ChangeSwitchTexture(line,1);
      break;

    case 68:
      // Raise Plat to next highest floor and change texture
      if (EV_DoPlat(line,raiseToNearestAndChange,0))
        P_ChangeSwitchTexture(line,1);
      break;

    case 69:
      // Raise Floor to next highest floor
      if (EV_DoFloor(line, raiseFloorToNearest))
        P_ChangeSwitchTexture(line,1);
      break;

    case 70:
      // Turbo Lower Floor
      if (EV_DoFloor(line,turboLower))
        P_ChangeSwitchTexture(line,1);
      break;

    case 114:
      // Blazing Door Raise (faster than TURBO!)
      if (EV_DoDoor (line,blazeRaise))
        P_ChangeSwitchTexture(line,1);
      break;

    case 115:
      // Blazing Door Open (faster than TURBO!)
      if (EV_DoDoor (line,blazeOpen))
        P_ChangeSwitchTexture(line,1);
      break;

    case 116:
      // Blazing Door Close (faster than TURBO!)
      if (EV_DoDoor (line,blazeClose))
        P_ChangeSwitchTexture(line,1);
      break;

    case 123:
      // Blazing PlatDownWaitUpStay
      if (EV_DoPlat(line,blazeDWUS,0))
        P_ChangeSwitchTexture(line,1);
      break;

    case 132:
      // Raise Floor Turbo
      if (EV_DoFloor(line,raiseFloorTurbo))
        P_ChangeSwitchTexture(line,1);
      break;

    case 138:
      // Light Turn On
      EV_LightTurnOn(line,255);
      P_ChangeSwitchTexture(line,1);
      break;

    case 139:
      // Light Turn Off
      EV_LightTurnOn(line,35);
      P_ChangeSwitchTexture(line,1);
      break;
	  //====Strife Linedef types==== - kaiser
	case 155:
      if (EV_DoPlat(line,upWaitDownStay,1))
        P_ChangeSwitchTexture(line,1);
      break;

	case 147:
		if (EV_DestoryLineShield(line, 0))
			P_ChangeSwitchTexture(line,0);
      break;

	case 207:
	case 229:
      if (EV_SlidingDoor (line,instantRaise))
		  P_ChangeSwitchTexture(line,1);
      break;

	case 144:
      EV_SlidingDoorNoTag (line, thing);
      break;

	case 40:
	  if (EV_DoFloorAndCeiling(line, lowerFloorToLowest, raiseToHighest))
		  P_ChangeSwitchTexture(line,0);
	  break;

	case 235:
		if(thing->player->sigilowned == sl_sigil5)
		{
	  if (EV_DoFloorAndCeiling(line, lowerFloorToLowest, raiseToHighest))
		  P_ChangeSwitchTexture(line,0);
		}
	  break;

	case 189:
		if(!GetKeyType(thing->player->mo,key_OracleKey))
		{
		thing->player->message = LOCK_DEFAULT;
		}
		else
		{
			if (EV_DoFloorAndCeiling(line, lowerFloorToLowest, raiseToHighest))
			P_ChangeSwitchTexture(line,0);
		}
		break;

	case 219:
		if(!GetKeyType(thing->player->mo,key_RedCrystalKey))
		{
		thing->player->message = LOCK_DEFAULT;
		}
		else
		{
			if (EV_DoFloor(line,lowerFloorToLowest))
			P_ChangeSwitchTexture(line,0);
		}
		break;

	case 220:
		if(!GetKeyType(thing->player->mo,key_BlueCrystalKey))
		{
		thing->player->message = LOCK_DEFAULT;
		}
		else
		{
			if (EV_DoFloor(line,lowerFloorToLowest))
			P_ChangeSwitchTexture(line,0);
		}
		break;

    case 226:
      // Lower Floor to Lowest	add bonus to player's status.	-kaiser
      if (EV_DoFloor(line,lowerFloorToLowest))
		  thing->player->message = "Congradulations! You completed\nthe training area!";
		  if(thing->player->accuracy != 100 && thing->player->stamina != 100
			  && thing->player->maxhealth != 200)
		  {
			thing->player->accuracy += 10;
			thing->player->stamina += 10;
			thing->player->maxhealth += 10;
		  }
        P_ChangeSwitchTexture(line,0);
      break;

	case 666:
	  line->v1->x += 16*FRACUNIT;
	  line->v2->x += 16*FRACUNIT;
	  break;
	
	case 99:
	case 134:
	case 136:
	case 152:
	case 153:
	case 151:
	case 223:
	case 168:
	case 167:
	case 191:
      if (EV_DoLockedDoor (line,blazeOpen,thing))
        P_ChangeSwitchTexture(line,1);
      break;

	case 133:
	case 135:
	case 137:
	case 162:
	case 163:
	case 164:
	case 172:
	case 173:
	case 176:
      if (EV_DoLockedDoor (line,blazeOpen,thing))
        P_ChangeSwitchTexture(line,0);
      break;

	case 234:
		if(thing->player->quest & (1 << TOKEN_QUEST3))
		{
			if (EV_DoDoor(line,normal))
			P_ChangeSwitchTexture(line,1);
		}
		else thing->player->message = NOWORK;
		break;

	case 171:
		if(thing->player->quest & (1 << TOKEN_QUEST11))
		{
			if(EV_DoDoor(line,open))
			P_ChangeSwitchTexture(line,1);
		}
		else thing->player->message = LOCK_PRISON;
		break;
	case 194:
		if(thing->player)
		{
			if(EV_DoDoor(line,open))
			{
				thing->player->quest |= 1 << TOKEN_QUEST13;
				thing->player->message = FREED;
				P_ChangeSwitchTexture(line,0);
			}
		}
		break;
	case 214:
            if (EV_DoFloor(line,lowerFloor30))
              P_ChangeSwitchTexture(line,0);
            break;
	case 181:
            if (EV_DoFloor(line,raise512Change))
              P_ChangeSwitchTexture(line,0);
            break;
	case 177:
		if(thing->player && GetKeyType(thing->player->mo, key_Power3Key))
		{
			if (EV_DoPlat(line,downWaitUpStay,1))
			P_ChangeSwitchTexture(line,1);
		}
		else thing->player->message = LOCK_DEFAULT;
			break;

	case 211:
			sprintf(chr, "VOC%i", line->tag);
			S_StartVoice(thing, chr);
			P_ChangeSwitchTexture(line,0);
		break;
		
  }
  return true;
}
