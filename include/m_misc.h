/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 * $Id: m_misc.h,v 1.6.2.1 2002/07/20 18:08:36 proff_fs Exp $
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
 *  External non-system-specific stuff, like storing config settings,
 *  simple file handling, and saving screnshots.
 *
 *-----------------------------------------------------------------------------*/


#ifndef __M_MISC__
#define __M_MISC__


#include "doomtype.h"
//
// MISC
//

boolean M_WriteFile (char const* name,void* source,int length);

int M_ReadFile (char const* name,byte** buffer);

void M_ScreenShot (void);
void M_DoScreenShot (const char*); // cph

void M_LoadDefaults (void);

void M_SaveDefaults (void);

int M_CheckStringWidth(const char* string);
int M_CheckStringHeight(const char* string);


int nrNum10(int number);	//kaiser
char LogText[1024 / 4];	//for logs
int M_DrawText (int x,int y,boolean direct,char* string, boolean type);
void M_DrawTextW(int x, int y, boolean direct, char* string,int width, boolean type);
void LoadTextLump(char *name, char *buf, int bufsize);
extern char *sc_String;
extern int sc_Number;
extern int sc_Line;
extern boolean sc_End;
extern boolean sc_Crossed;
extern boolean sc_FileScripts;
extern char *sc_ScriptsDir;
void SC_Open(char *name);
void SC_OpenLump(char *name);
void SC_OpenFile(char *name);
void SC_OpenFileCLib(char *name);
void SC_Close(void);
boolean SC_GetString(void);
void SC_MustGetString(void);
void SC_MustGetStringName(char *name);
boolean SC_GetNumber(void);
void SC_MustGetNumber(void);
boolean SC_Compare(char *text);
int SC_MatchString(char **strings);
int SC_MustMatchString(char **strings);

struct default_s *M_LookupDefault(const char *name);     /* killough 11/98 */

// phares 4/21/98:
// Moved from m_misc.c so m_menu.c could see it.

// CPhipps - struct to hold a value in a config file
// Cannot be a union, as it must be initialised
typedef struct default_s
{
  const char* name;
  /* cph -
   * The location struct holds the pointer to the variable holding the
   *  setting. For int's we do nothing special.
   * For strings, the string is actually stored on our heap with Z_Strdup()
   *  BUT we don't want the rest of the program to be able to modify them,
   *  so we declare it const. It's not really const though, and m_misc.c and
   *  m_menu.c cast it back when they need to change it. Possibly this is
   *  more trouble than it's worth.
   */
  struct {
    int* pi;
    const char** ppsz;
  } location;
  struct {
    int i;
    const char* psz;
  } defaultvalue; // CPhipps - default value
  // Limits (for an int)
  int   minvalue;         // jff 3/3/98 minimum allowed value
  int   maxvalue;         // jff 3/3/98 maximum allowed value
  enum {
    def_none, // Dummy entry
    def_str,  // A string
    def_int,  // Integer
    def_hex,  // Integer (write in hex)
    def_bool = def_int,  // Boolean
    def_key = def_hex,   // Key code (byte)
    def_mouseb = def_int,// Mouse button
    def_colour = def_hex // Colour (256 colour palette entry)
  } type; // CPhipps - type of entry
  int   setupscreen;      // phares 4/19/98: setup screen where this appears
  int  *current;          /* cph - MBF-like pointer to current value */
  // cph - removed the help strings from the config file
  // const char* help;       // jff 3/3/98 description of parameter
  // CPhipps - remove unused "lousy hack" code
  struct setup_menu_s *setup_menu;   /* Xref to setup menu item, if any */
} default_t;

#define IS_STRING(dv) ((dv).type == def_str)
// CPhipps - What is the max. key code that X will send us?
#define MAX_KEY 65536
#define MAX_MOUSEB 2

#define UL (-123456789) /* magic number for no min or max for parameter */

#endif
