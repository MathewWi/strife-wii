/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
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
 *      Status bar code.
 *      Does the face/direction indicator animatin.
 *      Does palette indicators as well (red pain/berserk, bright pickup)
 *
 *-----------------------------------------------------------------------------*/

#include "doomdef.h"
#include "doomstat.h"
#include "m_random.h"
#include "i_video.h"
#include "w_wad.h"
#include "st_stuff.h"
#include "st_lib.h"
#include "r_main.h"
#include "am_map.h"
#include "m_cheat.h"
#include "s_sound.h"
#include "sounds.h"
#include "dstrings.h"
#include "r_draw.h"
#include "p_inter.h"
#include "m_misc.h"
#include "d_player.h"

static void DrawCommonBar(void);	//kaiser
static patch_t *PatchINumbers[10];	//kaiser
static patch_t *PatchYNumbers[10];	//kaiser
//static patch_t *pcbow;
int AmmoPosY[NUMAMMO] = { 77, 101, 93, 141, 133, 117, 125 };	//janis
const patch_t *PatchSELECTBOX;
static char* hudText;
static boolean usePickupText = false;
static char *keyList[] = 
{
	"none",
	"base key",
	"govs key",
	"passcard",
	"id badge",
	"prison key",	
	"severed hand",
	"power1 key",	
	"power2 key",	
	"power3 key",	
	"gold key",
	"id card",
	"silver key",
	"oracle key",
	"military id",	
	"order key",
	"warehouse key",
	"brass key",
	"red crystal key",
	"blue crystal key",
	"chapel key",
	"catacomb key",	
	"security key",	
	"core key",
	"mauler key",
	"factory key",
	"mine key",
	"newkey 5"	
};

//
// STATUS BAR DATA
//

// Palette indices.
// For damage/bonus red-/gold-shifts
#define STARTREDPALS            1
#define STARTBONUSPALS          9
#define NUMREDPALS              8
#define NUMBONUSPALS            4
// Radiation suit, green shift.
#define RADIATIONPAL            13

#define ST_TOPX					(0)	//kaiser
#define ST_TOPY					(160)	//kaiser

// killough 2/8/98: weapon info position macros UNUSED, removed here

// main player in game
static player_t *plyr;
static int messageTics;	//kaiser

// ST_Start() has just been called
static boolean st_firsttime;

// used to execute ST_Init() only once
static int veryfirsttime = 1;

// CPhipps - no longer do direct PLAYPAL handling here

// used for timing
static unsigned int st_clock;

// used for making messages go away
static int st_msgcounter=0;

// whether in automap or first-person
static st_stateenum_t st_gamestate;

// whether left-side main status bar is active
static boolean st_statusbaron;

static boolean st_topon;	//kaiser

// main bar left
// CPhipps - convert to a bitmap
static byte *sbar;
static unsigned short sbar_width, sbar_height;

 // main bar right
static patchnum_t topbg;	//kaiser

static st_binicon_t	w_topbg;

// a random number per tick
static int      st_randomnumber;

extern char     *mapnames[];
//preserved for now
//jff 2/16/98 status color change levels
int ammo_red;      // ammo percent less than which status is red
int ammo_yellow;   // ammo percent less is yellow more green
int health_red;    // health amount less than which status is red
int health_yellow; // health amount less than which status is yellow
int health_green;  // health amount above is blue, below is green
int armor_red;     // armor amount less than which status is red
int armor_yellow;  // armor amount less than which status is yellow
int armor_green;   // armor amount above is blue, below is green

//
// STATUS BAR CODE
//

void ST_Stop(void);

void DrawCommonBar(void)	//kaiser
{
	V_DrawNamePatch(0, 161, 0, "INVTOP", CR_DEFAULT, VPT_STRETCH);
}

static void DrawWorldTimer(void)
{
	int days;
	int hours;
	int minutes;
	int seconds;
	int worldTimer;
	char timeBuffer[15];

	worldTimer = players[consoleplayer].worldTimer;

	worldTimer /= 35;
	days = worldTimer/86400;
	worldTimer -= days*86400;
	hours = worldTimer/3600;
	worldTimer -= hours*3600;
	minutes = worldTimer/60;
	worldTimer -= minutes*60;
	seconds = worldTimer;

	sprintf(timeBuffer, "%.2d : %.2d : %.2d", hours, minutes,seconds);
	M_DrawText(190,63,false,timeBuffer, true);
}

//new and improved		-kaiser 9-03-05
static void drawInvNumber(int number, int x, int y, boolean type)
{
	patch_t *patch;
	int i, loop = 0;
	int val = 10;
	int j = 10;
	int o = 1;
	int xVal = 16;
	boolean foundNum = false;

	while(foundNum == false)
	{
		loop++;
		if(number < val)
		{
			foundNum = true;
		}
		val *= 10;
	}
		//loops have been setup, lets draw the first far right digit
		patch = type ? PatchINumbers[number % 10] : PatchYNumbers[number % 10];
		V_DrawMemPatch(x+16, y, 0, patch, CR_DEFAULT, VPT_STRETCH);
		xVal -= 7;
		if(loop >= 3)
		{
			for(i = 2; i < loop; i++)
			{
				j *= 10;
				o *= 10;
				patch = type ? PatchINumbers[(number % j) / o] :
				PatchYNumbers[(number % j) / o];
				V_DrawMemPatch(x+xVal, y, 0, patch, CR_DEFAULT, VPT_STRETCH);
				xVal -=7;
			}
		}
		//now draw the last, left digit
		if(loop >= 2)
		{
			patch = type ? PatchINumbers[number / (val / 100)] :
			PatchYNumbers[number / (val / 100)];
			V_DrawMemPatch(x+xVal, y, 0, patch, CR_DEFAULT, VPT_STRETCH);
		}
}

//Draws a sprite on to the hud from pwad..	-kaiser
//rewritten 10-12-05 - kaiser
void ST_DrawHudSprite(int x, int y, char* sprite)
{
	V_DrawNumPatch(x, y, 0, ((W_CheckNumForName)(sprite, ns_sprites)),
		CR_DEFAULT, VPT_STRETCH);
}

static int pageNum;
static int pagePause;
void DrawKeyList(void)		//kaiser 9-4-05
{
	ticcmd_t *cmd = &plyr->cmd;
	int i, y1, y2, row;
	char namebuf[9];
	char namebuf2[9];
	if(cmd->arti&AFLAG_KEY && pagePause == 0)
	{
		pageNum++;
		pagePause = 5;
		if(pageNum == 4)
		{
			pageNum = 0;
		}
	}
	if(pageNum != 0)
	{
		y1 = 0;
		y2 = 0;
		i = 0;
		row = 0;
		V_DrawNameFuzzPatch(0, 58, 0, "INVPBAK2", VPT_STRETCH);
		V_DrawNamePatch(0, 58, 0, "INVPOP2", CR_DEFAULT, VPT_STRETCH);
		sprintf(namebuf, "Page %i / 3", pageNum);
		M_DrawText(128, 150, false, namebuf, true);
		for(i = (1 + (10 * pageNum - 10)); i < (10 * pageNum + 1); i++)
		{
			row++;
			if(plyr->keys & (1 << i))
			{
				sprintf(namebuf2, "I_%s", sprnames[plyr->inventory.invPic[i]]);
				V_DrawNamePatch(row >= 6 ? 168 :
				24, 64 + (row >= 6 ? y2 : y1), 0, namebuf2,
					CR_DEFAULT, VPT_STRETCH);
				M_DrawText(row >= 6 ? 192 :
				48, 70 + (row >= 6 ? y2 : y1),false,keyList[i],
					true);
			}
			row >= 6 ? (y2 += 16) : (y1 += 16);
		}
	}
}

/*my own version of M_DrawTextW, this is more simple and is more spaced out
 between lines - kaiser   (updated 10-10-05) Now removes double spacing when creating a
 new line*/
static void ST_DrawLogText(int x,int y, boolean type)
{
	char text[80];
	char string[1024];
	int i, k;
	int j = 0;
	int cy = 0;
	int d = 0;
	int len = 0;
	int count = 1;
	boolean stopChecking = false;
	int spaces = 1;
	memset(string, 0, sizeof(string));
	strcpy(string, LogText);
	k = strlen(string);
	for(i = 0; i < k; i++)
	{
		d++;
		count = 1;
		spaces = 1;
		stopChecking = false;
		if(len != 32)
		len++;
		if(len == 32)
		{
			if(string[i] == ' ')
			{
				memset(text, 0, sizeof(text));
				strncpy(text, string + j, i - j);
				M_DrawText(x, y + cy, false, text, type);
				while(stopChecking == false)
				{
					if(string[i + spaces] == ' ')
					{
						count++;
						spaces++;
					}
					else {
						stopChecking = true;
					}
				}
				j = i + count;
				cy += 12;
				len = 0;
			}
		}
	}
	M_DrawText(x, y + cy, false, string + j, type);
}

int LoadedLog;
void DrawPopUpBar(void)	//pop up status screen -kaiser
{
	ticcmd_t *cmd = &plyr->cmd;
	int	i, j;
	int keylist;
	char namebuf[9];
	//10-12-05
	char *wpn[6] = {"CBOWA0","RIFLA0","MMSLA0","GRNDA0","FLAMA0","TRPDA0"};
	int wpnX[6] = {38,40,39,78,79,75};
	int wpnY[6] = {88,109,133,89,119,144};
	int wpnType[6] = {wp_crossbow,wp_rifle,wp_missile,wp_grenade,wp_flamethrower,wp_blaster};

	sprintf(namebuf, "LOG%i", plyr->objectives);

	if(cmd->arti&AFLAG_LOG){
	V_DrawNameFuzzPatch(0, 58, 0, "INVPBAK2", VPT_STRETCH);
	V_DrawNamePatch(0, 58, 0, "INVPOP2", CR_DEFAULT, VPT_STRETCH);
	DrawWorldTimer();
	if(plyr->objectives)
	{
		if (LoadedLog != plyr->objectives)
			{
				LoadTextLump((namebuf), &LogText[0], 1024);
				LoadedLog = plyr->objectives;
			}
			//M_DrawTextW(30, 80, false, LogText, 280, true);
	}
		ST_DrawLogText(30, 72, true);
	}

	if(cmd->arti&AFLAG_STATUS){
	V_DrawNameFuzzPatch(0, 58, 0, "INVPBAK", VPT_STRETCH);
	V_DrawNamePatch(0, 58, 0, "INVPOP", CR_DEFAULT, VPT_STRETCH);
	//we need to draw the ammo counters...somehow..
	for (i = 0; i < NUMAMMO; i++)	//janis
	{
	drawInvNumber(plyr->ammo[i], 183, AmmoPosY[i], 1);
	drawInvNumber(plyr->maxammo[i], 216, AmmoPosY[i], 1);
	}
	drawInvNumber(plyr->accuracy, 244, 86, 1);
	drawInvNumber(plyr->stamina, 244, 110, 1);
	keylist = 0;
	for (i = 0; i < NUMKEYS; i++)
		{
			if (plyr->keys & (1 << i))
			{
				keylist++;
			}
		}
	drawInvNumber(keylist, 244, 134, 1);
	if(plyr->communicator)
	V_DrawNamePatch(280, 132, 0, "I_COMM", CR_DEFAULT, VPT_STRETCH);
	//lets draw those cool weapons into the hud!
	for(j = 0; j < 6; j++)
	{
		if(plyr->weaponowned[wpnType[j]])
		{
			ST_DrawHudSprite(wpnX[j], wpnY[j], wpn[j]);
		}
	}
	}
}

void DrawAmmoPictures(void)		//kaiser
{
	char namebuf[9];
	//int the ammo pictures	-kaiser
	static char *AmmoPic[] =	
	{
		"I_BLIT",
		"I_XQRL",
		"I_PQRL",
		"I_BRY1",
		"I_ROKT",
		"I_GRN1",
		"I_GRN2"
	};

	if(plyr->readyweapon != wp_fist && plyr->readyweapon != wp_sigil)
		V_DrawNamePatch(296, 178, 0, AmmoPic[weaponinfo[plyr->readyweapon].ammo],
		CR_DEFAULT, VPT_STRETCH);

	if(plyr->weaponowned[wp_sigil])
	{
		sprintf(namebuf,"I_SGL%d",plyr->sigilowned + 1);
		V_DrawNamePatch(253, 175, 0, namebuf,CR_DEFAULT,VPT_STRETCH);
	}
}

void DrawInventory(void)
{
	int i;
	int Idx;
	char namebuf[9];
	Idx = 0;
	for (i = plyr->inventory.invFirst; Idx < 6 && i < NUMMOBJTYPES; i++)
	{
		if (plyr->inventory.mobjItem[i])
		{
			if (plyr->inventory.invCursor == i)
			{
				V_DrawNamePatch(42+35*Idx,180,0,"INVCURS",CR_DEFAULT,VPT_STRETCH);
			}
			sprintf(namebuf, "I_%s", sprnames[plyr->inventory.invPic[i]]);
			if(W_CheckNumForName(namebuf) == -1)
				sprintf(namebuf, "STCFN063");	//icon not found? default to '?' -kaiser
			V_DrawNamePatch(48+Idx*35,182,0,namebuf,CR_DEFAULT,VPT_STRETCH);
			drawInvNumber(plyr->inventory.mobjItem[i],54+Idx*35,192, 1);
			Idx++;
		}
	}
}

void DrawHealthBar(void)	//SvStrife stuff	-kaiser
{
	int i;
	char *name;
	int integer;
	integer = plyr->health - 100;

	if(plyr->health < 100)
	{
	for (i = 0; i < plyr->health; i++)
	{
		if(plyr->health > 100)
			continue;
		else if(plyr->health > 40)
			name = "LIFE_G";
		else name = "LIFE_R";
		V_DrawNamePatch(49+i*2,175,0,name,CR_DEFAULT,VPT_STRETCH);
		V_DrawNamePatch(49+i*2,172,0,name,CR_DEFAULT,VPT_STRETCH);
	}
	}
	else
	{
		for (i = 0; i < 100; i++)	//draw the full bar
	{
		if(plyr->cheats & CF_GODMODE)
		name = "LIFE_M";
		else name = "LIFE_G";
		V_DrawNamePatch(49+i*2,175,0,name,CR_DEFAULT,VPT_STRETCH);
		V_DrawNamePatch(49+i*2,172,0,name,CR_DEFAULT,VPT_STRETCH);
	}
		for (i = 0; i < integer; i++)
	{
		name = "LIFE_B";
		V_DrawNamePatch(49+i*2,175,0,name,CR_DEFAULT,VPT_STRETCH);
		V_DrawNamePatch(49+i*2,172,0,name,CR_DEFAULT,VPT_STRETCH);
	}
	}
}

void ST_DrawWrapMessage(char* text)
{
	hudText = text;
	messageTics = 200;
}

static void ST_DisplayWrapMessage(void)
{
	if(messageTics != 0)
	{
		M_DrawTextW(0,0,false,hudText,280, false);
		messageTics--;
		//ignore this
		if(debug1)
		V_DrawNamePatch(64, 58, 0, "XKXSXVX", CR_DEFAULT, VPT_STRETCH);
	}
}

void ST_refreshBackground(void)
{
  int y=0;
  int screen=BG;

  if (st_statusbaron)
    {
#ifdef GL_DOOM
      // proff 05/17/2000: draw to the frontbuffer in OpenGL
      y=ST_Y;
#endif
    }
}


// Respond to keyboard input events,
//  intercept cheats.
boolean ST_Responder(event_t *ev)
{
  // Filter automap on/off.
  if (ev->type == ev_keyup && (ev->data1 & 0xffff0000) == AM_MSGHEADER)
    {
      switch(ev->data1)
        {
        case AM_MSGENTERED:
          st_gamestate = AutomapState;
          st_firsttime = true;
          break;

        case AM_MSGEXITED:
          st_gamestate = FirstPersonState;
          break;
        }
    }
  else  // if a user keypress...
    if (ev->type == ev_keydown)       // Try cheat responder in m_cheat.c
      return M_FindCheats(ev->data1); // killough 4/17/98, 5/2/98
  return false;
}

int sts_traditional_keys; // killough 2/28/98: traditional status bar keys

void ST_Ticker(void)
{
  st_clock++;
}

static int st_palette = 0;

void ST_doPaletteStuff(void)
{
  int         palette;
  int cnt = plyr->damagecount;

  if (plyr->powers[pw_strength])
    {
      // slowly fade the berzerk out
      int bzc = 12 - (plyr->powers[pw_strength]>>6);
      if (bzc > cnt)
        cnt = bzc;
    }

  if (cnt)
    {
      palette = (cnt+7)>>3;
      if (palette >= NUMREDPALS)
        palette = NUMREDPALS-1;
      palette += STARTREDPALS;
    }
  else
    if (plyr->bonuscount)
      {
        palette = (plyr->bonuscount+7)>>3;
        if (palette >= NUMBONUSPALS)
          palette = NUMBONUSPALS-1;
        palette += STARTBONUSPALS;
      }
    else if (plyr->misctime2 > 16*TICRATE || (plyr->misctime2 & 8))
	{
		palette = RADIATIONPAL;
	}
      else
        palette = 0;

  if (palette != st_palette)
    V_SetPalette(st_palette = palette); // CPhipps - use new palette function
}

void ST_doRefresh(void)
{

  st_firsttime = false;

}

void ST_Drawer(boolean st_statusbaron, boolean refresh)
{
	//what a mess..
	DrawPopUpBar();
	DrawKeyList();
	if(pagePause)
	{
		pagePause--;
	}
	ST_DisplayWrapMessage();
	if(!(viewheight == SCREENHEIGHT) || automapmode & am_active)
	{
		if(plyr->powers[pw_targeter])
		{
			ST_DrawHudSprite(160,80,"TRGTA0");
			ST_DrawHudSprite(110,80,"TRGTB0");
			ST_DrawHudSprite(210,80,"TRGTC0");
		}
		V_DrawNamePatch(0, 168, 0, "INVBACK", CR_DEFAULT, VPT_STRETCH);
		DrawCommonBar();
		DrawInventory();
		DrawHealthBar();
		DrawAmmoPictures();
		drawInvNumber(plyr->health, 56, 163, 1);	//kaiser
		if(plyr->armortype == 1)
		{	
			V_DrawNamePatch(10, 176, 0, "I_ARM2", CR_DEFAULT, VPT_STRETCH);
		}
		else if(plyr->armortype == 2)
		{
			V_DrawNamePatch(10, 176, 0, "I_ARM1", CR_DEFAULT, VPT_STRETCH);
		}
		if(plyr->readyweapon != wp_fist && plyr->readyweapon != wp_sigil)
		{
			drawInvNumber(plyr->ammo[weaponinfo[plyr->readyweapon].ammo], 289, 163, 1);	//kaiser
		}
		if(plyr->armorpoints != 0)
		{
			drawInvNumber(plyr->armorpoints, 8, 192, 0);	//kaiser
		}
	}
	else
	{
		if(plyr->powers[pw_targeter])
		{
			ST_DrawHudSprite(160,100,"TRGTA0");
			ST_DrawHudSprite(110,100,"TRGTB0");
			ST_DrawHudSprite(210,100,"TRGTC0");
		}
		V_DrawNamePatch(10, 176, 0, "I_MDKT", CR_DEFAULT, VPT_STRETCH);
		drawInvNumber(plyr->health, 8, 192, 1);	//kaiser
		DrawAmmoPictures();
		if(plyr->inventory.invDecide)
		{
			DrawInventory();
		}
		if(plyr->armortype == 1)
		{
			V_DrawNamePatch(10, 152, 0, "I_ARM2", CR_DEFAULT, VPT_STRETCH);
		}
		else if(plyr->armortype == 2)
		{
			V_DrawNamePatch(10, 152, 0, "I_ARM1", CR_DEFAULT, VPT_STRETCH);
		}
		if(plyr->readyweapon != wp_fist && plyr->readyweapon != wp_sigil)
		{
			drawInvNumber(plyr->ammo[weaponinfo[plyr->readyweapon].ammo], 289, 192, 1);	//kaiser
		}
		if(plyr->armorpoints != 0)
		{
			drawInvNumber(plyr->armorpoints, 8, 170, 0);	//kaiser
		}
	}

  st_firsttime = st_firsttime || refresh;

  ST_doPaletteStuff();  // Do red-/gold-shifts from damage/items

#ifdef GL_DOOM
  /* proff 05/17/2000: always draw everything in OpenGL, because there
   * is no backbuffer
   */
  if (st_statusbaron)
    ST_doRefresh();
#else
  if (st_statusbaron) {
    if (st_firsttime)
      ST_doRefresh();     /* If just after ST_Start(), refresh all */
  }
#endif
}



//
// ST_loadGraphics
//
// CPhipps - Loads graphics needed for status bar if doload is true,
//  unloads them otherwise
//
static void ST_loadGraphics(boolean doload)
{
  int  i, getlump, getlump2;

  R_SetPatchNum(&topbg,"INVTOP");
  PatchSELECTBOX = W_CacheLumpName("INVCURS");

  getlump = W_GetNumForName("INVFONG0");
  getlump2 = W_GetNumForName("INVFONY0");
  for(i = 0; i < 10; i++)
	{
		PatchINumbers[i] = W_CachePatchNum(getlump+i, PU_STATIC);	//kaiser
		PatchYNumbers[i] = W_CachePatchNum(getlump2+i, PU_STATIC);	//kaiser
	}

  // status bar background bits
  if (doload)
    sbar = V_PatchToBlock("INVBACK", CR_DEFAULT, VPT_NONE,
      &sbar_width, &sbar_height);
  else {
    free(sbar); sbar=NULL;
  }
}

void ST_loadData(void)
{
  ST_loadGraphics(true);
}

void ST_unloadData(void)
{
  ST_loadGraphics(false);
}

void ST_initData(void)
{
  st_firsttime = true;
  plyr = &players[displayplayer];            // killough 3/7/98
  st_clock = 0;
  st_gamestate = FirstPersonState;
  st_statusbaron = true;
  st_palette = -1;
  STlib_init();
}

static boolean st_stopped = true;

void ST_Start(void)
{
  if (!st_stopped)
    ST_Stop();
  ST_initData();
  st_stopped = false;
}

void ST_Stop(void)
{
  if (st_stopped)
    return;
  V_SetPalette(0);
  st_stopped = true;
}

void ST_Init(void)
{
  veryfirsttime = 0;
  ST_loadData();
// proff 08/18/98: Changed for high-res
  screens[4] = Z_Malloc(SCREENWIDTH*(ST_SCALED_HEIGHT+1), PU_STATIC, 0);
//  screens[4] = Z_Malloc(ST_WIDTH*ST_HEIGHT, PU_STATIC, 0);
}
