/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 * $Id: i_system.c,v 1.6.2.3 2002/07/21 10:20:47 cph Exp $
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
 *  Misc system stuff needed by Doom, implemented for Linux.
 *  Mainly timer handling, and ENDOOM/ENDBOOM.
 *
 *-----------------------------------------------------------------------------
 */

static const char
rcsid[] = "$Id: i_system.c,v 1.6.2.3 2002/07/21 10:20:47 cph Exp $";

#include <stdio.h>

#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
#ifdef _MSC_VER
#define    F_OK    0    /* Check for file existence */
#define    W_OK    2    /* Check for write permission */
#define    R_OK    4    /* Check for read permission */
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#endif
#include <sys/stat.h>

#include <SDL/SDL.h>

#include "i_system.h"
#include "m_argv.h"
#include "lprintf.h"
#include "doomtype.h"
#include "doomdef.h"

#ifdef __GNUG__
#pragma implementation "i_system.h"
#endif
#include "i_system.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

void I_uSleep(unsigned long usecs)
{
    SDL_Delay(usecs/1000);
}

int I_GetTime_RealTime (void)
{
  return (SDL_GetTicks()*TICRATE)/1000;
}

/*
 * I_GetRandomTimeSeed
 *
 * CPhipps - extracted from G_ReloadDefaults because it is O/S based
 */
unsigned long I_GetRandomTimeSeed(void)
{
/* This isnt very random */
  return(SDL_GetTicks());
}

/* cphipps - I_GetVersionString
 * Returns a version string in the given buffer
 */
const char* I_GetVersionString(char* buf, size_t sz)
{
#ifdef HAVE_SNPRINTF
  snprintf(buf,sz,"%s v%s (email: svkaiser@gmail.com)",PACKAGE,VERSION);
#else
  sprintf(buf,"%s v%s (email: svkaiser@gmail.com)",PACKAGE,VERSION);
#endif
  return buf;
}

/* cphipps - I_SigString
 * Returns a string describing a signal number
 */
const char* I_SigString(char* buf, size_t sz, int signum)
{
#ifdef SYS_SIGLIST_DECLARED
  if (strlen(sys_siglist[signum]) < sz)
    strcpy(buf,sys_siglist[signum]);
  else
#endif
    sprintf(buf,"signal %d",signum);
  return buf;
}

#ifndef PRBOOM_SERVER

// Return the path where the executable lies -- Lee Killough
// proff_fs 2002-07-04 - moved to i_system
#ifdef _WIN32
char *I_DoomExeDir(void)
{
  static const char current_dir_dummy[] = {"./"};
  static char *base;
  if (!base)        // cache multiple requests
    {
      size_t len = strlen(*myargv);
      char *p = (base = malloc(len+1)) + len - 1;
      strcpy(base,*myargv);
      while (p > base && *p!='/' && *p!='\\')
        *p--=0;
      if (*p=='/' || *p=='\\')
        *p--=0;
      if (strlen(base)<2)
      {
        free(base);
        base = malloc(1024);
        if (!getcwd(base,1024))
          strcpy(base, current_dir_dummy);
      }
    }
  return base;
}
#else
// cph - V.Aguilar (5/30/99) suggested return ~/.lxdoom/, creating
//  if non-existant
static const char prboom_dir[] = {"/.prboom/"};

char *I_DoomExeDir(void)
{
  static char *base;
  if (!base)        // cache multiple requests
    {
  //Determine SD or USB
  FILE * fp2;
  boolean sd = false;
  boolean usb = false;
  fp2 = fopen("sd:/apps/strifewii/data/prboom.wad", "rb");
  if(fp2)
  sd = true;
  if(!fp2){
  fp2 = fopen("usb:/apps/strifewii/data/prboom.wad", "rb");
  }
  if(fp2 && !sd)
  usb = true;
  
      if(sd)
      base = malloc(strlen("sd:/apps/strifewii/data") + 1);
          if(usb)
          base = malloc(strlen("usb:/apps/strifewii/data") + 1);
          
      if(sd)
      strcat(base, "sd:/apps/strifewii/data");
          if(usb)
          strcat(base, "usb:/apps/strifewii/data");

      //mkdir(base, S_IRUSR | S_IWUSR | S_IXUSR); // Make sure it exists
    }
  return base;
}

#endif

/*
 * HasTrailingSlash
 *
 * cphipps - simple test for trailing slash on dir names
 */

static boolean HasTrailingSlash(const char* dn)
{
  return (dn[strlen(dn)-1] == '/');
}

/*
 * I_FindFile
 *
 * proff_fs 2002-07-04 - moved to i_system
 *
 * cphipps 19/1999 - writen to unify the logic in FindIWADFile and the WAD
 *      autoloading code.
 * Searches the standard dirs for a named WAD file
 * The dirs are:
 * .
 * DOOMWADDIR
 * ~/doom
 * /usr/share/games/doom
 * /usr/local/share/games/doom
 * ~
 */

char* I_FindFile(const char* wfname, const char* ext)
{
  //size_t  pl = strlen(wfname) + strlen(ext) + 4;
  
        //Determine SD or USB
    FILE * fp2;
    boolean sd = false;
    boolean usb = false;
    fp2 = fopen("sd:/apps/strifewii/data/prboom.wad", "rb");
    if(fp2)
    sd = true;
    if(!fp2){
    fp2 = fopen("usb:/apps/strifewii/data/prboom.wad", "rb");
    }
    if(fp2 && !sd)
    usb = true;
        
        if(fp2)
        fclose(fp2);

        char *p;
        if(sd)
        p = "sd:/apps/strifewii/data/";
        if(usb)
        p = "usb:/apps/strifewii/data/";
        
        char *f;
        f = malloc(strlen(p) + strlen(wfname) + 4);
        sprintf(f, "%s%s", p, wfname);
        if (fopen(f, "r"))
        {
                return f;
        }
        else
        {
                return NULL;
        }
}
#endif