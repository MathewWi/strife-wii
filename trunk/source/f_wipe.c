/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 * $Id: f_wipe.c,v 1.5.2.1 2002/07/20 18:08:34 proff_fs Exp $
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
 *      Mission begin melt/wipe screen special effect.
 *
 *-----------------------------------------------------------------------------
 */

static const char rcsid[] = "$Id: f_wipe.c,v 1.5.2.1 2002/07/20 18:08:34 proff_fs Exp $";

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "z_zone.h"
#include "doomdef.h"
#include "i_video.h"
#include "v_video.h"
#include "m_random.h"
#include "f_wipe.h"
#include "w_wad.h"

//
// SCREEN WIPE PACKAGE
//

//1-12-06 - Implented Strife's version of f_wipe. Screen fades instead of melt.

// CPhipps - macros for the source and destination screens
#define SRC_SCR 2
#define DEST_SCR 3
#define FADESPEED	4

static byte *wipe_scr_start;
static byte *wipe_scr_end;
static byte *wipe_scr;
//Strife screen fade stuff
static int fadeTic;
extern byte *xlatabtable;

//strife version		-kaiser
static int wipe_doMelt(int width, int height, int ticks)
{
	boolean done = true;
	int i;
	if((fadeTic % FADESPEED == 0) && ticks--)
	{
		for(i = 0; i < width*height; i++)
		{
			wipe_scr_start[i] = xlatabtable[(wipe_scr_start[i]*256) + wipe_scr_end[i]];
		}
		memcpy(wipe_scr, wipe_scr_start, width*height);
	}
	fadeTic++;
	if(fadeTic != (FADESPEED * 10))
	{
		done = false;
	}
	return done;
}

// CPhipps - modified to allocate and deallocate screens[2 to 3] as needed, saving memory

static int wipe_exitMelt(int width, int height, int ticks)
{
  Z_Free(wipe_scr_start);
  Z_Free(wipe_scr_end);
  wipe_scr_start = wipe_scr_end = screens[SRC_SCR] = screens[DEST_SCR] = NULL;
  return 0;
}

int wipe_StartScreen(int x, int y, int width, int height)
{
  wipe_scr_start = screens[SRC_SCR] = malloc(SCREENWIDTH * SCREENHEIGHT);
  V_CopyRect(x, y, 0,       width, height, x, y, SRC_SCR, VPT_NONE ); // Copy start screen to buffer
  return 0;
}

int wipe_EndScreen(int x, int y, int width, int height)
{
  wipe_scr_end = screens[DEST_SCR] = malloc(SCREENWIDTH * SCREENHEIGHT);
  V_CopyRect(x, y, 0,       width, height, x, y, DEST_SCR, VPT_NONE); // Copy end screen to buffer
  V_CopyRect(x, y, SRC_SCR, width, height, x, y, 0       , VPT_NONE); // restore start screen
  return 0;
}

// killough 3/5/98: reformatted and cleaned up
int wipe_ScreenWipe(int x, int y, int width, int height, int ticks)
{
  static boolean go;                               // when zero, stop the wipe
  if (!go)                                         // initial stuff
    {
      go = 1;
      wipe_scr = screens[0];
    }
  V_MarkRect(0, 0, width, height);                 // do a piece of wipe-in
  if (wipe_doMelt(width, height, ticks))     // final stuff
    {
      wipe_exitMelt(width, height, ticks);
      go = 0;
	  fadeTic = 0;
    }
  return !go;
}
