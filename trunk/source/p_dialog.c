/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 * $Id: p_dialog.c 2005/10/14 0:0:0 kaiser Exp $
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
 *  Strife dialog system and behavior by Janis Legzdinsh and
 *  improvments and updates by Samuel Villarreal
 *  Thing talks, prints out text, plays voice file, and gives choices/replies
 *
 *-----------------------------------------------------------------------------*/

#include "doomstat.h"
#include "p_mobj.h"
#include "p_setup.h"
#include "p_inter.h"
#include "lprintf.h"
#include "m_menu.h"		//kaiser
#include "d_englsh.h"	//kaiser
#include "p_dialog.h"
#include "p_map.h"
#include "s_sound.h"
#include "p_tick.h"
#include "p_maputl.h"
#include "r_main.h"

/*Specs on the strife(non shareware) script lump. One npc makes up one dialog block.
Each block is equal to 1516 bytes. In each blocks contains the following information
NPC info and Choice Info. The NPC info makes up the following items:
SpeakerID;		//	Type of the object (MT_xxx)		(4 bytes)
DropItem;		//	Item dropped when killed		(4 bytes)
CheckItem1;		//	Item 1 to check for jump		(4 bytes)
CheckItem2;		//	Item 2 to check for jump		(4 bytes)
CheckItem3;		//	Item 3 to check for jump		(4 bytes)
JumpToConv;		//	Jump to conversation if have certain item(s)	(4 bytes)
Name[16/4];		//	Name of the character							(16 bytes)
Voice[8/4];		//	Voice to play									(8 bytes)
BackPic[8/4];	//	Picture of the speaker							(8 bytes)
Text[320/4];	//	Message							(320 bytes)

As far as I know, CheckItem2 and 3 are unused. But I could be wrong..
The Choice info makes up the following items:
GiveItem;		//	Item given on success		(4 bytes)	use -1 to skip this check
NeedItem1;		//	Required item 1				(4 bytes)
NeedItem2;		//	Required item 2				(4 bytes)
NeedItem3;		//	Required item 3				(4 bytes)
NeedAmount1;	//	Amount of item 1			(4 bytes)
NeedAmount2;	//	Amount of item 2			(4 bytes)
NeedAmount3;	//	Amount of item 3			(4 bytes)
Text[32/4];		//	Text of the answer			(32 bytes)
TextOK[80/4];	//	Message displayed on success	(80 bytes)
Next;			//	Dialog to go on success, negative values to go		(4 bytes)
						// here immediately
Objectives;		//	Mission objectives, LOGxxxx lump					(4 bytes)
TextNo[80/4];	//	Message displayed on failure (player doesn't		(80 bytes)
						// have needed thing, it haves enough health/ammo,
						// item is not ready, quest is not completed)

Unlike the ones in NPC info, NeedItem2 and 3 and NeedAmount2 and 3 are used
but not often. NeedItem1 MUST be used for coin checking. If this is used then the
following text will be added to the choice it is refering to:
"Choice + for %". Where % is how much NeedAmount1 is specifying. There are some items other
than coins can be used as well. This is where NeedItem2 and NeedAmount 2 comes in.
To use them, NeedItem1 and NeedItem 2 needs to be blank. These must be used and set to
0 if NeedItem2 and NeedAmount2 needs to be used. Once the NeedItem2 and NeedAmount2 are
used, the for % won't appear next to the choice text. Mainly these are used to bypass the
for % text. I am not sure if NeedItem3 and NeedAmount3 are used..

If GiveItem is unused, it must be set to -1. To skip TextOk and TextNo messages, the
string must be set to "_". Otherwise anything for the values will be printed on to the
player's hud. 

A single NPC can carry up to 5 choices.

Another thing about scripts is that they must ALWAYS be at 1516 bytes, otherwise the
script will break when read. So that means unused items must be null.. or 0x00..

The shareware version of the scripts have a smaller size but I'd rather not get into that
for now ;)
Below are the functions that reads the script then outputs the info into m_menu.c
and m_mobj.c
Oringinal version of these funtions have been taken from Janis' Vavoom source port but
later I modified and improved them to emulate the oringinal Strife behavior.

  -Kaiser
*/


//==========================================================================
//
//	GetSpeechIndex - Scans though the script and tries to match the mobj ID and paragraph
//
//==========================================================================

int C_GetSpeechIndex(RogueConSpeech_t *List, int Count, int ID, int Num)
{
	int Found;
	int i;

	Found = 0;
	for (i = 0; i < Count; i++)
	{
		if (List[i].SpeakerID == ID)
		{
			Found++;
			if (Found == Num)
			{
				return i + 1;
			}
		}
	}
	//FIXME - need to revert back to GenericSpeeches if LevelSpeech index is not found. See map15 for example
	return 0;
}

//==========================================================================
//
//	GetSpeech - Simply returns the dialog block found
//
//==========================================================================

int C_GetSpeech(mobj_t* A)
{
	int Index;

	if (!A->paragraph)
	{
		A->paragraph = 1;
	}
	Index = C_GetSpeechIndex(LevelSpeeches, NumLevelSpeeches,
		A->type, A->paragraph);
	if (Index)
	{
		return Index;
	}
	return -C_GetSpeechIndex(GenericSpeeches, NumGenericSpeeches,
		A->type, A->paragraph);
}

//==========================================================================
//
//	CheckForNeededItem
//
//==========================================================================
//1-24-06	made static	-kaiser
static int C_CheckForNeededItem(mobj_t* mo, int ID, int Amount)
{

   if (ID <= 0) 
      return false; 

   if (ID >= 133 && ID < 160)
   {
	  if(mo->player->keys & (1 << (ID - 133 + 1)))
	  {
		  return true;
	  }
   }
   if (ID >= 196 && ID <= 200)
   {
	   if(mo->player->sigilowned & (1 << (ID - 196)))
	   {
		   return true;
	   }
   }
   if (ID >= 312 && ID <= 342)
   {
	   if(mo->player->quest & (1 << ((ID - 312)+1)))
	   {
		   return true;
	   }
   }
   if(mo->player->inventory.mobjItem[ID] > Amount - 1)
   {
	   return true;
   }
   return false;
}

//==========================================================================
//
//	StopSpeech
//
//==========================================================================

void C_StopSpeech(void)
{
	if(!(CurrentSpeaker && CurrentSpeakingTo))
		return;
	//CurrentSpeaker->talktics = 5;
	CurrentSpeaker->angle = CurrentSpeaker->oldangle;
	P_SetTarget(&CurrentSpeaker, NULL);
	P_SetTarget(&CurrentSpeakingTo, NULL);
	CurrentSpeechIndex = 0;
}

//==========================================================================
//
//	StartSpeech - Sets up the script and choice defs
//
//==========================================================================
//1-24-06	made static	-kaiser
static void C_StartSpeech(int SpeechNum)
{
	RogueConSpeech_t *Speech;
	boolean conJumped = false;

	do
	{
		conJumped = false;
		if (!SpeechNum)
		{
			C_StopSpeech();
			return;
		}
		//added 9-06-05 -kaiser
		if (P_AproxDistance(CurrentSpeakingTo->x-CurrentSpeaker->x,
			CurrentSpeakingTo->y-CurrentSpeaker->y) >
			 96*FRACUNIT + CurrentSpeakingTo->info->radius)
		{
			C_StopSpeech();
			return;
		}
		if (SpeechNum < 0)
		{
			Speech = &GenericSpeeches[-SpeechNum - 1];
		}
		else
		{
			Speech = &LevelSpeeches[SpeechNum - 1];
		}
		if (Speech->JumpToConv &&
			C_CheckForNeededItem(CurrentSpeakingTo, Speech->CheckItem1, 1) == true ||
			C_CheckForNeededItem(CurrentSpeakingTo, Speech->CheckItem2, 1) == true ||
			C_CheckForNeededItem(CurrentSpeakingTo, Speech->CheckItem3, 1) == true)
		{
			CurrentSpeaker->paragraph = Speech->JumpToConv;
			SpeechNum = C_GetSpeech(CurrentSpeaker);
			conJumped = true;
		}
	}
	while (conJumped);
	CurrentSpeechIndex = SpeechNum;
	if(!Speech->Name[0])
	{
		sprintf(defaultname,"%s",CurrentSpeaker->info->stringname);
	}
	CurrentSpeaker->talktics = 10;		//give a delay between speeches -kaiser
	//M_StartDialog(CurrentSpeaker->type,CurrentSpeaker->paragraph);
}

void C_SlideShowDelay(void)
{
	//empty for now
}

//==========================================================================
//
//	StartConversation - Startup the script
//
//==========================================================================

boolean C_StartConversation(mobj_t* User, mobj_t* UseOn)
{
	int SpeechNum = 0;

	if (netgame)
	{
		return false;
	}
	if (!User || !User->player || User->health <= 0)
	{
		return false;
	}
	if (!UseOn || UseOn->health <= 0)
	{
		return false;
	}
	if (!UseOn->type)
	{
		return false;
	}
	if (!UseOn->flags & MF_COUNTKILL || !UseOn->flags & MF_SHOOTABLE)
	{
		return false;
	}
	SpeechNum = C_GetSpeech(UseOn);
	if (SpeechNum)
	{
		CurrentSpeaker = UseOn;
		CurrentSpeakingTo = User;
		C_StartSpeech(SpeechNum);
	}
	return true;
}


//==========================================================================
//
//	C_GetChoiceItem
//
//==========================================================================
//1-24-06	made static	-kaiser
static void C_GetChoiceItem(void)
{
	mobj_t* item;
	int scriptname;
	RogueConSpeech_t *Speech;
	RogueConChoice_t *Choice;

  scriptname = C_GetSpeechIndex(LevelSpeeches, NumLevelSpeeches,
		CurrentSpeaker->type, CurrentSpeaker->paragraph);

  if(scriptname)
  {
	  Speech = &LevelSpeeches[scriptname - 1];
	  Choice = &Speech->Choices[0];

	  if (Choice->GiveItem > 0)
	{
		item = P_SpawnMobj(CurrentSpeaker->x,CurrentSpeaker->y,
			CurrentSpeaker->z,Choice->GiveItem);
		item->silenceditem = true;
		P_TouchSpecialThing(item,CurrentSpeakingTo);
	}
	  
  }
}

//==========================================================================
//
//	CheckChoice - Item choice handling
//
//==========================================================================

boolean plentyofstuff;
boolean slideshow = false;
//1-24-06	made static -kaiser
static boolean C_CheckChoice(RogueConChoice_t *Choice)
{
	int Item1;
	int Item2;
	int Item3;
	mobj_t* item;

	plentyofstuff = false;

	if(Choice->NeedItem1)
	{
		Item1 = C_CheckForNeededItem(CurrentSpeakingTo, Choice->NeedItem1,
			Choice->NeedAmount1);
		if (Item1 == false)
			return false;
	}
	if(Choice->NeedItem2)
	{
		Item2 = C_CheckForNeededItem(CurrentSpeakingTo, Choice->NeedItem2,
			Choice->NeedAmount2);
		if (Item2 == false)
			return false;
	}
	if(Choice->NeedItem3)
	{
		Item3 = C_CheckForNeededItem(CurrentSpeakingTo, Choice->NeedItem3,
			Choice->NeedAmount3);
		if (Item3 == false)
			return false;
	}

	if (Choice->Objectives > 0)
	{
		CurrentSpeakingTo->player->objectives = Choice->Objectives;
	}

	if (Choice->GiveItem > 0)
	{
		item = P_SpawnMobj(CurrentSpeaker->x,CurrentSpeaker->y,
			CurrentSpeaker->z,Choice->GiveItem);
		if(item->type == MT_SLIDESHOW)
		{
			CurrentSpeaker->paragraph++;
			item->silenceditem = true;
			slideshow = true;
			Choice->GiveItem = -1;
			CurrentSpeakingTo->player->lastNpcTic = 20;	//omg hack
			P_TouchSpecialThing(item,CurrentSpeakingTo);
			return false;
		} else slideshow = false;
		if(P_InventoryCheck(CurrentSpeakingTo->player,item))
		{
		item->silenceditem = true;
		plentyofstuff = false;
		P_TouchSpecialThing(item,CurrentSpeakingTo);
		}
		else
		{
			doom_printf("you seem to have enough!");
			plentyofstuff = true;
			P_RemoveMobj(item);
			return true;
		}
	}

	if (Item1 == true)
		P_RemoveInvItem(CurrentSpeakingTo->player,Choice->NeedItem1, Choice->NeedAmount1);
	if (Item2 == true)
		P_RemoveInvItem(CurrentSpeakingTo->player,Choice->NeedItem2, Choice->NeedAmount2);
	if (Item3 == true)
		P_RemoveInvItem(CurrentSpeakingTo->player,Choice->NeedItem3, Choice->NeedAmount3);
	return true;
}

//==========================================================================
//
//	ConChoiceImpulse - Choice selection handling
//
//==========================================================================

void C_ConChoiceImpulse(int ChoiceNum)
{
	RogueConSpeech_t *Speech;
	RogueConChoice_t *Choice;
	char namebuf3[1024 / 4];

	if (!CurrentSpeaker || !CurrentSpeechIndex)
	{
		return;
	}
	if (!ChoiceNum)
	{
		C_StopSpeech();
		return;
	}
	if (CurrentSpeechIndex < 0)
	{
		Speech = &GenericSpeeches[-CurrentSpeechIndex - 1];
	}
	else
	{
		Speech = &LevelSpeeches[CurrentSpeechIndex - 1];
	}
	Choice = &Speech->Choices[ChoiceNum - 1];
	if (!C_CheckChoice(Choice))
	{
		if(slideshow)
			return;
		if(CurrentSpeaker)	//9-04-05 - shopkeeper makes different moods now- kaiser
		{
			if(P_CheckForClass(CurrentSpeaker,CLASS_SHOPGUY))
			P_SetMobjState(CurrentSpeaker,S_SHOP_2);
		}
			doom_printf("%s",Choice->TextNo);
			C_StopSpeech();
			return;
	}
		if(Choice->TextOK[ChoiceNum])
		{
			sprintf(namebuf3,"%s",Choice->TextOK);
			if (strcmp(namebuf3, "") && strcmp(namebuf3, "_"))
			{
				if(plentyofstuff == false) 
				{
					doom_printf("%s",Choice->TextOK);
					if(P_CheckForClass(CurrentSpeaker,CLASS_SHOPGUY))
					P_SetMobjState(CurrentSpeaker,S_SHOP_1);
				}
				else
				{
					doom_printf("you seem to have enough!");
					plentyofstuff = false;
				}
			}
		}
		CurrentSpeakingTo->angle = R_PointToAngle2(CurrentSpeakingTo->x, CurrentSpeakingTo->y,
        CurrentSpeaker->x, CurrentSpeaker->y);
	if (Choice->Next < 0)
	{
		CurrentSpeaker->paragraph = -Choice->Next;
		C_StartSpeech(C_GetSpeech(CurrentSpeaker));
	}
	else
	{
		if (Choice->Next)
		{
			CurrentSpeaker->paragraph = Choice->Next;
		}
		C_StopSpeech();
	}
}

void C_StartTalking(char* spc)
{
	S_StartVoice(CurrentSpeaker, spc);
}