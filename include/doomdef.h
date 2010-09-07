/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 * $Id: doomdef.h,v 1.6.2.2 2002/07/20 18:08:34 proff_fs Exp $
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
 *  Internally used data structures for virtually everything,
 *   key definitions, lots of other stuff.
 *
 *-----------------------------------------------------------------------------*/

#ifndef __DOOMDEF__
#define __DOOMDEF__

/* use config.h if autoconf made one -- josh */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// killough 4/25/98: Make gcc extensions mean nothing on other compilers
#ifndef __GNUC__
#define __attribute__(x)
#endif

// This must come first, since it redefines malloc(), free(), etc. -- killough:
#include "z_zone.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

// this should go here, not in makefile/configure.in -- josh
#ifndef O_BINARY
#define O_BINARY 0
#endif

#include "m_swap.h"
#include "version.h"

// Game mode handling - identify IWAD version
//  to handle IWAD dependend animations etc.
typedef enum {
  shareware,    // DOOM 1 shareware, E1, M9
  registered,   // DOOM 1 registered, E3, M27
  commercial,   // DOOM 2 retail, E1 M34  (DOOM 2 german edition not handled)
  retail,       // DOOM 1 retail, E4, M36
  indetermined  // Well, no IWAD found.
} GameMode_t;

typedef enum	//different endings used for D_QuitGame(); -kaiser
{
	quitgame,
	dark,
	sad,
	happy,
	maciltalk1,
	maciltalk2,
	noend
} Finale_t;

// Mission packs - might be useful for TC stuff?
typedef enum {
  doom,         // DOOM 1
  doom2,        // DOOM 2
  pack_tnt,     // TNT mission pack
  pack_plut,    // Plutonia pack
  pack_striferetail,	//kaiser
  pack_strifedemo,	//kaiser
  none
} GameMission_t;

// Identify language to use, software localization.
typedef enum {
  english,
  french,
  german,
  unknown
} Language_t;

//
// For resize of screen, at start of game.
//

#define BASE_WIDTH 320

// It is educational but futile to change this
//  scaling e.g. to 2. Drawing of status bar,
//  menues etc. is tied to the scale implied
//  by the graphics.

#define INV_ASPECT_RATIO   0.625 /* 0.75, ideally */

#define NEWTICRATERATIO   1  // kaiser
signed char pitch; // look up/down (-110 : +90)

// killough 2/8/98: MAX versions for maximum screen sizes
// allows us to avoid the overhead of dynamic allocation
// when multiple screen sizes are supported

// proff 08/17/98: Changed for high-res
#define MAX_SCREENWIDTH  1600
#define MAX_SCREENHEIGHT 1200

extern int SCREENWIDTH;
extern int SCREENHEIGHT;

// The maximum number of players, multiplayer/networking.
#define MAXPLAYERS       4

// phares 5/14/98:
// DOOM Editor Numbers (aka doomednum in mobj_t)	//changed for strife - kaiser

#define DEN_PLAYER5 5
#define DEN_PLAYER6 6
#define DEN_PLAYER7 7
#define DEN_PLAYER8 8

// State updates, number of tics / second.
#define TICRATE          35

// The current state of the game: whether we are playing, gazing
// at the intermission screen, the game final animation, or a demo.

typedef enum {
  GS_LEVEL,
  GS_INTERMISSION,
  GS_FINALE,
  GS_DEMOSCREEN,
  GS_ENDING
} gamestate_t;

//
// Difficulty/skill settings/filters.
//
// These are Thing flags	//kaiser

// Skill flags.
#define MTF_EASY                1
#define MTF_NORMAL              2
#define MTF_HARD                4
#define MTF_STAND              8
#define MTF_NOTSINGLE          16
#define MTF_AMBUSH  		  32	//renamed 1-22-06	kaiser
#define MTF_FRIEND            64
#define MTF_UNKNOWN2		  128
#define MTF_TRANSLUCENT		  256
#define MTF_MVIS		      512
#define MTF_RESERVED          1024

typedef enum {
  sk_none=-1, //jff 3/24/98 create unpicked skill setting
  sk_baby=0,
  sk_easy,
  sk_medium,
  sk_hard,
  sk_nightmare
} skill_t;

//
// Key cards.
//

typedef enum {
  it_bluecard,
  it_yellowcard,
  it_redcard,
  it_blueskull,
  it_yellowskull,
  it_redskull,
  NUMCARDS
} card_t;

// The defined weapons, including a marker
// indicating user has not changed weapon.
typedef enum {	//kaiser
  wp_fist,
  wp_crossbow,	//pistol
  wp_rifle,	//shotgun
  wp_grenade,	//chaingun
  wp_flamethrower,	//rocket launcher
  wp_crossbow2,	//plamsagun
  wp_grenade2,	//bfg
  wp_blaster,	//chainsaw
  wp_missile,	//supershotgun
  wp_blaster2,	
  wp_sigil,


  NUMWEAPONS,
  wp_nochange              // No pending weapon change.
} weapontype_t;

typedef enum {	//kaiser
	sl_sigil1,
	sl_sigil2,
	sl_sigil3,
	sl_sigil4,
	sl_sigil5,

	NUMSIGIL,
}	sigiltype_t;

// Ammunition types defined.
typedef enum {	//kaiser
  am_clip,
  am_arrows,	//shells
  am_poisarrows,
  am_cell,
  am_misl,
  am_grenades1,
  am_grenades2,
  NUMAMMO,
  am_noammo
} ammotype_t;

// Power up artifacts.
typedef enum {
  pw_invulnerability,
  pw_strength,
  pw_invisibility,
  pw_ironfeet,
  pw_allmap,
  pw_infrared,
  pw_allthings,
  pw_targeter,
  NUMPOWERS
} powertype_t;

typedef enum {	//kaiser
	TOKEN_QUEST1 = 1,
	TOKEN_QUEST2,
	TOKEN_QUEST3,
	TOKEN_QUEST4,
	TOKEN_QUEST5,
	TOKEN_QUEST6,
	TOKEN_QUEST7,
	TOKEN_QUEST8,
	TOKEN_QUEST9,
	TOKEN_QUEST10,
	TOKEN_QUEST11,
	TOKEN_QUEST12,
	TOKEN_QUEST13,
	TOKEN_QUEST14,
	TOKEN_QUEST15,
	TOKEN_QUEST16,
	TOKEN_QUEST17,
	TOKEN_QUEST18,
	TOKEN_QUEST19,
	TOKEN_QUEST20,
	TOKEN_QUEST21,
	TOKEN_QUEST22,
	TOKEN_QUEST23,
	TOKEN_QUEST24,
	TOKEN_QUEST25,
	TOKEN_QUEST26,
	TOKEN_QUEST27,
	TOKEN_QUEST28,
	TOKEN_QUEST29,
	TOKEN_QUEST30,
	TOKEN_QUEST31,
	NUMQUESTS
} questtype_t;

typedef enum	//kaiser/janis
{
	key_blank,
	key_BaseKey,
	key_GovsKey,
	key_Passcard,
	key_IDBadge,
	key_PrisonKey,
	key_SeveredHand,
	key_Power1Key,
	key_Power2Key,
	key_Power3Key,
	key_GoldKey,
	key_IDCard,
	key_SilverKey,
	key_OracleKey,
	key_MilitaryID,
	key_OrderKey,
	key_WarehouseKey,
	key_BrassKey,
	key_RedCrystalKey,
	key_BlueCrystalKey,
	key_ChapelKey,
	key_CatacombKey,
	key_SecurityKey,
	key_CoreKey,
	key_MaulerKey,
	key_FactoryKey,
	key_MineKey,
	key_NewKey5,

	NUMKEYS
};

// Power up durations (how many seconds till expiration).
typedef enum {
  INVULNTICS  = (30*TICRATE),
  INVISTICS   = (60*TICRATE),
  INFRATICS   = (120*TICRATE),
  IRONTICS    = (60*TICRATE),
  TARGETTICS    = (120*TICRATE),
} powerduration_t;

// DOOM keyboard definition.
// This is the stuff configured by Setup.Exe.
// Most key data are simple ascii (uppercased).

#define KEYD_RIGHTARROW 0xae
#define KEYD_LEFTARROW  0xac
#define KEYD_UPARROW    0xad
#define KEYD_DOWNARROW  0xaf
#define KEYD_ESCAPE     27
#define KEYD_ENTER      13
#define KEYD_TAB        9
#define KEYD_F1         (0x80+0x3b)
#define KEYD_F2         (0x80+0x3c)
#define KEYD_F3         (0x80+0x3d)
#define KEYD_F4         (0x80+0x3e)
#define KEYD_F5         (0x80+0x3f)
#define KEYD_F6         (0x80+0x40)
#define KEYD_F7         (0x80+0x41)
#define KEYD_F8         (0x80+0x42)
#define KEYD_F9         (0x80+0x43)
#define KEYD_F10        (0x80+0x44)
#define KEYD_F11        (0x80+0x57)
#define KEYD_F12        (0x80+0x58)
#define KEYD_BACKSPACE  127
#define KEYD_PAUSE      0xff
#define KEYD_EQUALS     0x3d
#define KEYD_MINUS      0x2d
#define KEYD_RSHIFT     (0x80+0x36)
#define KEYD_RCTRL      (0x80+0x1d)
#define KEYD_RALT       (0x80+0x38)
#define KEYD_LALT       KEYD_RALT
#define KEYD_CAPSLOCK   0xba                                        // phares

// phares 3/2/98:
#define KEYD_INSERT     0xd2
#define KEYD_HOME       0xc7
#define KEYD_PAGEUP     0xc9
#define KEYD_PAGEDOWN   0xd1
#define KEYD_DEL        0xc8
#define KEYD_END        0xcf
#define KEYD_SCROLLLOCK 0xc6
#define KEYD_SPACEBAR   0x20
// phares 3/2/98

#define KEYD_NUMLOCK    0xC5                 // killough 3/6/98

// cph - Add the numeric keypad keys, as suggested by krose 4/22/99:
// The way numbers are assigned to keys is a mess, but it's too late to
// change that easily. At least these additions are don neatly.
// Codes 0x100-0x200 are reserved for number pad

#define KEYD_KEYPAD0      (0x100 + '0')
#define KEYD_KEYPAD1      (0x100 + '1')
#define KEYD_KEYPAD2      (0x100 + '2')
#define KEYD_KEYPAD3      (0x100 + '3')
#define KEYD_KEYPAD4      (0x100 + '4')
#define KEYD_KEYPAD5      (0x100 + '5')
#define KEYD_KEYPAD6      (0x100 + '6')
#define KEYD_KEYPAD7      (0x100 + '7')
#define KEYD_KEYPAD8      (0x100 + '8')
#define KEYD_KEYPAD9      (0x100 + '9')
#define KEYD_KEYPADENTER  (0x100 + KEYD_ENTER)
#define KEYD_KEYPADDIVIDE (0x100 + '/')
#define KEYD_KEYPADMULTIPLY (0x100 + '*')
#define KEYD_KEYPADMINUS  (0x100 + '-')
#define KEYD_KEYPADPLUS   (0x100 + '+')
#define KEYD_KEYPADPERIOD (0x100 + '.')

// phares 4/19/98:
// Defines Setup Screen groups that config variables appear in.
// Used when resetting the defaults for every item in a Setup group.

typedef enum {
  ss_none,
  ss_keys,
  ss_weap,
  ss_stat,
  ss_auto,
  ss_enem,
  ss_mess,
  ss_chat,
  ss_gen,       /* killough 10/98 */
  ss_comp,      /* killough 10/98 */
  ss_max
} ss_types;

// phares 3/20/98:
//
// Player friction is variable, based on controlling
// linedefs. More friction can create mud, sludge,
// magnetized floors, etc. Less friction can create ice.

#define MORE_FRICTION_MOMENTUM 15000       // mud factor based on momentum
#define ORIG_FRICTION          0xE800      // original value
#define ORIG_FRICTION_FACTOR   2048        // original value

#endif          // __DOOMDEF__
