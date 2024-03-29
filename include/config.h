/* Emacs style mode select   -*- C++ -*- 
 *-----------------------------------------------------------------------------
 *
 * $Id: config.h,v 1.14.2.5 2002/07/27 16:00:09 proff_fs Exp $
 *
 *  PrBoom a Doom port merged with LxDoom and LSDLDoom
 *  based on BOOM, a modified and improved DOOM engine
 *  Copyright (C) 1999 by
 *  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
 *  Copyright (C) 1999-2000 by
 *  Colin Phipps (cph@lxdoom.linuxgames.com), 
 *  Jess Haas (JessH@lbjhs.net)
 *  and Florian Schulze (florian.proff.schulze@gmx.net)
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
 *  This is the config file for Windows
 *
 *-----------------------------------------------------------------------------*/

#define inline __inline

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if your processor stores words with the most significant
   byte first (like Motorola and SPARC, unlike Intel and VAX).  */
/* #undef WORDS_BIGENDIAN */

/* Define to strcasecmp, if we have it */
#define stricmp strcasecmp

/* Define to strncasecmp, if we have it */
#define strnicmp strncasecmp

/* Define on targets supporting 386 Assembly */
/* This is in the project settings */
/* #define I386_ASM 1 */

/* Define to enable internal range checking */
/* #undef RANGECHECK */

/* Define this to see real-time memory allocation
 * statistics, and enable extra debugging features 
 */
/* #undef INSTRUMENTED */

/* Uncomment this to exhaustively run memory checks
 * while the game is running (this is EXTREMELY slow).
 * Only useful if INSTRUMENTED is also defined.
 */
/* #undef CHECKHEAP */

/* Uncomment this to cause heap dumps to be generated.
 * Only useful if INSTRUMENTED is also defined.
 */
/* #undef HEAPDUMP */

/* Uncomment this to perform id checks on zone blocks,
 * to detect corrupted and illegally freed blocks
 */
#define ZONEIDCHECK 1

/* CPhipps - some debugging macros for the new wad lump handling code */
/* Defining this causes quick checks which only impose an overhead if a 
 *  posible error is detected. */
#define SIMPLECHECKS 1

/* Defining this causes time stamps to be created each time a lump is locked, and 
 *  lumps locked for long periods of time are reported */
/* #undef TIMEDIAG */

/* Define to be the path where Doom WADs are stored */
#define DOOMWADDIR "sd:/apps/strifewii/data" //Note to self: No '/' after data

/* Define if you have the SDL mixer library -lSDL_mixer */
//#define HAVE_LIBSDL_MIXER 1

/* Define if you want network game support */
//#define HAVE_NET 1
//#define USE_SDL_NET 1

/* Define if you have struct sockaddr_in6 */
/* #define HAVE_IPv6 1 */

/* Define if you have the inet_aton function.  */
/* #define HAVE_INET_ATON 1 */

/* Define if you have the inet_ntop function.  */
/* #define HAVE_INET_NTOP 1 */

/* Define if you have the inet_pton function.  */
/* #define HAVE_INET_PTON 1 */

/* Define if you have the setsockopt function.  */
/* #define HAVE_SETSOCKOPT 1 */

/* Define if you have the snprintf function.  */
//#define HAVE_SNPRINTF 1
#//define snprintf _snprintf

/* Define if you have the vsnprintf function.  */
//#define HAVE_VSNPRINTF 1
//#define vsnprintf _vsnprintf

/* Define for support for MBF helper dogs */
#define DOGS 1

/* Define if you want to have boomlump.wad in the exe  */
#define ALL_IN_ONE 1

/* Define if you want to compile with SDL  */
#define USE_SDL 1

#define MONITOR_VISIBILITY 1

/* Define if you want to use the gluTesselator  */
//#define USE_GLU_TESS 1

/* Define if you want to use gluImageScale  */
//#define USE_GLU_IMAGESCALE 1

/* Define if you want to use gluBuild2DMipmaps  */
//#define USE_GLU_MIPMAP 1

/* Set to the attribute to apply to struct definitions to make them packed.
 * For MSVC++ we can't do it like this, there are pragma's in the source 
 * instead. */
#define PACKEDATTR

/* Name of package */
#define PACKAGE "svstrife"	//kaiser

/* Version number of package */
#define VERSION "1.4"

#define WORDS_BIGENDIAN 1
