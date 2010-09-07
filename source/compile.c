/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 * $Id: am_map.c,v 1.11.2.1 2002/07/20 18:08:33 proff_fs Exp $
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
 *   The Strife script compiling code by Samuel Villarreal
 *	 Many thanks to SlayeR and Quasar
 *
 *-----------------------------------------------------------------------------
 */
#include <math.h>
#include "m_misc.h"
#include "doomstat.h"
#include "m_argv.h"
#include "compile.h"
#include "lprintf.h"

void SV_CompileScript(int scriptmap)
{
	int i;
	/*Compile Script stuff */
  //-------------------
  char ScriptName[9];
  FILE *mapscript;
  char mapname[9];

  //mobj
  int mobjnumber;
  //--
  //Drop
  int dropnumber;
  //--
  //mobj name
  char mobjname[16];
  //--
  //check mobj
  int checkmobj;
  //--
  //goto block
  int gotoblock;
  //--
  //voice
  char voice[8];
  //--
  //panel
  char panel[8];
  //--
  //dialouge
  char dialouge[320];
  //--
  //responses
  int nullchoice;
  char choice[32];
  char yestext[80];
  int linkblock;
  char notext[80];
  int giveitem;
  int checkitem;
  int itemamount;
  int usedreplies;
  int totalreplies;
  //--
  sprintf(mapname,"script%02d.lmp",scriptmap);
  mapscript = fopen (mapname, "wb");

  sprintf(ScriptName,"SCRIPT%02d",scriptmap);
  SC_Open(ScriptName);
  while(SC_GetString())
  {
	if(SC_Compare("MOBJ"))
	{
		SC_MustGetNumber();
		mobjnumber = sc_Number;
		if(!mobjnumber >= 1)
		{
			I_Error("Line %i - Need to specify a map thing!", sc_Line);
			return;
		}
		fwrite(&mobjnumber,sizeof(int), 1, mapscript);
	}
	if(SC_Compare("DROP"))
	{
		SC_MustGetNumber();
		dropnumber = sc_Number;
		if(dropnumber == 0)
			memset(&dropnumber, 0, 4);
		fwrite(&dropnumber,sizeof(int), 1, mapscript);
	}
	if(SC_Compare("IF_ITEM1"))
	{
		SC_MustGetNumber();
		checkmobj = sc_Number;
		if(checkmobj == 0)
			memset(&checkmobj, 0, 4);
		fwrite(&checkmobj,sizeof(int), 1, mapscript);
	}
	if(SC_Compare("IF_ITEM2"))
	{
		SC_MustGetNumber();
		checkmobj = sc_Number;
		if(checkmobj == 0)
			memset(&checkmobj, 0, 4);
		fwrite(&checkmobj,sizeof(int), 1, mapscript);
	}
	if(SC_Compare("IF_ITEM3"))
	{
		SC_MustGetNumber();
		checkmobj = sc_Number;
		if(checkmobj == 0)
			memset(&checkmobj, 0, 4);
		fwrite(&checkmobj,sizeof(int), 1, mapscript);
	}
	if(SC_Compare("GOTO"))
	{
		SC_MustGetNumber();
		gotoblock = sc_Number;
		if(gotoblock == 0)
			memset(&gotoblock, 0, 4);
		fwrite(&gotoblock,sizeof(int), 1, mapscript);
	}
	if(SC_Compare("NAME"))
	{
		memset(mobjname, 0, 16);
		SC_MustGetString();
		sprintf(mobjname,"%s",sc_String);
		fwrite(mobjname,sizeof(mobjname), 1, mapscript);
	}
	if(SC_Compare("VOICE"))
	{
		memset(voice, 0, 8);
		SC_MustGetString();
		sprintf(voice,"%s",sc_String);
		fwrite(voice,sizeof(voice), 1, mapscript);
	}
	if(SC_Compare("PANEL"))
	{
		memset(panel, 0, 8);
		SC_MustGetString();
		sprintf(panel,"%s",sc_String);
		fwrite(panel,sizeof(panel), 1, mapscript);
	}
	if(SC_Compare("DIALOUGE"))
	{
		memset(dialouge, 0, 320);
		SC_MustGetString();
		sprintf(dialouge,"%s",sc_String);
		fwrite(dialouge,sizeof(dialouge), 1, mapscript);
	}
	if(SC_Compare("GIVEMOBJ"))
		{
			SC_MustGetNumber();
			giveitem = sc_Number;
			if(giveitem == 0)
			{
				I_Error("Line %i - Value needs to be a -1 if not used!", sc_Line);
				return;
			}
			fwrite(&giveitem,sizeof(int), 1, mapscript);
		}
	if(SC_Compare("IF_HAS1"))
		{
			SC_MustGetNumber();
			checkitem = sc_Number;
			if(checkitem == 0)
			memset(&checkitem, 0, 4);
			fwrite(&checkitem,sizeof(int), 1, mapscript);
		}
	if(SC_Compare("IF_HAS2"))
		{
			SC_MustGetNumber();
			checkitem = sc_Number;
			if(checkitem == 0)
			memset(&checkitem, 0, 4);
			fwrite(&checkitem,sizeof(int), 1, mapscript);
		}
	if(SC_Compare("IF_HAS3"))
		{
			SC_MustGetNumber();
			checkitem = sc_Number;
			if(checkitem == 0)
			memset(&checkitem, 0, 4);
			fwrite(&checkitem,sizeof(int), 1, mapscript);
		}
	if(SC_Compare("IF_HASAMOUNT1"))
		{
			SC_MustGetNumber();
			itemamount = sc_Number;
			if(itemamount == 0)
			memset(&itemamount, 0, 4);
			fwrite(&itemamount,sizeof(int), 1, mapscript);
		}
	if(SC_Compare("IF_HASAMOUNT2"))
		{
			SC_MustGetNumber();
			itemamount = sc_Number;
			if(itemamount == 0)
			memset(&itemamount, 0, 4);
			fwrite(&itemamount,sizeof(int), 1, mapscript);
		}
	if(SC_Compare("IF_HASAMOUNT3"))
		{
			SC_MustGetNumber();
			itemamount = sc_Number;
			if(itemamount == 0)
			memset(&itemamount, 0, 4);
			fwrite(&itemamount,sizeof(int), 1, mapscript);
		}
		if(SC_Compare("TEXT"))
			{
				memset(choice, 0, 32);
				SC_MustGetString();
				sprintf(choice,"%s",sc_String);
				//set the no text
				memset(notext,0,80);
				sprintf(notext,"NO.  you don't have what i want for the  %s",sc_String);
				//--
				fwrite(choice,sizeof(choice), 1, mapscript);
			}
			if(SC_Compare("YESTEXT"))
			{
				memset(yestext, 0, 80);
				SC_MustGetString();
				sprintf(yestext,"%s",sc_String);
				fwrite(yestext,sizeof(yestext), 1, mapscript);
			}
			if(SC_Compare("USEDEFAULTYES"))
			{
				memset(yestext,0,80);
				sprintf(yestext,"you got the %s", choice);
				fwrite(yestext,sizeof(yestext), 1, mapscript);
			}
			if(SC_Compare("LINK"))
			{
				SC_MustGetNumber();
				linkblock = sc_Number;
				if(linkblock == 0)
				memset(&linkblock, 0, 4);
				fwrite(&linkblock,sizeof(int), 1, mapscript);
				memset(&nullchoice, 0, 4);
				fwrite(&nullchoice,sizeof(int), 1, mapscript);
			}
			if(SC_Compare("NOTEXT"))
			{
				memset(notext, 0, 80);
				SC_MustGetString();
				sprintf(notext,"%s",sc_String);
				fwrite(notext,sizeof(notext), 1, mapscript);
			}
			if(SC_Compare("USEDEFAULTNO"))
			{
				//write the no text after this
				fwrite(notext,sizeof(notext), 1, mapscript);
			}
			if(SC_Compare("TOTAL"))
			{
				usedreplies = 0;
				totalreplies = 0;
				SC_MustGetNumber();
				usedreplies = sc_Number;
				switch(usedreplies)
				{
				case 0:
					 totalreplies = 285;
					 break;
				case 1:
					totalreplies = 228;
					break;
				case 2:
					totalreplies = 171;
					break;
				case 3:
					totalreplies = 114;
					break;
				case 4:
					totalreplies = 57;
					break;
				case 5:
					totalreplies = 0;
					break;
				}
			}
	if(SC_Compare("END"))
	{
		//heh..fill in that gap since there is no response..
		for(i = 0; i < totalreplies; i++)
		{
		memset(&nullchoice, 0, 4);
		fwrite(&nullchoice,sizeof(int), 1, mapscript);
		}
	}
	SC_Close();
  }
  I_Error("Compile Finished");
}
