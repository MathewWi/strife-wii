/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 * $Id: m_misc.c,v 1.26.2.7 2002/07/27 15:58:45 proff_fs Exp $
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
 *  Main loop menu stuff.
 *  Default Config File.
 *  PCX Screenshots.
 *
 *-----------------------------------------------------------------------------*/

static const char
rcsid[] = "$Id: m_misc.c,v 1.26.2.7 2002/07/27 15:58:45 proff_fs Exp $";

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <errno.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef _MSC_VER
#include <io.h>
#endif
#include <fcntl.h>
#include <sys/stat.h>

#include "doomstat.h"
#include "m_argv.h"
#include "g_game.h"
#include "m_menu.h"
#include "am_map.h"
#include "w_wad.h"
#include "i_sound.h"
#include "i_video.h"
#include "v_video.h"
#include "hu_stuff.h"
#include "st_stuff.h"
#include "dstrings.h"
#include "m_misc.h"
#include "s_sound.h"
#include "sounds.h"
#include "i_joy.h"
#include "lprintf.h"
#include "d_main.h"
#include <string.h>
#include <stdlib.h>

//
// M_DrawText
// Returns the final X coordinate
// HU_Init must have been called to init the font
//
extern patchnum_t hu_font[HU_FONTSIZE];
extern patchnum_t hu_font3[HU_FONTSIZE];
#define MAX_STRING_SIZE 1024
#define ASCII_COMMENT ('#')
#define ASCII_QUOTE ('&')
#define LUMP_SCRIPT 1
#define FILE_ZONE_SCRIPT 2
#define FILE_CLIB_SCRIPT 3
char *sc_String;
int sc_Number;
int sc_Line;
boolean sc_End;
boolean sc_Crossed;
boolean sc_FileScripts = false;
char *sc_ScriptsDir = "";
static char ScriptName[16];
static char *ScriptBuffer;
static char *ScriptPtr;
static char *ScriptEndPtr;
static char StringBuffer[MAX_STRING_SIZE];
static boolean ScriptOpen = false;
static boolean ScriptFreeCLib; // true = de-allocate using free()
static int ScriptSize;
static boolean AlreadyGot = false;


//rounds off to the nearest decimal. Not made for four+ digit numbers - kaiser
int nrNum10(int number)
{
	int tempNum;
	int newNum;
	if(number > 1000 || number < -1000)
		return 0;
	tempNum = number % 10;
	if(tempNum <= 5)
		number -= tempNum;
	else
	{
		newNum = 10 - tempNum;
		number += newNum;
	}
	return number;
}


int M_DrawText(int x,int y,boolean direct,char* string, boolean type)
{
  int c;
  int w;

  while (*string) {
    c = toupper(*string) - HU_FONTSTART;
    string++;
    if (c < 0 || c> HU_FONTSIZE) {
      x += 8;
      continue;
    }

    w = SHORT (type ? hu_font3[c].width : hu_font[c].width);
    if (x+w > SCREENWIDTH)
      break;

    // proff/nicolas 09/20/98 -- changed for hi-res
    // CPhipps - patch drawing updated, reformatted
    V_DrawNumPatch(x, y, 0, type ? hu_font3[c].lumpnum : hu_font[c].lumpnum,
		CR_DEFAULT, VPT_STRETCH);
    x+=w;
  }

  return x;
}

int M_CheckStringWidth(const char* string)
{
  int i, c, w = 0;
  for (i = 0;(size_t)i < strlen(string);i++)
    w += (c = toupper(string[i]) - HU_FONTSTART) < 0 || c >= HU_FONTSIZE ?
      4 : SHORT(hu_font[c].width);
  return w;
}

//
//    Find string height from hu_font chars
//

int M_CheckStringHeight(const char* string)
{
  int i, h, height = h = SHORT(hu_font[0].height);
  for (i = 0;string[i];i++)
    if (string[i] == '\n')
      h += height;
  return h;
}

void M_DrawTextW(int x,int y,boolean direct,char* string,int width, boolean type)
{
	int start = 0;
	int cx;
	int cy;
	char cs[80];
	int i, j;
	boolean wordStart = true;

	cx = x;
	cy = y;

	for (i = 0; string[i]; i++)
	{
		if (string[i] == '\n')
	{
		memset(cs, 0, sizeof(cs));
		strncpy(cs, string + start, i - start);
		M_DrawText(cx, cy, false, cs, type);
		cy += M_CheckStringHeight(cs);
		start = i + 1;
		wordStart = true;
	}
	else if (wordStart && string[i] > ' ')
	{
		j = i;
		while (string[j] > ' ')
		j++;
		memset(cs, 0, sizeof(cs));
		strncpy(cs, string + start, j - start);
		if (M_CheckStringWidth(cs) > width)
		{
			memset(cs, 0, sizeof(cs));
			strncpy(cs, string + start, i - start);
			M_DrawText(cx, cy, false, cs, type);
			cy += M_CheckStringHeight(cs);
			start = i;
	}
		wordStart = false;
	}
		else if (string[i] <= ' ')
		{
		wordStart = true;
		}
		if (!string[i + 1])
		{
			M_DrawText(cx, cy, false, string + start, type);
		}
	}
}

void LoadTextLump(char *name, char *buf, int bufsize)
{
	int		msgSize;
	int		msgLump;

	if(W_CheckNumForName(name) == -1)
		return;

	msgLump = W_GetNumForName(name);
	msgSize = W_LumpLength(msgLump);
	if (msgSize >= bufsize)
	{
		I_Error("Message is too long (%s)", name);
	}
	W_ReadLump(msgLump, buf);
	buf[msgSize] = 0; // Append terminator
}

//==========================================================================
//
// M_ExtractFileBase
//
//==========================================================================

void M_ExtractFileBase(char *path, char *dest)
{
	char *src;
	int length;

	src = path+strlen(path)-1;

	// Back up until a \ or the start
	while(src != path && *(src-1) != '\\' && *(src-1) != '/')
	{
		src--;
	}

	// Copy up to eight characters
	memset(dest, 0, 8);
	length = 0;
	while(*src && *src != '.')
	{
		if(++length == 9)
		{
			I_Error("Filename base of %s > 8 chars", path);
		}
		*dest++ = toupper((int)*src++);
	}
}

//==========================================================================
//
// SC_Close
//
//==========================================================================

void SC_Close(void)
{
	if(ScriptOpen)
	{
		if(ScriptFreeCLib == true)
		{
			free(ScriptBuffer);
		}
		else
		{
			Z_Free(ScriptBuffer);
		}
		ScriptOpen = false;
	}
}

//==========================================================================
//
// OpenScript
//
//==========================================================================

static void OpenScript(char *name, int type)
{
	SC_Close();
	if(type == FILE_ZONE_SCRIPT)
	{ // File script - zone
		ScriptSize = M_ReadFile(name, (byte **)&ScriptBuffer);
		M_ExtractFileBase(name, ScriptName);
		ScriptFreeCLib = false; // De-allocate using Z_Free()
	}
	ScriptPtr = ScriptBuffer;
	ScriptEndPtr = ScriptPtr+ScriptSize;
	sc_Line = 1;
	sc_End = false;
	ScriptOpen = true;
	sc_String = StringBuffer;
	AlreadyGot = false;
}

//==========================================================================
//
// SC_OpenLump
//
// Loads a script (from the WAD files) and prepares it for parsing.
//
//==========================================================================

void SC_OpenLump(char *name)
{
	OpenScript(name, LUMP_SCRIPT);
}

//==========================================================================
//
// SC_OpenFile
//
// Loads a script (from a file) and prepares it for parsing.  Uses the
// zone memory allocator for memory allocation and de-allocation.
//
//==========================================================================

void SC_OpenFile(char *name)
{
	OpenScript(name, FILE_ZONE_SCRIPT);
}

//==========================================================================
//
// SC_OpenFileCLib
//
// Loads a script (from a file) and prepares it for parsing.  Uses C
// library function calls for memory allocation and de-allocation.
//
//==========================================================================

void SC_OpenFileCLib(char *name)
{
	OpenScript(name, FILE_CLIB_SCRIPT);
}

//==========================================================================
//
// SC_Open
//
//==========================================================================

void SC_Open(char *name)
{
	char fileName[128];

		sprintf(fileName, "%s.txt", name);
		SC_OpenFile(fileName);
}

//==========================================================================
//
// SC_MatchString
//
// Returns the index of the first match to sc_String from the passed
// array of strings, or -1 if not found.
//
//==========================================================================

int SC_MatchString(char **strings)
{
	int i;

	for(i = 0; *strings != NULL; i++)
	{
		if(SC_Compare(*strings++))
		{
			return i;
		}
	}
	return -1;
}

//==========================================================================
//
// SC_MustMatchString
//
//==========================================================================

int SC_MustMatchString(char **strings)
{
	int i;

	i = SC_MatchString(strings);
	if(i == -1)
	{
		I_Error("String Error");
	}
	return i;
}


//==========================================================================
//
// SC_GetString
//
//==========================================================================

boolean SC_GetString(void)
{
	char *text;
	boolean foundToken;

	if(AlreadyGot)
	{
		AlreadyGot = false;
		return true;
	}
	foundToken = false;
	sc_Crossed = false;
	if(ScriptPtr >= ScriptEndPtr)
	{
		sc_End = true;
		return false;
	}
	while(foundToken == false)
	{
		while(*ScriptPtr <= 32)
		{
			if(ScriptPtr >= ScriptEndPtr)
			{
				sc_End = true;
				return false;
			}
			if(*ScriptPtr++ == '\n')
			{
				sc_Line++;
				sc_Crossed = true;
			}
		}
		if(ScriptPtr >= ScriptEndPtr)
		{
			sc_End = true;
			return false;
		}
		if(*ScriptPtr != ASCII_COMMENT)
		{ // Found a token
			foundToken = true;
		}
		else
		{ // Skip comment
			while(*ScriptPtr++ != '\n')
			{
				if(ScriptPtr >= ScriptEndPtr)
				{
					sc_End = true;
					return false;
	
			}
			}
			sc_Line++;
			sc_Crossed = true;
		}
	}
	text = sc_String;
	if(*ScriptPtr == ASCII_QUOTE)
	{ // Quoted string
		ScriptPtr++;
		while(*ScriptPtr != ASCII_QUOTE)
		{
			*text++ = *ScriptPtr++;
			if(ScriptPtr == ScriptEndPtr
				|| text == &sc_String[MAX_STRING_SIZE-1])
			{
				break;
			}
		}
		ScriptPtr++;
	}
	else
	{ // Normal string
		while((*ScriptPtr > 32) && (*ScriptPtr != ASCII_COMMENT))
		{
			*text++ = *ScriptPtr++;
			if(ScriptPtr == ScriptEndPtr
				|| text == &sc_String[MAX_STRING_SIZE-1])
			{
				break;
			}
		}
	}
	*text = 0;
	return true;
}

boolean SC_Compare(char *text)
{
	if(strcasecmp(text, sc_String) == 0)
	{
		return true;
	}
	return false;
}

//==========================================================================
//
// SC_MustGetString
//
//==========================================================================

void SC_MustGetString(void)
{
	if(SC_GetString() == false)
	{
		I_Error("Missing String");
	}
}

void SC_MustGetStringName(char *name)
{
	SC_MustGetString();
	if(SC_Compare(name) == false)
	{
		I_Error("String Error");
	}
}

boolean SC_GetNumber(void)
{
	char *stopper;

	if(SC_GetString())
	{
		sc_Number = strtol(sc_String, &stopper, 0);
		if(*stopper != 0)
		{
			I_Error("SC_GetNumber: Bad numeric constant \"%s\".\n"
				"Script %s, Line %d", sc_String, ScriptName, sc_Line);
		}
		return true;
	}
	else
	{
		return false;
	}
}

//==========================================================================
//
// SC_MustGetNumber
//
//==========================================================================

void SC_MustGetNumber(void)
{
	if(SC_GetNumber() == false)
	{
		I_Error("missing integer");
	}
}



//
// M_WriteFile
//

boolean M_WriteFile(char const* name,void* source,int length)
{
  int handle;
  int count;

  handle = open ( name, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, 0666);

  if (handle == -1)
    return false;

  count = write (handle, source, length);
  close (handle);

  if (count < length) {
    unlink(name); // CPhipps - no corrupt data files around, they only confuse people.
    return false;
  }

  return true;
}


//
// M_ReadFile
//

int M_ReadFile(char const* name,byte** buffer)
{
  int handle, count, length;
  struct stat fileinfo;
  byte   *buf;

  handle = open (name, O_RDONLY | O_BINARY, 0666);
  if ((handle == -1) || (fstat (handle,&fileinfo) == -1))
    I_Error ("M_ReadFile: Couldn't read file %s", name);

  length = fileinfo.st_size;
  buf = Z_Malloc (length, PU_STATIC, NULL);
  count = read (handle, buf, length);
  close (handle);

  if (count < length)
    I_Error ("M_ReadFile: Couldn't read file %s", name);

  *buffer = buf;
  return length;
}

//
// DEFAULTS
//

int usemouse;
boolean    precache = true; /* if true, load all graphics at start */

extern int mousebfire;
extern int mousebjump;
extern int mousebforward;

extern int viewwidth;
extern int viewheight;
extern int fake_contrast;
#ifdef GL_DOOM
extern int gl_nearclip;
extern int gl_farclip;
extern int gl_colorbuffer_bits;
extern int gl_depthbuffer_bits;
extern char *gl_tex_filter_string;
extern char *gl_tex_format_string;
extern int gl_drawskys;
extern int gl_sortsprites;
extern int gl_use_paletted_texture;
extern int gl_use_shared_texture_palette;
#endif
extern int mouseSensitivity_horiz,mouseSensitivity_vert;  // killough

extern int realtic_clock_rate;         // killough 4/13/98: adjustable timer
extern int tran_filter_pct;            // killough 2/21/98

extern int screenblocks;
extern int showMessages;

#ifndef DJGPP
const char* musserver_filename;
const char* sndserver_filename;
const char* snd_device;
int         mus_pause_opt; // 0 = kill music, 1 = pause, 2 = continue
#endif

extern const char* chat_macros[];

extern int endoom_mode;
int X_opt;

extern const char* S_music_files[]; // cournia

/* cph - Some MBF stuff parked here for now
 * killough 10/98
 */
int map_point_coordinates;

default_t defaults[] =
{
  {"Misc settings",{NULL},{0},UL,UL,def_none,ss_none},
  {"default_compatibility_level",{&default_compatibility_level},
   {-1},-1,MAX_COMPATIBILITY_LEVEL-1,
   def_int,ss_none}, // compatibility level" - CPhipps
  {"realtic_clock_rate",{&realtic_clock_rate},{100},0,UL,
   def_int,ss_none}, // percentage of normal speed (35 fps) realtic clock runs at
  {"max_player_corpse", {&bodyquesize}, {32},-1,UL,   // killough 2/8/98
   def_int,ss_none}, // number of dead bodies in view supported (-1 = no limit)
  {"flashing_hom",{&flashing_hom},{0},0,1,
   def_bool,ss_none}, // killough 10/98 - enable flashing HOM indicator
  {"demo_insurance",{&default_demo_insurance},{2},0,2,  // killough 3/31/98
   def_int,ss_none}, // 1=take special steps ensuring demo sync, 2=only during recordings
  {"endoom_mode", {&endoom_mode},{5},0,7, // CPhipps - endoom flags
   def_hex, ss_none}, // 0, +1 for colours, +2 for non-ascii chars, +4 for skip-last-line
  {"level_precache",{(int*)&precache},{0},0,1,
   def_bool,ss_none}, // precache level data?

  {"Files",{NULL},{0},UL,UL,def_none,ss_none},
  /* cph - MBF-like wad/deh/bex autoload code
   * POSIX targets need to get lumps from prboom.wad */
  {"wadfile_1",{NULL,&wad_files[0]},{0,
#ifdef ALL_IN_ONE
             ""
#else
             "sd:/apps/strifewii/data/prboom.wad"
#endif
                                         },UL,UL,def_str,ss_none},
  {"wadfile_2",{NULL,&wad_files[1]},{0,""},UL,UL,def_str,ss_none},
  {"dehfile_1",{NULL,&deh_files[0]},{0,""},UL,UL,def_str,ss_none},
  {"dehfile_2",{NULL,&deh_files[1]},{0,""},UL,UL,def_str,ss_none},

  {"Game settings",{NULL},{0},UL,UL,def_none,ss_none},
  {"default_skill",{&defaultskill},{3},1,5, // jff 3/24/98 allow default skill setting
   def_int,ss_none}, // selects default skill 1=TYTD 2=NTR 3=HMP 4=UV 5=NM
  {"weapon_recoil",{&default_weapon_recoil},{1},0,1,
   def_bool,ss_weap, &weapon_recoil},
  /* killough 10/98 - toggle between SG/SSG and Fist/Chainsaw */
  {"doom_weapon_toggles",{&doom_weapon_toggles}, {1}, 0, 1,
   def_bool, ss_weap },
  {"player_bobbing",{&default_player_bobbing},{1},0,1,         // phares 2/25/98
   def_bool,ss_weap, &player_bobbing},
  {"monsters_remember",{&default_monsters_remember},{1},0,1,   // killough 3/1/98
   def_bool,ss_enem, &monsters_remember},
   /* MBF AI enhancement options */
  {"monster_infighting",{&default_monster_infighting}, {1}, 0, 1,
   def_bool, ss_enem, &monster_infighting},
  {"monster_backing",{&default_monster_backing}, {1}, 0, 1,
   def_bool, ss_enem, &monster_backing},
  {"monster_avoid_hazards",{&default_monster_avoid_hazards}, {1}, 0, 1,
   def_bool, ss_enem, &monster_avoid_hazards},
  {"monkeys",{&default_monkeys}, {1}, 0, 1,
   def_bool, ss_enem, &monkeys},
  {"monster_friction",{&default_monster_friction}, {1}, 0, 1,
   def_bool, ss_enem, &monster_friction},
  {"help_friends",{&default_help_friends}, {1}, 0, 1,
   def_bool, ss_enem, &help_friends},
#ifdef DOGS
  {"player_helpers",{&default_dogs}, {0}, 0, 3,
   def_bool, ss_enem },
  {"friend_distance",{&default_distfriend}, {128}, 0, 999,
   def_int, ss_enem, &distfriend},
  {"dog_jumping",{&default_dog_jumping}, {1}, 0, 1,
   def_bool, ss_enem, &dog_jumping},
#endif
   /* End of MBF AI extras */
  {"sts_always_red",{&sts_always_red},{0},0,1, // no color changes on status bar
   def_bool,ss_stat},
  {"sts_pct_always_gray",{&sts_pct_always_gray},{1},0,1, // 2/23/98 chg default
   def_bool,ss_stat}, // makes percent signs on status bar always gray
  {"sts_traditional_keys",{&sts_traditional_keys},{0},0,1,  // killough 2/28/98
   def_bool,ss_stat}, // disables doubled card and skull key display on status bar
  {"traditional_menu",{&traditional_menu},{0},0,1,
   def_bool,ss_none}, // force use of Doom's main menu ordering // killough 4/17/98
  {"show_messages",{&showMessages},{1},0,1,
   def_bool,ss_none}, // enables message display
  {"autorun",{&autorun},{0},0,1,  // killough 3/6/98: preserve autorun across games
   def_bool,ss_none},

  {"Compatibility settings",{NULL},{0},UL,UL,def_none,ss_none},
  {"comp_zombie",{&default_comp[comp_zombie]},{0},0,1,def_bool,ss_comp,&comp[comp_zombie]},
  {"comp_infcheat",{&default_comp[comp_infcheat]},{0},0,1,def_bool,ss_comp,&comp[comp_infcheat]},
  {"comp_stairs",{&default_comp[comp_stairs]},{0},0,1,def_bool,ss_comp,&comp[comp_stairs]},
  {"comp_telefrag",{&default_comp[comp_telefrag]},{0},0,1,def_bool,ss_comp,&comp[comp_telefrag]},
  {"comp_dropoff",{&default_comp[comp_dropoff]},{0},0,1,def_bool,ss_comp,&comp[comp_dropoff]},
  {"comp_falloff",{&default_comp[comp_falloff]},{0},0,1,def_bool,ss_comp,&comp[comp_falloff]},
  {"comp_staylift",{&default_comp[comp_staylift]},{0},0,1,def_bool,ss_comp,&comp[comp_staylift]},
  {"comp_doorstuck",{&default_comp[comp_doorstuck]},{0},0,1,def_bool,ss_comp,&comp[comp_doorstuck]},
  {"comp_pursuit",{&default_comp[comp_pursuit]},{0},0,1,def_bool,ss_comp,&comp[comp_pursuit]},
  {"comp_vile",{&default_comp[comp_vile]},{0},0,1,def_bool,ss_comp,&comp[comp_vile]},
  {"comp_pain",{&default_comp[comp_pain]},{0},0,1,def_bool,ss_comp,&comp[comp_pain]},
  {"comp_skull",{&default_comp[comp_skull]},{0},0,1,def_bool,ss_comp,&comp[comp_skull]},
  {"comp_blazing",{&default_comp[comp_blazing]},{0},0,1,def_bool,ss_comp,&comp[comp_blazing]},
  {"comp_doorlight",{&default_comp[comp_doorlight]},{0},0,1,def_bool,ss_comp,&comp[comp_doorlight]},
  {"comp_god",{&default_comp[comp_god]},{0},0,1,def_bool,ss_comp,&comp[comp_god]},
  {"comp_skymap",{&default_comp[comp_skymap]},{0},0,1,def_bool,ss_comp,&comp[comp_skymap]},
  {"comp_floors",{&default_comp[comp_floors]},{0},0,1,def_bool,ss_comp,&comp[comp_floors]},
  {"comp_model",{&default_comp[comp_model]},{0},0,1,def_bool,ss_comp,&comp[comp_model]},
  {"comp_zerotags",{&default_comp[comp_zerotags]},{0},0,1,def_bool,ss_comp,&comp[comp_zerotags]},
  {"comp_moveblock",{&default_comp[comp_moveblock]},{0},0,1,def_bool,ss_comp,&comp[comp_moveblock]},

  {"Sound settings",{NULL},{0},UL,UL,def_none,ss_none},
  {"sound_card",{&snd_card},{-1},-1,7,       // jff 1/18/98 allow Allegro drivers
   def_int,ss_none}, // select sounds driver (DOS), -1 is autodetect, 0 is none; in Linux, non-zero enables sound
  {"music_card",{&mus_card},{-1},-1,9,       //  to be set,  -1 = autodetect
   def_int,ss_none}, // select music driver (DOS), -1 is autodetect, 0 is none"; in Linux, non-zero enables music
  {"pitched_sounds",{&pitched_sounds},{0},0,1, // killough 2/21/98
   def_bool,ss_none}, // enables variable pitch in sound effects (from id's original code)
  {"samplerate",{&snd_samplerate},{22050},11025,48000, def_int,ss_none},
  {"sfx_volume",{&snd_SfxVolume},{8},0,15, def_int,ss_none},
  {"music_volume",{&snd_MusicVolume},{8},0,15, def_int,ss_none},
  {"mus_pause_opt",{&mus_pause_opt},{2},0,2, // CPhipps - music pausing
   def_int, ss_none}, // 0 = kill music when paused, 1 = pause music, 2 = let music continue
  {"sounddev", {NULL,&snd_device}, {0,"/dev/dsp"},UL,UL,
   def_str,ss_none}, // sound output device (UNIX)
  {"snd_channels",{&default_numChannels},{8},1,32,
   def_int,ss_none}, // number of audio events simultaneously // killough

  {"Video settings",{NULL},{0},UL,UL,def_none,ss_none},
  // CPhipps - default screensize for targets that support high-res
#ifndef GL_DOOM
  {"screen_width",{&desired_screenwidth},{320}, 320, 1600,
   def_int,ss_none},
  {"screen_height",{&desired_screenheight},{200},200,1200,
   def_int,ss_none},
#else
  /* proff - 640x480 for OpenGL */
  {"screen_width",{&desired_screenwidth},{640}, 320, 1600,
   def_int,ss_none},
  {"screen_height",{&desired_screenheight},{480},200,1200,
   def_int,ss_none},
#endif
  {"fake_contrast",{&fake_contrast},{1},0,1,
   def_bool,ss_none}, /* cph - allow crappy fake contrast to be disabled */
  {"use_fullscreen",{&use_fullscreen},{1},0,1, /* proff 21/05/2000 */
   def_bool,ss_none},
  {"use_doublebuffer",{&use_doublebuffer},{1},0,1,             // proff 2001-7-4
   def_bool,ss_none}, // enable doublebuffer to avoid display tearing (fullscreen)
  {"translucency",{&default_translucency},{1},0,1,   // phares
   def_bool,ss_none}, // enables translucency
  {"tran_filter_pct",{&tran_filter_pct},{66},0,100,         // killough 2/21/98
   def_int,ss_none}, // set percentage of foreground/background translucency mix
  {"screenblocks",{&screenblocks},{10},3,11,  // killough 2/21/98: default to 10
   def_int,ss_none},
  {"usegamma",{&usegamma},{3},0,4, //jff 3/6/98 fix erroneous upper limit in range
   def_int,ss_none}, // gamma correction level // killough 1/18/98
  {"X_options",{&X_opt},{0},0,3, // CPhipps - misc X options
   def_hex,ss_none}, // X options, see l_video_x.c

#ifdef GL_DOOM
  {"OpenGL settings",{NULL},{0},UL,UL,def_none,ss_none},
  {"gl_nearclip",{&gl_nearclip},{5},0,UL,
   def_int,ss_none}, /* near clipping plane pos */
  {"gl_farclip",{&gl_farclip},{6400},0,UL,
   def_int,ss_none}, /* far clipping plane pos */
  {"gl_colorbuffer_bits",{&gl_colorbuffer_bits},{16},16,32,
   def_int,ss_none},
  {"gl_depthbuffer_bits",{&gl_depthbuffer_bits},{16},16,32,
   def_int,ss_none},
  {"gl_tex_filter_string", {NULL,&gl_tex_filter_string}, {0,"GL_LINEAR"},UL,UL,
   def_str,ss_none},
  {"gl_tex_format_string", {NULL,&gl_tex_format_string}, {0,"GL_RGB5_A1"},UL,UL,
   def_str,ss_none},
  {"gl_drawskys",{&gl_drawskys},{1},0,1,
   def_bool,ss_none},
  {"gl_sortsprites",{&gl_sortsprites},{1},0,1,
   def_bool,ss_none},
  {"gl_use_paletted_texture",{&gl_use_paletted_texture},{1},0,1,
   def_bool,ss_none},
  {"gl_use_shared_texture_palette",{&gl_use_shared_texture_palette},{1},0,1,
   def_bool,ss_none},
#endif

  {"Mouse settings",{NULL},{0},UL,UL,def_none,ss_none},
  {"use_mouse",{&usemouse},{1},0,1,
   def_bool,ss_none}, // enables use of mouse with DOOM
  //jff 4/3/98 allow unlimited sensitivity
  {"mouse_sensitivity_horiz",{&mouseSensitivity_horiz},{10},0,UL,
   def_int,ss_none}, /* adjust horizontal (x) mouse sensitivity killough/mead */
  //jff 4/3/98 allow unlimited sensitivity
  {"mouse_sensitivity_vert",{&mouseSensitivity_vert},{10},0,UL,
   def_int,ss_none}, /* adjust vertical (y) mouse sensitivity killough/mead */
  //jff 3/8/98 allow -1 in mouse bindings to disable mouse function
  {"mouseb_fire",{&mousebfire},{0},-1,MAX_MOUSEB,
   def_int,ss_keys}, // mouse button number to use for fire
  {"mouseb_jump",{&mousebjump},{1},-1,MAX_MOUSEB,
   def_int,ss_keys}, // mouse button number to use for strafing
  {"mouseb_forward",{&mousebforward},{2},-1,MAX_MOUSEB,
   def_int,ss_keys}, // mouse button number to use for forward motion
  //jff 3/8/98 end of lower range change for -1 allowed in mouse binding

// For key bindings, the values stored in the key_* variables       // phares
// are the internal Doom Codes. The values stored in the default.cfg
// file are the keyboard codes.
// CPhipps - now they're the doom codes, so default.cfg can be portable

  {"Key bindings",{NULL},{0},UL,UL,def_none,ss_none},
  {"key_right",       {&key_right},          {KEYD_RIGHTARROW},
   0,MAX_KEY,def_key,ss_keys}, // key to turn right
  {"key_left",        {&key_left},           {KEYD_LEFTARROW} ,
   0,MAX_KEY,def_key,ss_keys}, // key to turn left
  {"key_up",          {&key_up},             {KEYD_UPARROW}   ,
   0,MAX_KEY,def_key,ss_keys}, // key to move forward
  {"key_down",        {&key_down},           {KEYD_DOWNARROW},
   0,MAX_KEY,def_key,ss_keys}, // key to move backward
   {"key_jump",        {&key_jump},           {' '},
   0,MAX_KEY,def_key,ss_keys}, // key to move backward
  {"key_menu_right",  {&key_menu_right},     {KEYD_RIGHTARROW},// phares 3/7/98
   0,MAX_KEY,def_key,ss_keys}, // key to move right in a menu  //     |
  {"key_menu_left",   {&key_menu_left},      {KEYD_LEFTARROW} ,//     V
   0,MAX_KEY,def_key,ss_keys}, // key to move left in a menu
  {"key_menu_up",     {&key_menu_up},        {KEYD_UPARROW}   ,
   0,MAX_KEY,def_key,ss_keys}, // key to move up in a menu
  {"key_menu_down",   {&key_menu_down},      {KEYD_DOWNARROW} ,
   0,MAX_KEY,def_key,ss_keys}, // key to move down in a menu
  {"key_menu_backspace",{&key_menu_backspace},{KEYD_BACKSPACE} ,
   0,MAX_KEY,def_key,ss_keys}, // delete key in a menu
  {"key_menu_escape", {&key_menu_escape},    {KEYD_ESCAPE}    ,
   0,MAX_KEY,def_key,ss_keys}, // key to leave a menu      ,   // phares 3/7/98
  {"key_menu_enter",  {&key_menu_enter},     {KEYD_ENTER}     ,
   0,MAX_KEY,def_key,ss_keys}, // key to select from menu
  {"key_strafeleft",  {&key_strafeleft},     {','}           ,
   0,MAX_KEY,def_key,ss_keys}, // key to strafe left
  {"key_straferight", {&key_straferight},    {'.'}           ,
   0,MAX_KEY,def_key,ss_keys}, // key to strafe right

  {"key_fire",        {&key_fire},           {KEYD_RCTRL}     ,
   0,MAX_KEY,def_key,ss_keys}, // duh
  {"key_use",         {&key_use},            {' '}           ,
   0,MAX_KEY,def_key,ss_keys}, // key to open a door, use a switch
  {"key_strafe",      {&key_strafe},         {KEYD_RALT}      ,
   0,MAX_KEY,def_key,ss_keys}, // key to use with arrows to strafe
  {"key_speed",       {&key_speed},          {KEYD_RSHIFT}    ,
   0,MAX_KEY,def_key,ss_keys}, // key to run

  {"key_savegame",    {&key_savegame},       {KEYD_F2}        ,
   0,MAX_KEY,def_key,ss_keys}, // key to save current game
  {"key_loadgame",    {&key_loadgame},       {KEYD_F3}        ,
   0,MAX_KEY,def_key,ss_keys}, // key to restore from saved games
  {"key_soundvolume", {&key_soundvolume},    {KEYD_F4}        ,
   0,MAX_KEY,def_key,ss_keys}, // key to bring up sound controls
  {"key_setup",         {&key_setup},            {KEYD_F5}        ,
   0,MAX_KEY,def_key,ss_keys},
  {"key_quicksave",   {&key_quicksave},      {KEYD_F6}        ,
   0,MAX_KEY,def_key,ss_keys}, // key to to quicksave
  {"key_endgame",     {&key_endgame},        {KEYD_F7}        ,
   0,MAX_KEY,def_key,ss_keys}, // key to end the game
  {"key_messages",    {&key_messages},       {KEYD_F8}        ,
   0,MAX_KEY,def_key,ss_keys}, // key to toggle message enable
  {"key_quickload",   {&key_quickload},      {KEYD_F9}        ,
   0,MAX_KEY,def_key,ss_keys}, // key to load from quicksave
  {"key_quit",        {&key_quit},           {KEYD_F10}       ,
   0,MAX_KEY,def_key,ss_keys}, // key to quit game
  {"key_gamma",       {&key_gamma},          {KEYD_F11}       ,
   0,MAX_KEY,def_key,ss_keys}, // key to adjust gamma correction
  {"key_spy",         {&key_spy},            {KEYD_F12}       ,
   0,MAX_KEY,def_key,ss_keys}, // key to view from another coop player's view
  {"key_pause",       {&key_pause},          {KEYD_PAUSE}     ,
   0,MAX_KEY,def_key,ss_keys}, // key to pause the game
  {"key_autorun",     {&key_autorun},        {KEYD_CAPSLOCK}  ,
   0,MAX_KEY,def_key,ss_keys}, // key to toggle always run mode
  {"key_chat",        {&key_chat},           {'t'}            ,
   0,MAX_KEY,def_key,ss_keys}, // key to enter a chat message
  {"key_backspace",   {&key_backspace},      {' '} ,
   0,MAX_KEY,def_key,ss_keys}, // backspace key
  {"key_enter",       {&key_enter},          {KEYD_ENTER}     ,
   0,MAX_KEY,def_key,ss_keys}, // key to select from menu or see last message
  {"key_map",         {&key_map},            {KEYD_TAB}       ,
   0,MAX_KEY,def_key,ss_keys}, // key to toggle automap display
  {"key_map_right",   {&key_map_right},      {KEYD_RIGHTARROW},// phares 3/7/98
   0,MAX_KEY,def_key,ss_keys}, // key to shift automap right   //     |
  {"key_map_left",    {&key_map_left},       {KEYD_LEFTARROW} ,//     V
   0,MAX_KEY,def_key,ss_keys}, // key to shift automap left
  {"key_map_up",      {&key_map_up},         {KEYD_UPARROW}   ,
   0,MAX_KEY,def_key,ss_keys}, // key to shift automap up
  {"key_map_down",    {&key_map_down},       {KEYD_DOWNARROW} ,
   0,MAX_KEY,def_key,ss_keys}, // key to shift automap down
  {"key_map_zoomin",  {&key_map_zoomin},      {'='}           ,
   0,MAX_KEY,def_key,ss_keys}, // key to enlarge automap
  {"key_map_zoomout", {&key_map_zoomout},     {'-'}           ,
   0,MAX_KEY,def_key,ss_keys}, // key to reduce automap
  {"key_map_gobig",   {&key_map_gobig},       {'0'}           ,
   0,MAX_KEY,def_key,ss_keys},  // key to get max zoom for automap
  {"key_map_follow",  {&key_map_follow},      {'f'}           ,
   0,MAX_KEY,def_key,ss_keys}, // key to toggle follow mode
  {"key_map_mark",    {&key_map_mark},        {'m'}           ,
   0,MAX_KEY,def_key,ss_keys}, // key to drop a marker on automap
  {"key_map_clear",   {&key_map_clear},       {'c'}           ,
   0,MAX_KEY,def_key,ss_keys}, // key to clear all markers on automap
  {"key_map_grid",    {&key_map_grid},        {'g'}           ,
   0,MAX_KEY,def_key,ss_keys}, // key to toggle grid display over automap
  {"key_map_rotate",  {&key_map_rotate},      {'r'}           ,
   0,MAX_KEY,def_key,ss_keys}, // key to toggle rotating the automap to match the player's orientation
  {"key_map_overlay", {&key_map_overlay},     {'o'}           ,
   0,MAX_KEY,def_key,ss_keys}, // key to toggle overlaying the automap on the rendered display
  {"key_reverse",     {&key_reverse},         {'/'}           ,
   0,MAX_KEY,def_key,ss_keys}, // key to spin 180 instantly
  {"key_zoomin",      {&key_zoomin},          {'='}           ,
   0,MAX_KEY,def_key,ss_keys}, // key to enlarge display
  {"key_zoomout",     {&key_zoomout},         {'-'}           ,
   0,MAX_KEY,def_key,ss_keys}, // key to reduce display
  {"key_chatplayer1", {&destination_keys[0]}, {'g'}            ,
   0,MAX_KEY,def_key,ss_keys}, // key to chat with player 1
  // killough 11/98: fix 'i'/'b' reversal
  {"key_chatplayer2", {&destination_keys[1]}, {'i'}            ,
   0,MAX_KEY,def_key,ss_keys}, // key to chat with player 2
  {"key_chatplayer3", {&destination_keys[2]}, {'b'}            ,
   0,MAX_KEY,def_key,ss_keys}, // key to chat with player 3
  {"key_chatplayer4", {&destination_keys[3]}, {'r'}            ,
   0,MAX_KEY,def_key,ss_keys}, // key to chat with player 4
  {"key_weapontoggle",{&key_weapontoggle},    {'0'}            ,
   0,MAX_KEY,def_key,ss_keys}, // key to toggle between two most preferred weapons with ammo
  {"key_weapon1",     {&key_weapon1},         {'1'}            ,
   0,MAX_KEY,def_key,ss_keys}, // key to switch to weapon 1 (fist/chainsaw)
  {"key_weapon2",     {&key_weapon2},         {'2'}            ,
   0,MAX_KEY,def_key,ss_keys}, // key to switch to weapon 2 (pistol)
  {"key_weapon3",     {&key_weapon3},         {'3'}            ,
   0,MAX_KEY,def_key,ss_keys}, // key to switch to weapon 3 (supershotgun/shotgun)
  {"key_weapon4",     {&key_weapon4},         {'4'}            ,
   0,MAX_KEY,def_key,ss_keys}, // key to switch to weapon 4 (chaingun)
  {"key_weapon5",     {&key_weapon5},         {'5'}            ,
   0,MAX_KEY,def_key,ss_keys}, // key to switch to weapon 5 (rocket launcher)
  {"key_weapon6",     {&key_weapon6},         {'6'}            ,
   0,MAX_KEY,def_key,ss_keys}, // key to switch to weapon 6 (plasma rifle)
  {"key_weapon7",     {&key_weapon7},         {'7'}            ,
   0,MAX_KEY,def_key,ss_keys}, // key to switch to weapon 7 (bfg9000)         //    ^
  {"key_weapon8",     {&key_weapon8},         {'8'}            ,
   0,MAX_KEY,def_key,ss_keys}, // key to switch to weapon 8 (chainsaw)        //    |
  {"key_weapon9",     {&key_weapon9},         {'9'}            ,
   0,MAX_KEY,def_key,ss_keys}, // key to switch to weapon 9 (supershotgun)    // phares

  // killough 2/22/98: screenshot key
  {"key_screenshot",  {&key_screenshot},      {'*'}            ,
   0,MAX_KEY,def_key,ss_keys}, // key to take a screenshot

   {"key_showstat",        {&key_showstat},           {'z'},
   0,MAX_KEY,def_key,ss_keys},

    {"key_showkeylist",        {&key_showkeylist},           {'c'},
   0,MAX_KEY,def_key,ss_keys},

   {"key_invleft",        {&key_invleft},           {'['},
   0,MAX_KEY,def_key,ss_keys},
   {"key_invright",        {&key_invright},           {']'},
   0,MAX_KEY,def_key,ss_keys},

   {"key_dropinv",        {&key_dropinv},           {KEYD_BACKSPACE},
   0,MAX_KEY,def_key,ss_keys},

   {"key_useinv",        {&key_useinv},           {'u'},
   0,MAX_KEY,def_key,ss_keys},

   {"key_log",        {&key_log},           {'x'},
   0,MAX_KEY,def_key,ss_keys},

   {"key_lookup",        {&key_lookup},           {'v'},
   0,MAX_KEY,def_key,ss_keys},

   {"key_lookdown",        {&key_lookdown},           {'q'},
   0,MAX_KEY,def_key,ss_keys},

   {"key_lookcenter",        {&key_lookcenter},           {'h'},
   0,MAX_KEY,def_key,ss_keys},

  {"Joystick settings",{NULL},{0},UL,UL,def_none,ss_none},
  {"use_joystick",{&usejoystick},{0},0,2,
   def_int,ss_none}, // number of joystick to use (0 for none)
  {"joy_left",{&joyleft},{0},  UL,UL,def_int,ss_none},
  {"joy_right",{&joyright},{0},UL,UL,def_int,ss_none},
  {"joy_up",  {&joyup},  {0},  UL,UL,def_int,ss_none},
  {"joy_down",{&joydown},{0},  UL,UL,def_int,ss_none},
  {"joyb_fire",{&joybfire},{0},0,UL,
   def_int,ss_keys}, // joystick button number to use for fire
  {"joyb_strafe",{&joybstrafe},{1},0,UL,
   def_int,ss_keys}, // joystick button number to use for strafing
  {"joyb_speed",{&joybspeed},{2},0,UL,
   def_int,ss_keys}, // joystick button number to use for running
  {"joyb_use",{&joybuse},{3},0,UL,
   def_int,ss_keys}, // joystick button number to use for use/open

  {"Chat macros",{NULL},{0},UL,UL,def_none,ss_none},
  {"chatmacro0", {0,&chat_macros[0]}, {0,HUSTR_CHATMACRO0},UL,UL,
   def_str,ss_chat}, // chat string associated with 0 key
  {"chatmacro1", {0,&chat_macros[1]}, {0,HUSTR_CHATMACRO1},UL,UL,
   def_str,ss_chat}, // chat string associated with 1 key
  {"chatmacro2", {0,&chat_macros[2]}, {0,HUSTR_CHATMACRO2},UL,UL,
   def_str,ss_chat}, // chat string associated with 2 key
  {"chatmacro3", {0,&chat_macros[3]}, {0,HUSTR_CHATMACRO3},UL,UL,
   def_str,ss_chat}, // chat string associated with 3 key
  {"chatmacro4", {0,&chat_macros[4]}, {0,HUSTR_CHATMACRO4},UL,UL,
   def_str,ss_chat}, // chat string associated with 4 key
  {"chatmacro5", {0,&chat_macros[5]}, {0,HUSTR_CHATMACRO5},UL,UL,
   def_str,ss_chat}, // chat string associated with 5 key
  {"chatmacro6", {0,&chat_macros[6]}, {0,HUSTR_CHATMACRO6},UL,UL,
   def_str,ss_chat}, // chat string associated with 6 key
  {"chatmacro7", {0,&chat_macros[7]}, {0,HUSTR_CHATMACRO7},UL,UL,
   def_str,ss_chat}, // chat string associated with 7 key
  {"chatmacro8", {0,&chat_macros[8]}, {0,HUSTR_CHATMACRO8},UL,UL,
   def_str,ss_chat}, // chat string associated with 8 key
  {"chatmacro9", {0,&chat_macros[9]}, {0,HUSTR_CHATMACRO9},UL,UL,
   def_str,ss_chat}, // chat string associated with 9 key

  {"Automap settings",{NULL},{0},UL,UL,def_none,ss_none},
  //jff 1/7/98 defaults for automap colors
  //jff 4/3/98 remove -1 in lower range, 0 now disables new map features
  {"mapcolor_back", {&mapcolor_back}, {0},0,255,  // black //jff 4/6/98 new black
   def_colour,ss_auto}, // color used as background for automap
  {"mapcolor_grid", {&mapcolor_grid}, {15},0,255,  // dk gray
   def_colour,ss_auto}, // color used for automap grid lines
  {"mapcolor_wall", {&mapcolor_wall}, {6},0,255,   // red-brown
   def_colour,ss_auto}, // color used for one side walls on automap
  {"mapcolor_fchg", {&mapcolor_fchg}, {116},0,255,   // lt brown
   def_colour,ss_auto}, // color used for lines floor height changes across
  {"mapcolor_cchg", {&mapcolor_cchg}, {116},0,255,  // orange
   def_colour,ss_auto}, // color used for lines ceiling height changes across
  {"mapcolor_clsd", {&mapcolor_clsd}, {116},0,255,  // white
   def_colour,ss_auto}, // color used for lines denoting closed doors, objects
  {"mapcolor_rkey", {&mapcolor_rkey}, {0},0,255,  // red
   def_colour,ss_auto}, // color used for red key sprites
  {"mapcolor_bkey", {&mapcolor_bkey}, {0},0,255,  // blue
   def_colour,ss_auto}, // color used for blue key sprites
  {"mapcolor_ykey", {&mapcolor_ykey}, {0},0,255,  // yellow
   def_colour,ss_auto}, // color used for yellow key sprites
  {"mapcolor_rdor", {&mapcolor_rdor}, {0},0,255,  // red
   def_colour,ss_auto}, // color used for closed red doors
  {"mapcolor_bdor", {&mapcolor_bdor}, {0},0,255,  // blue
   def_colour,ss_auto}, // color used for closed blue doors
  {"mapcolor_ydor", {&mapcolor_ydor}, {0},0,255,  // yellow
   def_colour,ss_auto}, // color used for closed yellow doors
  {"mapcolor_tele", {&mapcolor_tele}, {96},0,255,  // dk green
   def_colour,ss_auto}, // color used for teleporter lines
  {"mapcolor_secr", {&mapcolor_secr}, {208},0,255,  // purple
   def_colour,ss_auto}, // color used for lines around secret sectors
  {"mapcolor_exit", {&mapcolor_exit}, {64},0,255,    // none
   def_colour,ss_auto}, // color used for exit lines
  {"mapcolor_unsn", {&mapcolor_unsn}, {16},0,255,  // dk gray
   def_colour,ss_auto}, // color used for lines not seen without computer map
  {"mapcolor_flat", {&mapcolor_flat}, {16},0,255,   // lt gray
   def_colour,ss_auto}, // color used for lines with no height changes
  {"mapcolor_sprt", {&mapcolor_sprt}, {233},0,255,  // green
   def_colour,ss_auto}, // color used as things
  {"mapcolor_hair", {&mapcolor_hair}, {208},0,255,  // white
   def_colour,ss_auto}, // color used for dot crosshair denoting center of map
  {"mapcolor_sngl", {&mapcolor_sngl}, {224},0,255,  // white
   def_colour,ss_auto}, // color used for the single player arrow
  {"mapcolor_me",   {&mapcolor_me}, {241},0,255, // green
   def_colour,ss_auto}, // your (player) colour
  {"mapcolor_frnd",   {&mapcolor_frnd}, {241},0,255,
   def_colour,ss_auto},
  //jff 3/9/98 add option to not show secrets til after found
  {"map_secret_after", {&map_secret_after}, {0},0,1, // show secret after gotten
   def_bool,ss_auto}, // prevents showing secret sectors till after entered
  {"map_point_coord", {&map_point_coordinates}, {0},0,1,
   def_bool,ss_auto},
  //jff 1/7/98 end additions for automap
//  {"automapmode", {(int*)&automapmode}, {0}, 0, 31, // CPhipps - remember automap mode
  // def_hex,ss_none}, // automap mode	//disabled for great justice - kaiser

  {"Heads-up display settings",{NULL},{0},UL,UL,def_none,ss_none},
  //jff 2/16/98 defaults for color ranges in hud and status
  {"hudcolor_titl", {&hudcolor_titl}, {5},0,9,  // gold range
   def_int,ss_auto}, // color range used for automap level title
  {"hudcolor_xyco", {&hudcolor_xyco}, {3},0,9,  // green range
   def_int,ss_auto}, // color range used for automap coordinates
  {"hudcolor_mesg", {&hudcolor_mesg}, {5},0,9,  // red range	//kaiser
   def_int,ss_mess}, // color range used for messages during play
  {"hudcolor_chat", {&hudcolor_chat}, {5},0,9,  // gold range
   def_int,ss_mess}, // color range used for chat messages and entry
  {"hudcolor_list", {&hudcolor_list}, {5},0,9,  // gold range  //jff 2/26/98
   def_int,ss_mess}, // color range used for message review
  {"hud_msg_lines", {&hud_msg_lines}, {1},1,16,  // 1 line scrolling window
   def_int,ss_mess}, // number of messages in review display (1=disable)
  {"hud_list_bgon", {&hud_list_bgon}, {0},0,1,  // solid window bg ena //jff 2/26/98
   def_bool,ss_mess}, // enables background window behind message review
  {"hud_distributed",{&hud_distributed},{0},0,1, // hud broken up into 3 displays //jff 3/4/98
   def_bool,ss_none}, // splits HUD into three 2 line displays

  {"health_red",    {&health_red}   , {25},0,200, // below is red
   def_int,ss_stat}, // amount of health for red to yellow transition
  {"health_yellow", {&health_yellow}, {50},0,200, // below is yellow
   def_int,ss_stat}, // amount of health for yellow to green transition
  {"health_green",  {&health_green} , {100},0,200,// below is green, above blue
   def_int,ss_stat}, // amount of health for green to blue transition
  {"armor_red",     {&armor_red}    , {25},0,200, // below is red
   def_int,ss_stat}, // amount of armor for red to yellow transition
  {"armor_yellow",  {&armor_yellow} , {50},0,200, // below is yellow
   def_int,ss_stat}, // amount of armor for yellow to green transition
  {"armor_green",   {&armor_green}  , {100},0,200,// below is green, above blue
   def_int,ss_stat}, // amount of armor for green to blue transition
  {"ammo_red",      {&ammo_red}     , {25},0,100, // below 25% is red
   def_int,ss_stat}, // percent of ammo for red to yellow transition
  {"ammo_yellow",   {&ammo_yellow}  , {50},0,100, // below 50% is yellow, above green
   def_int,ss_stat}, // percent of ammo for yellow to green transition

  //jff 2/16/98 HUD and status feature controls
  {"hud_active",    {&hud_active}, {2},0,2, // 0=off, 1=small, 2=full
   def_int,ss_none}, // 0 for HUD off, 1 for HUD small, 2 for full HUD
  //jff 2/23/98
  {"hud_displayed", {&hud_displayed},  {0},0,1, // whether hud is displayed
   def_bool,ss_none}, // enables display of HUD
  {"hud_nosecrets", {&hud_nosecrets},  {0},0,1, // no secrets/items/kills HUD line
   def_bool,ss_stat}, // disables display of kills/items/secrets on HUD

  {"Weapon preferences",{NULL},{0},UL,UL,def_none,ss_none},
  // killough 2/8/98: weapon preferences set by user:
  {"weapon_choice_1", {&weapon_preferences[0][0]}, {6}, 0,9,
   def_int,ss_weap}, // first choice for weapon (best)
  {"weapon_choice_2", {&weapon_preferences[0][1]}, {9}, 0,9,
   def_int,ss_weap}, // second choice for weapon
  {"weapon_choice_3", {&weapon_preferences[0][2]}, {4}, 0,9,
   def_int,ss_weap}, // third choice for weapon
  {"weapon_choice_4", {&weapon_preferences[0][3]}, {3}, 0,9,
   def_int,ss_weap}, // fourth choice for weapon
  {"weapon_choice_5", {&weapon_preferences[0][4]}, {2}, 0,9,
   def_int,ss_weap}, // fifth choice for weapon
  {"weapon_choice_6", {&weapon_preferences[0][5]}, {8}, 0,9,
   def_int,ss_weap}, // sixth choice for weapon
  {"weapon_choice_7", {&weapon_preferences[0][6]}, {5}, 0,9,
   def_int,ss_weap}, // seventh choice for weapon
  {"weapon_choice_8", {&weapon_preferences[0][7]}, {7}, 0,9,
   def_int,ss_weap}, // eighth choice for weapon
  {"weapon_choice_9", {&weapon_preferences[0][8]}, {1}, 0,9,
   def_int,ss_weap}, // ninth choice for weapon (worst)

  // cournia - support for arbitrary music file (defaults are mp3)
  {"Music", {NULL},{0},UL,UL,def_none,ss_none},	//kaiser - wtf is this used for?
  {"mus_action", {0,&S_music_files[mus_action]}, {0,"action.mp3"},UL,UL,
   def_str,ss_none},
  {"mus_tavern", {0,&S_music_files[mus_tavern]}, {0,"tavern.mp3"},UL,UL,
   def_str,ss_none},
   {"mus_danger", {0,&S_music_files[mus_danger]}, {0,"danger.mp3"},UL,UL,
   def_str,ss_none},
  {"mus_fast", {0,&S_music_files[mus_fast]}, {0,"fast.mp3"},UL,UL,
   def_str,ss_none},
  {"mus_intro", {0,&S_music_files[mus_intro]}, {0,"intro.mp3"},UL,UL,
   def_str,ss_none},
  {"mus_darker", {0,&S_music_files[mus_darker]}, {0,"darker.mp3"},UL,UL,
   def_str,ss_none},
  {"mus_strike", {0,&S_music_files[mus_strike]}, {0,"strike.mp3"},UL,UL,
   def_str,ss_none},
  {"mus_slide", {0,&S_music_files[mus_slide]}, {0,"slide.mp3"},UL,UL,
   def_str,ss_none},
  {"mus_tribal", {0,&S_music_files[mus_tribal]}, {0,"tribal.mp3"},UL,UL,
   def_str,ss_none},
  {"mus_march", {0,&S_music_files[mus_march]}, {0,"march.mp3"},UL,UL,
   def_str,ss_none},
  {"mus_mood", {0,&S_music_files[mus_mood]}, {0,"mood.mp3"},UL,UL,
   def_str,ss_none},
  {"mus_castle", {0,&S_music_files[mus_castle]}, {0,"castle.mp3"},UL,UL,
   def_str,ss_none},
  {"mus_fight", {0,&S_music_files[mus_fight]}, {0,"fight.mp3"},UL,UL,
   def_str,ss_none},
  {"mus_spense", {0,&S_music_files[mus_spense]}, {0,"spense.mp3"},UL,UL,
   def_str,ss_none},
  {"mus_dark", {0,&S_music_files[mus_dark]}, {0,"dark.mp3"},UL,UL,
   def_str,ss_none},
  {"mus_tech", {0,&S_music_files[mus_tech]}, {0,"tech.mp3"},UL,UL,
   def_str,ss_none},
  {"mus_drone", {0,&S_music_files[mus_drone]}, {0,"drone.mp3"},UL,UL,
   def_str,ss_none},
  {"mus_panthr", {0,&S_music_files[mus_panthr]}, {0,"panthr.mp3"},UL,UL,
   def_str,ss_none},
  {"mus_sad", {0,&S_music_files[mus_sad]}, {0,"sad.mp3"},UL,UL,
   def_str,ss_none},
  {"mus_instry", {0,&S_music_files[mus_instry]}, {0,"instry.mp3"},UL,UL,
   def_str,ss_none},
  {"mus_happy", {0,&S_music_files[mus_happy]}, {0,"happy.mp3"},UL,UL,
   def_str,ss_none},
  {"mus_end", {0,&S_music_files[mus_end]}, {0,"end.mp3"},UL,UL,
   def_str,ss_none},
  {"mus_logo", {0,&S_music_files[mus_logo]}, {0,"logo.mp3"},UL,UL,
   def_str,ss_none},
   //shareware mus
   {"mus_fmintr", {0,&S_music_files[mus_strife]}, {0,"strife.mp3"},UL,UL,
   def_str,ss_none},
   {"mus_fmstl", {0,&S_music_files[mus_strife]}, {0,"strife.mp3"},UL,UL,
   def_str,ss_none},
   {"mus_fmfast", {0,&S_music_files[mus_strife]}, {0,"strife.mp3"},UL,UL,
   def_str,ss_none},
   {"mus_fmact", {0,&S_music_files[mus_strife]}, {0,"strife.mp3"},UL,UL,
   def_str,ss_none},
   {"mus_fmdrne", {0,&S_music_files[mus_strife]}, {0,"strife.mp3"},UL,UL,
   def_str,ss_none},
   {"mus_map1", {0,&S_music_files[mus_strife]}, {0,"strife.mp3"},UL,UL,
   def_str,ss_none},
   {"mus_map2", {0,&S_music_files[mus_strife]}, {0,"strife.mp3"},UL,UL,
   def_str,ss_none},
   {"mus_map3", {0,&S_music_files[mus_strife]}, {0,"strife.mp3"},UL,UL,
   def_str,ss_none},
   {"mus_strife", {0,&S_music_files[mus_strife]}, {0,"strife.mp3"},UL,UL,
   def_str,ss_none},
};

int numdefaults;
static const char* defaultfile; // CPhipps - static, const

//
// M_SaveDefaults
//

void M_SaveDefaults (void)
  {
  int   i;
  FILE* f;

  f = fopen (defaultfile, "w");
  if (!f)
    return; // can't write the file, but don't complain

  // 3/3/98 explain format of file

  fprintf(f,"# Strife config file\n");

  for (i = 0 ; i < numdefaults ; i++) {
    if (defaults[i].type == def_none) {
      // CPhipps - pure headers
      fprintf(f, "\n# %s\n", defaults[i].name);
    } else
    // CPhipps - modified for new default_t form
    if (!IS_STRING(defaults[i])) //jff 4/10/98 kill super-hack on pointer value
      {
      // CPhipps - remove keycode hack
      // killough 3/6/98: use spaces instead of tabs for uniform justification
      if (defaults[i].type == def_hex)
  fprintf (f,"%-25s 0x%x\n",defaults[i].name,*(defaults[i].location.pi));
      else
  fprintf (f,"%-25s %5i\n",defaults[i].name,*(defaults[i].location.pi));
      }
    else
      {
      fprintf (f,"%-25s \"%s\"\n",defaults[i].name,*(defaults[i].location.ppsz));
      }
    }

  fclose (f);
  }

/*
 * M_LookupDefault
 *
 * cph - mimic MBF function for now. Yes it's crap.
 */

struct default_s *M_LookupDefault(const char *name)
{
  int i;
  for (i = 0 ; i < numdefaults ; i++)
    if ((defaults[i].type != def_none) && !strcmp(name, defaults[i].name))
      return &defaults[i];
  I_Error("M_LookupDefault: %s not found",name);
  return NULL;
}

//
// M_LoadDefaults
//

#define NUMCHATSTRINGS 10 // phares 4/13/98

void M_LoadDefaults (void)
{
  int   i;
  int   len;
  FILE* f;
  char  def[80];
  char  strparm[100];
  char* newstring = NULL;   // killough
  int   parm;
  boolean isstring;

  // set everything to base values

  numdefaults = sizeof(defaults)/sizeof(defaults[0]);
  for (i = 0 ; i < numdefaults ; i++) {
    if (defaults[i].location.ppsz)
      *defaults[i].location.ppsz = strdup(defaults[i].defaultvalue.psz);
    if (defaults[i].location.pi)
      *defaults[i].location.pi = defaults[i].defaultvalue.i;
  }

  // check for a custom default file

  i = M_CheckParm ("-config");
  if (i && i < myargc-1)
    defaultfile = myargv[i+1];
  else
    defaultfile = basedefault;

  lprintf (LO_CONFIRM, " default file: %s\n",defaultfile);

  // read the file in, overriding any set defaults

  f = fopen (defaultfile, "r");
  if (f)
    {
    while (!feof(f))
      {
      isstring = false;
      if (fscanf (f, "%79s %[^\n]\n", def, strparm) == 2)
        {

        //jff 3/3/98 skip lines not starting with an alphanum

        if (!isalnum(def[0]))
          continue;

        if (strparm[0] == '"') {
          // get a string default

          isstring = true;
          len = strlen(strparm);
          newstring = (char *) malloc(len);
          strparm[len-1] = 0; // clears trailing double-quote mark
          strcpy(newstring, strparm+1); // clears leading double-quote mark
  } else if ((strparm[0] == '0') && (strparm[1] == 'x')) {
    // CPhipps - allow ints to be specified in hex
    sscanf(strparm+2, "%x", &parm);
  } else {
          sscanf(strparm, "%i", &parm);
    // Keycode hack removed
  }

        for (i = 0 ; i < numdefaults ; i++)
          if ((defaults[i].type != def_none) && !strcmp(def, defaults[i].name))
            {
      // CPhipps - safety check
            if (isstring != IS_STRING(defaults[i])) {
        lprintf(LO_WARN, "M_LoadDefaults: Type mismatch reading %s\n", defaults[i].name);
        continue;
      }
            if (!isstring)
              {

              //jff 3/4/98 range check numeric parameters

              if ((defaults[i].minvalue==UL || defaults[i].minvalue<=parm) &&
                  (defaults[i].maxvalue==UL || defaults[i].maxvalue>=parm))
                *(defaults[i].location.pi) = parm;
              }
            else
              {
              free((char*)*(defaults[i].location.ppsz));  /* phares 4/13/98 */
              *(defaults[i].location.ppsz) = newstring;
              }
            break;
            }
        }
      }

    fclose (f);
    }
  //jff 3/4/98 redundant range checks for hud deleted here
}


//
// SCREEN SHOTS
//

// CPhipps - nasty but better than nothing
static boolean screenshot_write_error;

// jff 3/30/98 types and data structures for BMP output of screenshots
//
// killough 5/2/98:
// Changed type names to avoid conflicts with endianess functions

#define BI_RGB 0L

typedef unsigned long dword_t;
typedef long     long_t;
typedef unsigned char ubyte_t;

#ifdef _MSC_VER // proff: This is the same as __attribute__ ((packed)) in GNUC
#pragma pack(push)
#pragma pack(1)
#endif //_MSC_VER

#if defined(__MWERKS__)
#pragma options align=packed
#endif

typedef struct tagBITMAPFILEHEADER
  {
  unsigned short  bfType;
  dword_t bfSize;
  unsigned short  bfReserved1;
  unsigned short  bfReserved2;
  dword_t bfOffBits;
  } PACKEDATTR BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
  {
  dword_t biSize;
  long_t  biWidth;
  long_t  biHeight;
  unsigned short  biPlanes;
  unsigned short  biBitCount;
  dword_t biCompression;
  dword_t biSizeImage;
  long_t  biXPelsPerMeter;
  long_t  biYPelsPerMeter;
  dword_t biClrUsed;
  dword_t biClrImportant;
  } PACKEDATTR BITMAPINFOHEADER;

#if defined(__MWERKS__)
#pragma options align=reset
#endif

#ifdef _MSC_VER
#pragma pack(pop)
#endif //_MSC_VER

// jff 3/30/98 binary file write with error detection
// CPhipps - static, const on parameter
static void SafeWrite(const void *data, size_t size, size_t number, FILE *st)
{
  if (fwrite(data,size,number,st)<number)
    screenshot_write_error = true; // CPhipps - made non-fatal
}

#ifndef GL_DOOM
//
// WriteBMPfile
// jff 3/30/98 Add capability to write a .BMP file (256 color uncompressed)
//

// CPhipps - static, const on parameters
static void WriteBMPfile(const char* filename, const byte* data,
       const int width, const int height, const byte* palette)
{
  int i,wid;
  BITMAPFILEHEADER bmfh;
  BITMAPINFOHEADER bmih;
  int fhsiz,ihsiz;
  FILE *st;
  char zero=0;
  ubyte_t c;

  fhsiz = sizeof(BITMAPFILEHEADER);
  ihsiz = sizeof(BITMAPINFOHEADER);
  wid = 4*((width+3)/4);
  //jff 4/22/98 add endian macros
  bmfh.bfType = SHORT(19778);
  bmfh.bfSize = LONG(fhsiz+ihsiz+256L*4+width*height);
  bmfh.bfReserved1 = SHORT(0);
  bmfh.bfReserved2 = SHORT(0);
  bmfh.bfOffBits = LONG(fhsiz+ihsiz+256L*4);

  bmih.biSize = LONG(ihsiz);
  bmih.biWidth = LONG(width);
  bmih.biHeight = LONG(height);
  bmih.biPlanes = SHORT(1);
  bmih.biBitCount = SHORT(8);
  bmih.biCompression = LONG(BI_RGB);
  bmih.biSizeImage = LONG(wid*height);
  bmih.biXPelsPerMeter = LONG(0);
  bmih.biYPelsPerMeter = LONG(0);
  bmih.biClrUsed = LONG(256);
  bmih.biClrImportant = LONG(256);

  st = fopen(filename,"wb");
  if (st!=NULL) {
    // write the header
    SafeWrite(&bmfh.bfType,sizeof(bmfh.bfType),1,st);
    SafeWrite(&bmfh.bfSize,sizeof(bmfh.bfSize),1,st);
    SafeWrite(&bmfh.bfReserved1,sizeof(bmfh.bfReserved1),1,st);
    SafeWrite(&bmfh.bfReserved2,sizeof(bmfh.bfReserved2),1,st);
    SafeWrite(&bmfh.bfOffBits,sizeof(bmfh.bfOffBits),1,st);

    SafeWrite(&bmih.biSize,sizeof(bmih.biSize),1,st);
    SafeWrite(&bmih.biWidth,sizeof(bmih.biWidth),1,st);
    SafeWrite(&bmih.biHeight,sizeof(bmih.biHeight),1,st);
    SafeWrite(&bmih.biPlanes,sizeof(bmih.biPlanes),1,st);
    SafeWrite(&bmih.biBitCount,sizeof(bmih.biBitCount),1,st);
    SafeWrite(&bmih.biCompression,sizeof(bmih.biCompression),1,st);
    SafeWrite(&bmih.biSizeImage,sizeof(bmih.biSizeImage),1,st);
    SafeWrite(&bmih.biXPelsPerMeter,sizeof(bmih.biXPelsPerMeter),1,st);
    SafeWrite(&bmih.biYPelsPerMeter,sizeof(bmih.biYPelsPerMeter),1,st);
    SafeWrite(&bmih.biClrUsed,sizeof(bmih.biClrUsed),1,st);
    SafeWrite(&bmih.biClrImportant,sizeof(bmih.biClrImportant),1,st);

    // write the palette, in blue-green-red order, gamma corrected
    for (i=0;i<768;i+=3) {
      c=gammatable[usegamma][palette[i+2]];
      SafeWrite(&c,sizeof(char),1,st);
      c=gammatable[usegamma][palette[i+1]];
      SafeWrite(&c,sizeof(char),1,st);
      c=gammatable[usegamma][palette[i+0]];
      SafeWrite(&c,sizeof(char),1,st);
      SafeWrite(&zero,sizeof(char),1,st);
    }

    for (i = 0 ; i < height ; i++)
      SafeWrite(data+(height-1-i)*width,sizeof(byte),wid,st);

    fclose(st);
  }
}

#else /* GL_DOOM */

//
// WriteTGAfile
// proff 05/15/2000 Add capability to write a .TGA file (24bit color uncompressed)
//

// CPhipps - static, const on parameters
static void WriteTGAfile(const char* filename, const byte* data,
       const int width, const int height)
{
  unsigned char c;
  unsigned short s;
  int i;
  FILE *st;

  st = fopen(filename,"wb");
  if (st!=NULL) {
    // write the header
    // id_length
    c=0; SafeWrite(&c,sizeof(c),1,st);
    // colormap_type
    c=0; SafeWrite(&c,sizeof(c),1,st);
    // image_type
    c=2; SafeWrite(&c,sizeof(c),1,st);
    // colormap_index
    s=0; SafeWrite(&s,sizeof(s),1,st);
    // colormap_length
    s=0; SafeWrite(&s,sizeof(s),1,st);
    // colormap_size
    c=0; SafeWrite(&c,sizeof(c),1,st);
    // x_origin
    s=0; SafeWrite(&s,sizeof(s),1,st);
    // y_origin
    s=0; SafeWrite(&s,sizeof(s),1,st);
    // width
    s=SHORT(width); SafeWrite(&s,sizeof(s),1,st);
    // height
    s=SHORT(height); SafeWrite(&s,sizeof(s),1,st);
    // bits_per_pixel
    c=24; SafeWrite(&c,sizeof(c),1,st);
    // attributes
    c=0; SafeWrite(&c,sizeof(c),1,st);

    for (i=0; i<width*height*3; i+=3)
    {
      SafeWrite(&data[i+2],sizeof(byte),1,st);
      SafeWrite(&data[i+1],sizeof(byte),1,st);
      SafeWrite(&data[i+0],sizeof(byte),1,st);
    }

    fclose(st);
  }
}
#endif /* GL_DOOM */

//
// M_ScreenShot
//
// Modified by Lee Killough so that any number of shots can be taken,
// the code is faster, and no annoying "screenshot" message appears.

// CPhipps - modified to use its own buffer for the image
//         - checks for the case where no file can be created (doesn't occur on POSIX systems, would on DOS)
//         - track errors better
//         - split into 2 functions

//
// M_DoScreenShot
// Takes a screenshot into the names file

void M_DoScreenShot (const char* fname)
{
  byte       *linear;
#ifndef GL_DOOM
  const byte *pal;
  int        pplump = W_GetNumForName("PLAYPAL");
#endif

  screenshot_write_error = false;

#ifdef GL_DOOM
  // munge planar buffer to linear
  // CPhipps - use a malloc()ed buffer instead of screens[2]
  gld_ReadScreen(linear = malloc(SCREENWIDTH * SCREENHEIGHT * 3));

  // save the bmp file

  WriteTGAfile
    (fname, linear, SCREENWIDTH, SCREENHEIGHT);
#else
  // munge planar buffer to linear
  // CPhipps - use a malloc()ed buffer instead of screens[2]
  I_ReadScreen(linear = malloc(SCREENWIDTH * SCREENHEIGHT));

  // killough 4/18/98: make palette stay around (PU_CACHE could cause crash)
  pal = W_CacheLumpNum (pplump);

  // save the bmp file

  WriteBMPfile
    (fname, linear, SCREENWIDTH, SCREENHEIGHT, pal);

  // cph - free the palette
  W_UnlockLumpNum(pplump);
#endif
  free(linear);
  // 1/18/98 killough: replace "SCREEN SHOT" acknowledgement with sfx

  if (screenshot_write_error)
    doom_printf("M_ScreenShot: Error writing screenshot");
}

void M_ScreenShot(void)
{/*
  static int shot;
  char       lbmname[32];
  int        startshot;

  screenshot_write_error = false;

//  if (access(".",2)) screenshot_write_error = true;

  startshot = shot; // CPhipps - prevent infinite loop

  do
#ifdef GL_DOOM
    sprintf(lbmname,"STRIFE%02d.TGA", shot++);
#else
    sprintf(lbmname,"STRIFE%02d.BMP", shot++);
#endif
//  while (!access(lbmname,0) && (shot != startshot) && (shot < 10000));

//  if (!access(lbmname,0)) screenshot_write_error = true;

  if (screenshot_write_error) {
#ifdef GL_DOOM
    doom_printf ("M_ScreenShot: Couldn't create a TGA");
#else
    doom_printf ("M_ScreenShot: Couldn't create a BMP");
#endif
    // killough 4/18/98
    return;
  }

  M_DoScreenShot(lbmname); // cph
  doom_printf("captured %s", lbmname);	//kaiser

  S_StartSound(NULL,sfx_radio);*/
}
