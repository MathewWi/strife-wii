/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 * $Id: p_dialog.h 2005/10/14 0:0:0 kaiser Exp $
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
 *  Strife dialog system and behavior by Janis Legzdinsh and updated by Samuel Villarreal
 *  Dialog funtions
 *
 *-----------------------------------------------------------------------------*/

//moved 1-24-06 -kaiser
//
//	Strife conversation scripts
//

typedef struct {
	int GiveItem;		//	Item given on success
	int NeedItem1;		//	Required item 1
	int NeedItem2;		//	Required item 2
	int NeedItem3;		//	Required item 3
	int NeedAmount1;	//	Amount of item 1
	int NeedAmount2;	//	Amount of item 2
	int NeedAmount3;	//	Amount of item 3
	int Text[32/4];		//	Text of the answer
	int TextOK[80/4];	//	Message displayed on success
	int Next;			//	Dialog to go on success, negative values to go
						// here immediately
	int Objectives;		//	Mission objectives, LOGxxxx lump
	int TextNo[80/4];	//	Message displayed on failure (player doesn't
						// have needed thing, it haves enough health/ammo,
						// item is not ready, quest is not completed)
} RogueConChoice_t;

typedef struct {
	int SpeakerID;		//	Type of the object (MT_xxx)
	int DropItem;		//	Item dropped when killed
	int CheckItem1;		//	Item 1 to check for jump
	int CheckItem2;		//	Item 2 to check for jump
	int CheckItem3;		//	Item 3 to check for jump
	int JumpToConv;		//	Jump to conversation if have certain item(s)
	int Name[16/4];		//	Name of the character
	int Voice[8/4];		//	Voice to play
	int BackPic[8/4];	//	Picture of the speaker
	int Text[320/4];	//	Message
	RogueConChoice_t Choices[5];	//	Choices
} RogueConSpeech_t;

int NumGenericSpeeches;
RogueConSpeech_t *GenericSpeeches;
int NumLevelSpeeches;
RogueConSpeech_t *LevelSpeeches;
int NumSpeeches2;

boolean C_StartConversation(mobj_t* User, mobj_t* UseOn);
void C_StopSpeech(void);
int C_GetSpeech(mobj_t* A);
int C_GetSpeechIndex(RogueConSpeech_t *List, int Count, int ID, int Num);
void C_ConChoiceImpulse(int ChoiceNum);
mobj_t * CurrentSpeaker;
mobj_t * CurrentSpeakingTo;
int OldSpeakerAngle;
int CurrentSpeechIndex;
char defaultname[1024 / 4];
void C_SlideShowDelay(void);
void C_StartTalking(char* spc);
