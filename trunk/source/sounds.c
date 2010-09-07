/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 * $Id: sounds.c,v 1.4.2.1 2002/07/20 18:08:37 proff_fs Exp $
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
 *      Created by a sound utility.
 *      Kept as a sample, DOOM2 sounds.
 *
 *-----------------------------------------------------------------------------*/

static const char
rcsid[] = "$Id: sounds.c,v 1.4.2.1 2002/07/20 18:08:37 proff_fs Exp $";

// killough 5/3/98: reformatted

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "doomtype.h"
#include "sounds.h"

//
// Information about all the music
//
//kaiser
musicinfo_t S_music[] = {
  { 0 },
  { "action", 0 },//1
  { "tavern", 0 },//2
  { "danger", 0 },//3
  { "fast", 0 },//4
  { "intro", 0 },//5
  { "darker", 0 },//6
  { "strike", 0 },//7
  { "slide", 0 },//8
  { "tribal", 0 },//9
  { "march", 0 },//10
  { "mood", 0 },//11
  { "castle", 0 },//12
  { "fight", 0 },//13
  { "spense", 0 },//14
  { "dark", 0 },//15
  { "tech", 0 },//16
  { "drone", 0 },//17
  { "panthr", 0 },//18
  { "sad", 0 },//19
  { "instry", 0 },//20
  { "happy", 0 },//21
  { "end", 0 },//22
  { "logo", 0 },//23
  //shareware mus
  { "fmintr", 0 },
  { "fmstl", 0 },
  { "fmfast", 0 },
  { "fmact", 0 },
  { "fmdrne", 0 },
  { "map1", 0 },
  { "map2", 0 },
  { "map3", 0 },
  { "strife",0},
};


//
// Information about all the sfx
//

sfxinfo_t S_sfx[] = {
  // S_sfx[0] needs to be a dummy for odd reasons.
  { "none", false,  0, 0, -1, -1, 0 },
  //============Strife Sounds================kaiser

  { "swish", false, 64, 0, -1, -1, 0 },
  { "meatht", false, 64, 0, -1, -1, 0 },
  { "mtalht", false, 64, 0, -1, -1, 0 },
  { "wpnup", false, 72, 0, -1, -1, 0 },
  { "rifle", false, 64, 0, -1, -1, 0 },
  { "mislht", false, 64, 0, -1, -1, 0 },
  { "barexp", false, 72, 0, -1, -1, 0 },
  { "flburn", false, 64, 0, -1, -1, 0 },
  { "flidl", false, 118, 0, -1, -1, 0 },
  { "agrsee", false, 98, 0, -1, -1, 0 },
  { "plpain", false, 72, 0, -1, -1, 0 },
  { "pcrush", false, 96, 0, -1, -1, 0 },
  { "pespna", false, 96, 0, -1, -1, 0 },
  { "pespnb", false, 96, 0, -1, -1, 0 },
  { "pespnc", false, 98, 0, -1, -1, 0 },
  { "pespnd", false, 98, 0, -1, -1, 0 },
  { "agrdpn", false, 98, 0, -1, -1, 0 },
  { "pldeth", false, 72, 0, -1, -1, 0 },
  { "plxdth", false, 32, 0, -1, -1, 0 },
  { "slop", false, 72, 0, -1, -1, 0 },
  { "rebdth", false, 98, 0, -1, -1, 0 },
  { "agrdth", false, 98, 0, -1, -1, 0 },
  { "lgfire", false, 211, 0, -1, -1, 0 },
  { "smfire", false, 211, 0, -1, -1, 0 },
  { "alarm", false, 210, 0, -1, -1, 0 },
  { "drlmto", false, 98, 0, -1, -1, 0 },
  { "drlmtc", false, 98, 0, -1, -1, 0 },
  { "drsmto", false, 98, 0, -1, -1, 0 },
  { "drsmtc", false, 98, 0, -1, -1, 0 },
  { "drlwud", false, 98, 0, -1, -1, 0 },
  { "drswud", false, 98, 0, -1, -1, 0 },
  { "drston", false, 98, 0, -1, -1, 0 },
  { "bdopn", false, 72, 0, -1, -1, 0 },
  { "bdcls", false, 72, 0, -1, -1, 0 },
  { "swtchn", false, 72, 0, -1, -1, 0 },
  { "swbolt", false, 98, 0, -1, -1, 0 },
  { "swscan", false, 98, 0, -1, -1, 0 },
  { "yeah", false, 10, 0, -1, -1, 0 },
  { "mask", false, 210, 0, -1, -1, 0 },
  { "pstart", false, 72, 0, -1, -1, 0 },
  { "pstop", false, 72, 0, -1, -1, 0 },
  { "itemup", false, 72, 0, -1, -1, 0 },
  { "bglass", false, 200, 0, -1, -1, 0 },
  { "wriver", false, 201, 0, -1, -1, 0 },
  { "wfall", false, 201, 0, -1, -1, 0 },
  { "wdrip", false, 201, 0, -1, -1, 0 },
  { "wsplsh", false, 95, 0, -1, -1, 0 },
  { "rebact", false, 200, 0, -1, -1, 0 },
  { "agrac1", false, 98, 0, -1, -1, 0 },
  { "agrac2", false, 98, 0, -1, -1, 0 },
  { "agrac3", false, 98, 0, -1, -1, 0 },
  { "agrac4", false, 98, 0, -1, -1, 0 },
  { "ambppl", false, 218, 0, -1, -1, 0 },
  { "ambbar", false, 218, 0, -1, -1, 0 },
  { "telept", false, 72, 0, -1, -1, 0 },
  { "ratact", false, 99, 0, -1, -1, 0 },
  { "itmbk", false, 72, 0, -1, -1, 0 },
  { "xbow", false, 99, 0, -1, -1, 0 },
  { "burnme", false, 95, 0, -1, -1, 0 },
  { "oof", false, 72, 0, -1, -1, 0 },
  { "wbrldt", false, 98, 0, -1, -1, 0 },
  { "psdtha", false, 109, 0, -1, -1, 0 },
  { "psdthb", false, 109, 0, -1, -1, 0 },
  { "psdthc", false, 109, 0, -1, -1, 0 },
  { "rb2pn", false, 96, 0, -1, -1, 0 },
  { "rb2dth", false, 32, 0, -1, -1, 0 },
  { "rb2see", false, 98, 0, -1, -1, 0 },
  { "rb2act", false, 98, 0, -1, -1, 0 },
  { "firxpl", false, 72, 0, -1, -1, 0 },
  { "stnmov", false, 72, 0, -1, -1, 0 },
  { "noway", false, 72, 0, -1, -1, 0 },
  { "rlaunc", false, 72, 0, -1, -1, 0 },
  { "rflite", false, 65, 0, -1, -1, 0 },
  { "radio", false, 72, 0, -1, -1, 0 },
  { "pulchn", false, 98, 0, -1, -1, 0 },
  { "swknob", false, 98, 0, -1, -1, 0 },
  { "keycrd", false, 98, 0, -1, -1, 0 },
  { "swston", false, 98, 0, -1, -1, 0 },
  { "sntsee", false, 98, 0, -1, -1, 0 },
  { "sntdth", false, 98, 0, -1, -1, 0 },
  { "sntact", false, 98, 0, -1, -1, 0 },
  { "pgrdat", false, 64, 0, -1, -1, 0 },
  { "pgrsee", false, 90, 0, -1, -1, 0 },
  { "pgrdpn", false, 96, 0, -1, -1, 0 },
  { "pgrdth", false, 32, 0, -1, -1, 0 },
  { "pgract", false, 120, 0, -1, -1, 0 },
  { "proton", false, 64, 0, -1, -1, 0 },
  { "protfl", false, 64, 0, -1, -1, 0 },
  { "plasma", false,72, 0, -1, -1, 0 },
  { "dsrptr", false, 30, 0, -1, -1, 0 },
  { "reavat", false, 64, 0, -1, -1, 0 },
  { "revbld", false, 64, 0, -1, -1, 0 },
  { "revsee", false, 90, 0, -1, -1, 0 },
  { "reavpn", false, 96, 0, -1, -1, 0 },
  { "revdth", false, 32, 0, -1, -1, 0 },
  { "revact", false, 120, 0, -1, -1, 0 },
  { "spisit", false, 72, 0, -1, -1, 0 },
  { "spdwlk", false, 65, 0, -1, -1, 0 },
  { "spidth", false, 72, 0, -1, -1, 0 },
  { "spdatk", false, 32, 0, -1, -1, 0 },
  { "chant", false, 218, 0, -1, -1, 0 },
  { "static", false, 32, 0, -1, -1, 0 },
  { "chain", false, 70, 0, -1, -1, 0 },
  { "tend", false, 100, 0, -1, -1, 0 },
  { "phoot", false, 32, 0, -1, -1, 0 },
  { "explod", false, 32, 0, -1, -1, 0 },
  { "sigil", false, 32, 0, -1, -1, 0 },
  { "sglhit", false, 32, 0, -1, -1, 0 },
  { "siglup", false, 32, 0, -1, -1, 0 },
  { "prgpn", false, 96, 0, -1, -1, 0 },
  { "progac", false, 120, 0, -1, -1, 0 },
  { "lorpn", false, 96, 0, -1, -1, 0 },
  { "lorsee", false, 90, 0, -1, -1, 0 },
  { "difool", false, 32, 0, -1, -1, 0 },
  { "inqdth", false, 32, 0, -1, -1, 0 },
  { "inqact", false, 98, 0, -1, -1, 0 },
  { "inqsee", false, 90, 0, -1, -1, 0 },
  { "inqjmp", false, 65, 0, -1, -1, 0 },
  { "amaln1", false, 99, 0, -1, -1, 0 },
  { "amaln2", false, 99, 0, -1, -1, 0 },
  { "amaln3", false, 99, 0, -1, -1, 0 },
  { "amaln4", false, 99, 0, -1, -1, 0 },
  { "amaln5", false, 99, 0, -1, -1, 0 },
  { "amaln6", false, 99, 0, -1, -1, 0 },
  { "mnalse", false, 64, 0, -1, -1, 0 },
  { "alnsee", false, 64, 0, -1, -1, 0 },
  { "alnpn", false, 96, 0, -1, -1, 0 },
  { "alnact", false, 120, 0, -1, -1, 0 },
  { "alndth", false, 32, 0, -1, -1, 0 },
  { "mnaldt", false, 32, 0, -1, -1, 0 },
  { "reactr", false, 31, 0, -1, -1, 0 },
  { "airlck", false, 98, 0, -1, -1, 0 },
  { "drchno", false, 98, 0, -1, -1, 0 },
  { "drchnc", false, 98, 0, -1, -1, 0 },
  { "valve", false, 98, 0, -1, -1, 0 },

#ifdef DOGS
  // killough 11/98: dog sounds
  { "dgsit",  false,   98, 0, -1, -1, 0 },
  { "dgatk",  false,   70, 0, -1, -1, 0 },
  { "dgact",  false,  120, 0, -1, -1, 0 },
  { "dgdth",  false,   70, 0, -1, -1, 0 },
  { "dgpain", false,   96, 0, -1, -1, 0 },
#endif
};
