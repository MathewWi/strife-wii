/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 * $Id: d_englsh.h,v 1.3.2.1 2002/07/20 18:08:34 proff_fs Exp $
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
 *  Printed strings for translation.
 *  English language support (default).
 *  See dstrings.h for suggestions about foreign language BEX support
 *
 *-----------------------------------------------------------------------------*/

#ifndef __D_ENGLSH__
#define __D_ENGLSH__

/* d_main.c */
#define D_DEVSTR    "Development mode ON.\n"
#define D_CDROM     "CD-ROM Version: default.cfg from c:\\doomdata\n"

/* m_menu.c */
#define PRESSKEY    "press a key."
#define PRESSYN     "press y or n."
#define QUITMSG     "are you sure you want to\nquit this great game?"
#define LOADNET     "you can't do load while in a net game!\n\n"PRESSKEY
#define QLOADNET    "you can't quickload during a netgame!\n\n"PRESSKEY
#define QSAVESPOT   "you haven't picked a quicksave slot yet!\n\n"PRESSKEY
#define SAVEDEAD    "you can't save if you aren't playing!\n\n"PRESSKEY
#define QSPROMPT    "quicksave over your game named\n\n'%s'?\n\n"PRESSYN
#define QLPROMPT    "do you want to quickload the game named\n\n'%s'?\n\n"PRESSYN

#define NEWGAME \
  "you can't start a new game\n"\
  "while in a network game.\n\n"PRESSKEY

#define NIGHTMARE \
  "are you sure? this skill level\n"\
  "isn't even remotely fair.\n\n"PRESSYN

#define SWSTRING  \
  "this is the shareware version of doom.\n\n"\
  "you need to order the entire trilogy.\n\n"PRESSKEY

#define MSGOFF      "Messages OFF"
#define MSGON       "Messages ON"
#define NETEND      "you can't end a netgame!\n\n"PRESSKEY
#define ENDGAME     "are you sure you want to end the game?\n\n"PRESSYN
#define RESTARTLEVEL "restart the level?\n\n"PRESSYN

#define DOSY        "(press y to quit)"

#define DETAILHI    "High detail"
#define DETAILLO    "Low detail"
#define GAMMALVL0   "Gamma correction OFF"
#define GAMMALVL1   "Gamma correction level 1"
#define GAMMALVL2   "Gamma correction level 2"
#define GAMMALVL3   "Gamma correction level 3"
#define GAMMALVL4   "Gamma correction level 4"
#define EMPTYSTRING "empty slot"

/* p_inter.c */
#define GOTARMOR    "Picked up the armor."
#define GOTMEGA     "Picked up the MegaArmor!"
#define GOTHTHBONUS "Picked up a health bonus."
#define GOTARMBONUS "Picked up an armor bonus."
#define GOTSTIM     "Picked up a stimpack."
#define GOTMEDINEED "Picked up a medikit that you REALLY need!"
#define GOTMEDIKIT  "Picked up a medikit."
#define GOTSUPER    "Supercharge!"

#define GOTBLUECARD "Picked up a blue keycard."
#define GOTYELWCARD "Picked up a yellow keycard."
#define GOTREDCARD  "Picked up a red keycard."
#define GOTBLUESKUL "Picked up a blue skull key."
#define GOTYELWSKUL "Picked up a yellow skull key."
#define GOTREDSKULL "Picked up a red skull key."

#define GOTINVUL    "Invulnerability!"
#define GOTBERSERK  "Berserk!"
#define GOTINVIS    "Partial Invisibility"
#define GOTSUIT     "Radiation Shielding Suit"
#define GOTMAP      "Computer Area Map"
#define GOTVISOR    "Light Amplification Visor"
#define GOTMSPHERE  "MegaSphere!"

#define GOTCLIP     "Picked up a clip."
#define GOTCLIPBOX  "Picked up a box of bullets."
#define GOTROCKET   "Picked up a rocket."
#define GOTROCKBOX  "Picked up a box of rockets."
#define GOTCELL     "Picked up an energy cell."
#define GOTCELLBOX  "Picked up an energy cell pack."
#define GOTSHELLS   "Picked up 4 shotgun shells."
#define GOTSHELLBOX "Picked up a box of shotgun shells."
#define GOTBACKPACK "Picked up a backpack full of ammo!"
//===========Strife Ammo Texts=============-kaiser
//I've decided to leave the doom2 ammo texts alone for now
#define GOTBULLET	"Picked up a bullet."
#define GOTAMMO		"Picked up a box of bullets. "
#define GOTARROWS	"Picked up some electric arrows. "
#define GOTPARROWS	"Picked up some poison arrows. "
#define GOTMINIM	"Picked up mini missiles. "
#define GOTMINIBOX	"Picked up a box of mini missiles "
#define GOTGREN		"Picked up some grenades. "
#define GOTGREN2	"Picked up some explosive grenades. "
#define GOTCELL1	"Picked up a cell. "
#define GOTCELLPACK	"Picked up a cell pack. "	
//===================================================

#define GOTBFG9000  "You got the BFG9000!  Oh, yes."
#define GOTCHAINGUN "You got the chaingun!"
#define GOTCHAINSAW "A chainsaw!  Find some meat!"
#define GOTLAUNCHER "You got the rocket launcher!"
#define GOTPLASMA   "You got the plasma gun!"
#define GOTSHOTGUN  "You got the shotgun!"
#define GOTSHOTGUN2 "You got the super shotgun!"
//===========Strife Weapon Texts=============-kaiser
//I've decided to leave the doom2 weapon texts alone for now
#define	GOTCROSSBOW	"You got the Crossbow!"
#define GOTRIFLE	"You got the Assault Rifle!"
#define GOTMINI		"You got the Mini Missile Launcher!"
#define GOTGRENADE	"You got the Grenade Launcher!"
#define GOTFLAME	"You got the Flame Thrower!"
#define GOTBLASTER	"You got the Mauler!"

/* p_doors.c */
#define PD_BLUEO    "You need a blue key to activate this object"
#define PD_REDO     "You need a red key to activate this object"
#define PD_YELLOWO  "You need a yellow key to activate this object"
#define PD_BLUEK    "You need a blue key to open this door"
#define PD_REDK     "You need a red key to open this door"
#define PD_YELLOWK  "You need a yellow key to open this door"
/* jff 02/05/98 Create messages specific to card and skull keys */
#define PD_BLUEC    "You need a blue card to open this door"
#define PD_REDC     "You need a red card to open this door"
#define PD_YELLOWC  "You need a yellow card to open this door"
#define PD_BLUES    "You need a blue skull to open this door"
#define PD_REDS     "You need a red skull to open this door"
#define PD_YELLOWS  "You need a yellow skull to open this door"
#define PD_ANY      "Any key will open this door"
#define PD_ALL3     "You need all three keys to open this door"
#define PD_ALL6     "You need all six keys to open this door"

/* g_game.c */
#define GGSAVED     "game saved."

/* hu_stuff.c */
#define HUSTR_MSGU  "[Message unsent]"

#define HUSTR_E1M1  "E1M1: Hangar"
#define HUSTR_E1M2  "E1M2: Nuclear Plant"
#define HUSTR_E1M3  "E1M3: Toxin Refinery"
#define HUSTR_E1M4  "E1M4: Command Control"
#define HUSTR_E1M5  "E1M5: Phobos Lab"
#define HUSTR_E1M6  "E1M6: Central Processing"
#define HUSTR_E1M7  "E1M7: Computer Station"
#define HUSTR_E1M8  "E1M8: Phobos Anomaly"
#define HUSTR_E1M9  "E1M9: Military Base"

#define HUSTR_E2M1  "E2M1: Deimos Anomaly"
#define HUSTR_E2M2  "E2M2: Containment Area"
#define HUSTR_E2M3  "E2M3: Refinery"
#define HUSTR_E2M4  "E2M4: Deimos Lab"
#define HUSTR_E2M5  "E2M5: Command Center"
#define HUSTR_E2M6  "E2M6: Halls of the Damned"
#define HUSTR_E2M7  "E2M7: Spawning Vats"
#define HUSTR_E2M8  "E2M8: Tower of Babel"
#define HUSTR_E2M9  "E2M9: Fortress of Mystery"

#define HUSTR_E3M1  "E3M1: Hell Keep"
#define HUSTR_E3M2  "E3M2: Slough of Despair"
#define HUSTR_E3M3  "E3M3: Pandemonium"
#define HUSTR_E3M4  "E3M4: House of Pain"
#define HUSTR_E3M5  "E3M5: Unholy Cathedral"
#define HUSTR_E3M6  "E3M6: Mt. Erebus"
#define HUSTR_E3M7  "E3M7: Limbo"
#define HUSTR_E3M8  "E3M8: Dis"
#define HUSTR_E3M9  "E3M9: Warrens"

#define HUSTR_E4M1  "E4M1: Hell Beneath"
#define HUSTR_E4M2  "E4M2: Perfect Hatred"
#define HUSTR_E4M3  "E4M3: Sever The Wicked"
#define HUSTR_E4M4  "E4M4: Unruly Evil"
#define HUSTR_E4M5  "E4M5: They Will Repent"
#define HUSTR_E4M6  "E4M6: Against Thee Wickedly"
#define HUSTR_E4M7  "E4M7: And Hell Followed"
#define HUSTR_E4M8  "E4M8: Unto The Cruel"
#define HUSTR_E4M9  "E4M9: Fear"

//kaiser
#define HUSTR_1     "AREA  1: sanctuary"
#define HUSTR_2     "AREA  2: town"
#define HUSTR_3     "AREA  3: front base"
#define HUSTR_4     "AREA  4: power station"
#define HUSTR_5     "AREA  5: prison"
#define HUSTR_6     "AREA  6: sewers"
#define HUSTR_7     "AREA  7: castle"
#define HUSTR_8     "AREA  8: Audience Chamber"
#define HUSTR_9     "AREA  9: Castle: Programmer's Keep"
#define HUSTR_10    "AREA 10: New Front Base"
#define HUSTR_11    "AREA 11: Borderlands"

#define HUSTR_12    "AREA 12: the temple of the oracle"
#define HUSTR_13    "AREA 13: Catacombs"
#define HUSTR_14    "AREA 14: mines"
#define HUSTR_15    "AREA 15: Fortress: Administration"
#define HUSTR_16    "AREA 16: Fortress: Bishop's Tower"
#define HUSTR_17    "AREA 17: Fortress: The Bailey"
#define HUSTR_18    "AREA 18: Fortress: Stores"
#define HUSTR_19    "AREA 19: Fortress: Security Complex"
#define HUSTR_20    "AREA 20: Factory: Receiving"

#define HUSTR_21    "AREA 21: Factory: Manufacturing"
#define HUSTR_22    "AREA 22: Factory: Forge"
#define HUSTR_23    "AREA 23: Order Commons"
#define HUSTR_24    "AREA 24: Factory: Conversion Chapel"
#define HUSTR_25    "AREA 25: Catacombs: Ruined Temple"
#define HUSTR_26    "AREA 26: proving grounds"
#define HUSTR_27    "AREA 27: The Lab"
#define HUSTR_28    "AREA 28: Alien Ship"
#define HUSTR_29    "AREA 29: Entity's Lair"
#define HUSTR_30    "AREA 30: Abandoned Front Base"

#define HUSTR_31    "AREA 31: Training Facility"
#define HUSTR_32    "AREA  1: Sanctuary"

#define PHUSTR_1    "AREA  2: Town"
#define PHUSTR_2    "AREA  3: Movement Base"
#define PHUSTR_3    "level 3: aztec"
#define PHUSTR_4    "level 4: caged"
#define PHUSTR_5    "level 5: ghost town"
#define PHUSTR_6    "level 6: baron's lair"
#define PHUSTR_7    "level 7: caughtyard"
#define PHUSTR_8    "level 8: realm"
#define PHUSTR_9    "level 9: abattoire"
#define PHUSTR_10   "level 10: onslaught"
#define PHUSTR_11   "level 11: hunted"

#define PHUSTR_12   "level 12: speed"
#define PHUSTR_13   "level 13: the crypt"
#define PHUSTR_14   "level 14: genesis"
#define PHUSTR_15   "level 15: the twilight"
#define PHUSTR_16   "level 16: the omen"
#define PHUSTR_17   "level 17: compound"
#define PHUSTR_18   "level 18: neurosphere"
#define PHUSTR_19   "level 19: nme"
#define PHUSTR_20   "level 20: the death domain"

#define PHUSTR_21   "level 21: slayer"
#define PHUSTR_22   "level 22: impossible mission"
#define PHUSTR_23   "level 23: tombstone"
#define PHUSTR_24   "level 24: the final frontier"
#define PHUSTR_25   "level 25: the temple of darkness"
#define PHUSTR_26   "level 26: bunker"
#define PHUSTR_27   "level 27: anti-christ"
#define PHUSTR_28   "level 28: the sewers"
#define PHUSTR_29   "level 29: odyssey of noises"
#define PHUSTR_30   "level 30: the gateway of hell"

#define PHUSTR_31   "level 31: cyberden"
#define PHUSTR_32   "level 32: go 2 it"

#define THUSTR_1    "level 1: system control"
#define THUSTR_2    "level 2: human bbq"
#define THUSTR_3    "level 3: power control"
#define THUSTR_4    "level 4: wormhole"
#define THUSTR_5    "level 5: hanger"
#define THUSTR_6    "level 6: open season"
#define THUSTR_7    "level 7: prison"
#define THUSTR_8    "level 8: metal"
#define THUSTR_9    "level 9: stronghold"
#define THUSTR_10   "level 10: redemption"
#define THUSTR_11   "level 11: storage facility"

#define THUSTR_12   "level 12: crater"
#define THUSTR_13   "level 13: nukage processing"
#define THUSTR_14   "level 14: steel works"
#define THUSTR_15   "level 15: dead zone"
#define THUSTR_16   "level 16: deepest reaches"
#define THUSTR_17   "level 17: processing area"
#define THUSTR_18   "level 18: mill"
#define THUSTR_19   "level 19: shipping/respawning"
#define THUSTR_20   "level 20: central processing"

#define THUSTR_21   "level 21: administration center"
#define THUSTR_22   "level 22: habitat"
#define THUSTR_23   "level 23: lunar mining project"
#define THUSTR_24   "level 24: quarry"
#define THUSTR_25   "level 25: baron's den"
#define THUSTR_26   "level 26: ballistyx"
#define THUSTR_27   "level 27: mount pain"
#define THUSTR_28   "level 28: heck"
#define THUSTR_29   "level 29: river styx"
#define THUSTR_30   "level 30: last call"

#define THUSTR_31   "level 31: pharaoh"
#define THUSTR_32   "level 32: caribbean"

#define HUSTR_CHATMACRO1  "I'm ready to kick butt!"
#define HUSTR_CHATMACRO2  "I'm OK."
#define HUSTR_CHATMACRO3  "I'm not looking too good!"
#define HUSTR_CHATMACRO4  "Help!"
#define HUSTR_CHATMACRO5  "You suck!"
#define HUSTR_CHATMACRO6  "Next time, scumbag..."
#define HUSTR_CHATMACRO7  "Come here!"
#define HUSTR_CHATMACRO8  "I'll take care of it."
#define HUSTR_CHATMACRO9  "Yes"
#define HUSTR_CHATMACRO0  "No"

#define HUSTR_TALKTOSELF1 "You mumble to yourself"
#define HUSTR_TALKTOSELF2 "Who's there?"
#define HUSTR_TALKTOSELF3 "You scare yourself"
#define HUSTR_TALKTOSELF4 "You start to rave"
#define HUSTR_TALKTOSELF5 "You've lost it..."

#define HUSTR_MESSAGESENT "[Message Sent]"

/* The following should NOT be changed unless it seems
 * just AWFULLY necessary */

#define HUSTR_PLRGREEN    "Player 1: "
#define HUSTR_PLRINDIGO   "Player 2: "
#define HUSTR_PLRBROWN    "Player 3: "
#define HUSTR_PLRRED      "Player 4: "

#define HUSTR_KEYGREEN    'g'
#define HUSTR_KEYINDIGO   'i'
#define HUSTR_KEYBROWN    'b'
#define HUSTR_KEYRED      'r'

/* am_map.c */

#define AMSTR_FOLLOWON    "Follow Mode ON"
#define AMSTR_FOLLOWOFF   "Follow Mode OFF"

#define AMSTR_GRIDON      "Grid ON"
#define AMSTR_GRIDOFF     "Grid OFF"

#define AMSTR_MARKEDSPOT  "Marked Spot"
#define AMSTR_MARKSCLEARED  "All Marks Cleared"

#define AMSTR_ROTATEON    "Rotate Mode ON"
#define AMSTR_ROTATEOFF   "Rotate Mode OFF"

#define AMSTR_OVERLAYON    "Overlay Mode ON"
#define AMSTR_OVERLAYOFF   "Overlay Mode OFF"

/* st_stuff.c */

#define STSTR_MUS       "Music Change"
#define STSTR_NOMUS     "IMPOSSIBLE SELECTION"
#define STSTR_DQDON     "You're Invincible!"
#define STSTR_DQDOFF    "You're a looney!"
#define STSTR_TRUMP		"You got the midias touch baby!"

#define STSTR_KFAADDED  "Ammo Added"
#define STSTR_FAADDED   "Ammo Added"

#define STSTR_NCON      "No Clipping Mode ON"
#define STSTR_NCOFF     "No Clipping Mode OFF"

#define STSTR_BEHOLD    "inVuln, Str, Inviso, Rad, Allmap, or Lite-amp"
#define STSTR_BEHOLDX   "Power-up Toggled"

#define STSTR_CHOPPERS  "... doesn't suck - GM"
#define STSTR_CLEV      "Changing Level..."

#define STSTR_COMPON    "Compatibility Mode On"            /* phares */
#define STSTR_COMPOFF   "Compatibility Mode Off"           /* phares */

/* f_finale.c */

#define E1TEXT \
  "Once you beat the big badasses and\n"\
  "clean out the moon base you're supposed\n"\
  "to win, aren't you? Aren't you? Where's\n"\
  "your fat reward and ticket home? What\n"\
  "the hell is this? It's not supposed to\n"\
  "end this way!\n"\
  "\n" \
  "It stinks like rotten meat, but looks\n"\
  "like the lost Deimos base.  Looks like\n"\
  "you're stuck on The Shores of Hell.\n"\
  "The only way out is through.\n"\
  "\n"\
  "To continue the DOOM experience, play\n"\
  "The Shores of Hell and its amazing\n"\
  "sequel, Inferno!\n"


#define E2TEXT \
  "You've done it! The hideous cyber-\n"\
  "demon lord that ruled the lost Deimos\n"\
  "moon base has been slain and you\n"\
  "are triumphant! But ... where are\n"\
  "you? You clamber to the edge of the\n"\
  "moon and look down to see the awful\n"\
  "truth.\n" \
  "\n"\
  "Deimos floats above Hell itself!\n"\
  "You've never heard of anyone escaping\n"\
  "from Hell, but you'll make the bastards\n"\
  "sorry they ever heard of you! Quickly,\n"\
  "you rappel down to  the surface of\n"\
  "Hell.\n"\
  "\n" \
  "Now, it's on to the final chapter of\n"\
  "DOOM! -- Inferno."


#define E3TEXT \
  "The loathsome spiderdemon that\n"\
  "masterminded the invasion of the moon\n"\
  "bases and caused so much death has had\n"\
  "its ass kicked for all time.\n"\
  "\n"\
  "A hidden doorway opens and you enter.\n"\
  "You've proven too tough for Hell to\n"\
  "contain, and now Hell at last plays\n"\
  "fair -- for you emerge from the door\n"\
  "to see the green fields of Earth!\n"\
  "Home at last.\n" \
  "\n"\
  "You wonder what's been happening on\n"\
  "Earth while you were battling evil\n"\
  "unleashed. It's good that no Hell-\n"\
  "spawn could have come through that\n"\
  "door with you ..."


#define E4TEXT \
  "the spider mastermind must have sent forth\n"\
  "its legions of hellspawn before your\n"\
  "final confrontation with that terrible\n"\
  "beast from hell.  but you stepped forward\n"\
  "and brought forth eternal damnation and\n"\
  "suffering upon the horde as a true hero\n"\
  "would in the face of something so evil.\n"\
  "\n"\
  "besides, someone was gonna pay for what\n"\
  "happened to daisy, your pet rabbit.\n"\
  "\n"\
  "but now, you see spread before you more\n"\
  "potential pain and gibbitude as a nation\n"\
  "of demons run amok among our cities.\n"\
  "\n"\
  "next stop, hell on earth!"


/* after level 6, put this: */

#define C1TEXT \
  "YOU HAVE ENTERED DEEPLY INTO THE INFESTED\n" \
  "STARPORT. BUT SOMETHING IS WRONG. THE\n" \
  "MONSTERS HAVE BROUGHT THEIR OWN REALITY\n" \
  "WITH THEM, AND THE STARPORT'S TECHNOLOGY\n" \
  "IS BEING SUBVERTED BY THEIR PRESENCE.\n" \
  "\n"\
  "AHEAD, YOU SEE AN OUTPOST OF HELL, A\n" \
  "FORTIFIED ZONE. IF YOU CAN GET PAST IT,\n" \
  "YOU CAN PENETRATE INTO THE HAUNTED HEART\n" \
  "OF THE STARBASE AND FIND THE CONTROLLING\n" \
  "SWITCH WHICH HOLDS EARTH'S POPULATION\n" \
  "HOSTAGE."

/* After level 11, put this: */

#define C2TEXT \
  "YOU HAVE WON! YOUR VICTORY HAS ENABLED\n" \
  "HUMANKIND TO EVACUATE EARTH AND ESCAPE\n"\
  "THE NIGHTMARE.  NOW YOU ARE THE ONLY\n"\
  "HUMAN LEFT ON THE FACE OF THE PLANET.\n"\
  "CANNIBAL MUTATIONS, CARNIVOROUS ALIENS,\n"\
  "AND EVIL SPIRITS ARE YOUR ONLY NEIGHBORS.\n"\
  "YOU SIT BACK AND WAIT FOR DEATH, CONTENT\n"\
  "THAT YOU HAVE SAVED YOUR SPECIES.\n"\
  "\n"\
  "BUT THEN, EARTH CONTROL BEAMS DOWN A\n"\
  "MESSAGE FROM SPACE: \"SENSORS HAVE LOCATED\n"\
  "THE SOURCE OF THE ALIEN INVASION. IF YOU\n"\
  "GO THERE, YOU MAY BE ABLE TO BLOCK THEIR\n"\
  "ENTRY.  THE ALIEN BASE IS IN THE HEART OF\n"\
  "YOUR OWN HOME CITY, NOT FAR FROM THE\n"\
  "STARPORT.\" SLOWLY AND PAINFULLY YOU GET\n"\
  "UP AND RETURN TO THE FRAY."


/* After level 20, put this: */

#define C3TEXT \
  "YOU ARE AT THE CORRUPT HEART OF THE CITY,\n"\
  "SURROUNDED BY THE CORPSES OF YOUR ENEMIES.\n"\
  "YOU SEE NO WAY TO DESTROY THE CREATURES'\n"\
  "ENTRYWAY ON THIS SIDE, SO YOU CLENCH YOUR\n"\
  "TEETH AND PLUNGE THROUGH IT.\n"\
  "\n"\
  "THERE MUST BE A WAY TO CLOSE IT ON THE\n"\
  "OTHER SIDE. WHAT DO YOU CARE IF YOU'VE\n"\
  "GOT TO GO THROUGH HELL TO GET TO IT?"


/* After level 29, put this: */

#define C4TEXT \
  "THE HORRENDOUS VISAGE OF THE BIGGEST\n"\
  "DEMON YOU'VE EVER SEEN CRUMBLES BEFORE\n"\
  "YOU, AFTER YOU PUMP YOUR ROCKETS INTO\n"\
  "HIS EXPOSED BRAIN. THE MONSTER SHRIVELS\n"\
  "UP AND DIES, ITS THRASHING LIMBS\n"\
  "DEVASTATING UNTOLD MILES OF HELL'S\n"\
  "SURFACE.\n"\
  "\n"\
  "YOU'VE DONE IT. THE INVASION IS OVER.\n"\
  "EARTH IS SAVED. HELL IS A WRECK. YOU\n"\
  "WONDER WHERE BAD FOLKS WILL GO WHEN THEY\n"\
  "DIE, NOW. WIPING THE SWEAT FROM YOUR\n"\
  "FOREHEAD YOU BEGIN THE LONG TREK BACK\n"\
  "HOME. REBUILDING EARTH OUGHT TO BE A\n"\
  "LOT MORE FUN THAN RUINING IT WAS.\n"

/* Before level 31, put this: */

#define C5TEXT \
  "CONGRATULATIONS, YOU'VE FOUND THE SECRET\n"\
  "LEVEL! LOOKS LIKE IT'S BEEN BUILT BY\n"\
  "HUMANS, RATHER THAN DEMONS. YOU WONDER\n"\
  "WHO THE INMATES OF THIS CORNER OF HELL\n"\
  "WILL BE."


/* Before level 32, put this: */

#define C6TEXT \
  "CONGRATULATIONS, YOU'VE FOUND THE\n"\
  "SUPER SECRET LEVEL!  YOU'D BETTER\n"\
  "BLAZE THROUGH THIS ONE!\n"

/*** Plutonia ***/
/* after map 06 */

#define P1TEXT  \
  "You gloat over the steaming carcass of the\n"\
  "Guardian.  With its death, you've wrested\n"\
  "the Accelerator from the stinking claws\n"\
  "of Hell.  You relax and glance around the\n"\
  "room.  Damn!  There was supposed to be at\n"\
  "least one working prototype, but you can't\n"\
  "see it. The demons must have taken it.\n"\
  "\n"\
  "You must find the prototype, or all your\n"\
  "struggles will have been wasted. Keep\n"\
  "moving, keep fighting, keep killing.\n"\
  "Oh yes, keep living, too."


/* after map 11 */

#define P2TEXT \
  "Even the deadly Arch-Vile labyrinth could\n"\
  "not stop you, and you've gotten to the\n"\
  "prototype Accelerator which is soon\n"\
  "efficiently and permanently deactivated.\n"\
  "\n"\
  "You're good at that kind of thing."


/* after map 20 */

#define P3TEXT \
  "You've bashed and battered your way into\n"\
  "the heart of the devil-hive.  Time for a\n"\
  "Search-and-Destroy mission, aimed at the\n"\
  "Gatekeeper, whose foul offspring is\n"\
  "cascading to Earth.  Yeah, he's bad. But\n"\
  "you know who's worse!\n"\
  "\n"\
  "Grinning evilly, you check your gear, and\n"\
  "get ready to give the bastard a little Hell\n"\
  "of your own making!"

/* after map 30 */

#define P4TEXT \
  "The Gatekeeper's evil face is splattered\n"\
  "all over the place.  As its tattered corpse\n"\
  "collapses, an inverted Gate forms and\n"\
  "sucks down the shards of the last\n"\
  "prototype Accelerator, not to mention the\n"\
  "few remaining demons.  You're done. Hell\n"\
  "has gone back to pounding bad dead folks \n"\
  "instead of good live ones.  Remember to\n"\
  "tell your grandkids to put a rocket\n"\
  "launcher in your coffin. If you go to Hell\n"\
  "when you die, you'll need it for some\n"\
  "final cleaning-up ..."

/* before map 31 */

#define P5TEXT \
  "You've found the second-hardest level we\n"\
  "got. Hope you have a saved game a level or\n"\
  "two previous.  If not, be prepared to die\n"\
  "aplenty. For master marines only."

/* before map 32 */

#define P6TEXT \
  "Betcha wondered just what WAS the hardest\n"\
  "level we had ready for ya?  Now you know.\n"\
  "No one gets out alive."

/*** TNT: Evilution ***/

#define T1TEXT \
  "You've fought your way out of the infested\n"\
  "experimental labs.   It seems that UAC has\n"\
  "once again gulped it down.  With their\n"\
  "high turnover, it must be hard for poor\n"\
  "old UAC to buy corporate health insurance\n"\
  "nowadays..\n"\
  "\n"\
  "Ahead lies the military complex, now\n"\
  "swarming with diseased horrors hot to get\n"\
  "their teeth into you. With luck, the\n"\
  "complex still has some warlike ordnance\n"\
  "laying around."


#define T2TEXT \
  "You hear the grinding of heavy machinery\n"\
  "ahead.  You sure hope they're not stamping\n"\
  "out new hellspawn, but you're ready to\n"\
  "ream out a whole herd if you have to.\n"\
  "They might be planning a blood feast, but\n"\
  "you feel about as mean as two thousand\n"\
  "maniacs packed into one mad killer.\n"\
  "\n"\
  "You don't plan to go down easy."


#define T3TEXT \
  "The vista opening ahead looks real damn\n"\
  "familiar. Smells familiar, too -- like\n"\
  "fried excrement. You didn't like this\n"\
  "place before, and you sure as hell ain't\n"\
  "planning to like it now. The more you\n"\
  "brood on it, the madder you get.\n"\
  "Hefting your gun, an evil grin trickles\n"\
  "onto your face. Time to take some names."

#define T4TEXT \
  "Suddenly, all is silent, from one horizon\n"\
  "to the other. The agonizing echo of Hell\n"\
  "fades away, the nightmare sky turns to\n"\
  "blue, the heaps of monster corpses start \n"\
  "to evaporate along with the evil stench \n"\
  "that filled the air. Jeeze, maybe you've\n"\
  "done it. Have you really won?\n"\
  "\n"\
  "Something rumbles in the distance.\n"\
  "A blue light begins to glow inside the\n"\
  "ruined skull of the demon-spitter."


#define T5TEXT \
  "What now? Looks totally different. Kind\n"\
  "of like King Tut's condo. Well,\n"\
  "whatever's here can't be any worse\n"\
  "than usual. Can it?  Or maybe it's best\n"\
  "to let sleeping gods lie.."


#define T6TEXT \
  "Time for a vacation. You've burst the\n"\
  "bowels of hell and by golly you're ready\n"\
  "for a break. You mutter to yourself,\n"\
  "Maybe someone else can kick Hell's ass\n"\
  "next time around. Ahead lies a quiet town,\n"\
  "with peaceful flowing water, quaint\n"\
  "buildings, and presumably no Hellspawn.\n"\
  "\n"\
  "As you step off the transport, you hear\n"\
  "the stomp of a cyberdemon's iron shoe."

/*
 * Character cast strings F_FINALE.C
 */
#define CC_ZOMBIE "ZOMBIEMAN"
#define CC_SHOTGUN  "SHOTGUN GUY"
#define CC_HEAVY  "HEAVY WEAPON DUDE"
#define CC_IMP  "IMP"
#define CC_DEMON  "DEMON"
#define CC_LOST "LOST SOUL"
#define CC_CACO "CACODEMON"
#define CC_HELL "HELL KNIGHT"
#define CC_BARON  "BARON OF HELL"
#define CC_ARACH  "ARACHNOTRON"
#define CC_PAIN "PAIN ELEMENTAL"
#define CC_REVEN  "REVENANT"
#define CC_MANCU  "MANCUBUS"
#define CC_ARCH "ARCH-VILE"
#define CC_SPIDER "THE SPIDER MASTERMIND"
#define CC_CYBER  "THE CYBERDEMON"
#define CC_HERO "OUR HERO"

/*Strife Actor Class Strings	-kaiser*/
#define	MC_WEAPONSMITH	"Weapon Smith"
#define MC_BARKEEP	"Bar Keep"
#define MC_ARMORER	"Armorer"
#define MC_MEDIC	"medic"
#define MC_BEGGAR	"beggar"
#define MC_REBEL	"rebel"
#define MC_MACIL	"macil"
#define MC_ACOLYTE	"acolyte"
#define MC_TEMPLAR	"templar"
#define MC_ORACLE	"oracle"
#define MC_PRIEST	"priest"
#define MC_MEDPATCH	"medpatch"
#define MC_MEDICALKIT	"medical kit"
#define MC_SURGERYKIT	"surgery kit"
#define MC_DEGNINORE	"degnin ore"
#define MC_METALARMOR	"metal armor"
#define MC_LEATHERARMOR	"leather armor"
#define MC_BASEKEY	"base key"
#define MC_GOVSKEY	"govs key"	//not found in editors
#define MC_PASSCARD	"passcard"
#define MC_IDBADGE	"id badge"
#define MC_PRISONKEY	"prison key"	//
#define MC_SEVEREDHAND	"severed hand"
#define MC_POWER1KEY	"power1 key"	//
#define MC_POWER2KEY	"power2 key"	//
#define MC_POWER3KEY	"power3 key"	//
#define MC_GOLDKEY	"gold key"
#define MC_IDCARD	"id card"
#define MC_SILVERKEY	"silver key"
#define MC_ORACLEKEY	"oracle key"
#define MC_MILITARYID	"military id"	//
#define MC_ORDERKEY	"order key"
#define MC_WAREHOUSEKEY	"warehouse key"
#define MC_BRASSKEY	"brass key"
#define MC_REDCRYSTALKEY	"red crystal key"
#define MC_BLUECRYSTALKEY	"blue crystal key"
#define MC_CHAPELKEY	"chapel key"
#define MC_CATACOMBKEY	"catacomb key"	//
#define MC_SECURITYKEY	"security key"	//
#define MC_COREKEY	"core key"
#define MC_MAULERKEY	"mauler key"
#define MC_FACTORYKEY	"factory key"
#define MC_MINEKEY	"mine key"
#define MC_NEWKEY5	"newkey 5"	//
#define MC_SHADOWARMOR	"shadow armor"
#define MC_ENVIRONMENTALSUIT	"environmental suit"
#define MC_GUARDUNIFORM	"guard uniform"
#define MC_OFFICERSUNIFORM	"officer's uniform"
#define MC_MAP	"map"
#define MC_SCANNER	"scanner"
#define MC_TELEPORTERBEACON	"teleporter beacon"
#define MC_TARGETER	"targeter"
#define MC_COIN	"coin"
#define MC_10GOLD	"10gold"
#define MC_25GOLD	"25gold"
#define MC_50GOLD	"50gold"
#define MC_300GOLD	"300gold"
#define MC_RING	"ring"
#define MC_OFFERINGCHALICE	"offeringchalice"
#define MC_EAR	"ear"
#define MC_COMMUNITACTOR	"communitcator"
#define MC_HEGRENADEROUNDS	"he-grenade rounds"
#define MC_PHOSPHORUSGRENADEROUNDS	"phoshorus-grenade rounds"
#define MC_CLIPOFBULLETS	"clip of bullets"
#define MC_AMMO	"box of bullets"
#define MC_MINIMISSILES	"mini missiles"
#define MC_CRATEOFMISSILES	"crate of missiles"
#define MC_ENERGYPOD	"energy pod"
#define MC_ENERGYPACK	"engery pack"
#define MC_POISONBOLTS	"poison bolts"
#define MC_ELECTRICBOLTS	"electric bolts"
#define MC_AMMOSATCHEL	"ammo satchel"
#define MC_ASSAULTGUN	"assault gun"
#define MC_FLAMETHROWER	"flame thrower"
#define MC_FLAMETHROWERPARTS	"flame thrower parts"
#define MC_MINIMISSILELAUNCHER	"mini missile launcher"
#define MC_MAULER	"mauler"
#define MC_CROSSBOW	"crossbow"
#define MC_GRENADELAUNCHER	"grenade launcher"
#define MC_SIGIL	"sigil"
#define MC_RATBUDDY	"rat buddy"
#define MC_BROKENPOWERCOUPLING	"broken power coupling"
#define MC_HEALTH	"health"
#define MC_TAMMO	"ammo"
#define MC_INFO	"info"
//#define MC_ALARM	"alarm"
#define MC_ALARM	"You fool! You've set off the alarm!"	//this is better..  -kaiser
#define MC_ALARMDEAD	"You're dead!  You set off the alarm!"	//1-13-06	-kaiser
#define MC_PRISONPASS	"prison pass"
#define MC_REPORT	"report"
#define MC_TOUGHNESS	"toughness"
#define MC_ACCURACY	"accuracy"
#define MC_ORACLEPASS	"oracle pass"
#define MC_QUEST4	"quest4"
#define MC_QUEST5	"quest5"
#define MC_QUEST6	"quest6"
#define MC_BLOWNEDUPCRYSTAL	"You've Blown Up the Crystal!"
#define MC_BLOWNEDUPGATES	"You've Blown Up the Gates!"
#define MC_DEADBISHOP	"You've killed the Bishop!"
#define MC_DEADORACLE	"You've killed the Oracle!"
#define MC_DEADMACIL	"You've killed Macil!"
#define MC_DEADLOREMASTER	"You've killed the LoreMaster!"
#define MC_BLOWNEDUPCOMPUTER	"You've blowned Up the Computer!"

//Linedef stuff
#define NOWORK	"that doesn't seem to work"
#define SHAREWARE	"this area is only available in the retail version of strife"
#define LOCK_ID	"you need an id card"
#define LOCK_GOLD	"you need a gold key"
#define LOCK_BADGE	"you need an id badge"
#define LOCK_SILVER	"you need a silver key"
#define LOCK_PASS	"you need a pass card"
#define LOCK_BRASS	"you need a brass key"
#define LOCK_HAND	"hand print not on file"
#define LOCK_PRISON	"you don't have the key to the prison"
#define LOCK_DEFAULT	"you don't have the key"
#define LOCK_ID2	"you need an id card to open this door"
#define LOCK_PASS2	"you need a pass card key to open this door"
#define LOCK_BADGE2	"you need an id badge to open this door"
#define LOCK_CHALICE	"you need the chalice!"
#define LOCK_ORACLE	"you need the oracle pass!"
#define FREED	"you've freed the prisoners!"
#define CONVERTER	"you've destoryed the converter!"

/*Dialog Stuff - Used for SCRIPT00 - kaiser*/
#define PEASNPC_1	"PLEASE DON'T HURT ME."
#define PEASNPC_2	"IF YOU'RE LOOKING TO HURT ME, I'M NOT REALLY WORTH THE EFFORT."
#define PEASNPC_3	"I DON'T KNOW ANYTHING."
#define PEASNPC_4	"GO AWAY OR I'LL CALL THE GUARDS!"
#define PEASNPC_5	"I WISH SOMETIMES THAT ALL THESE REBELS WOULD JUST LEARN THEIR PLACE AND STOP THIS NONSENSE."
#define PEASNPC_6	"JUST LEAVE ME ALONE, OK?"
#define PEASNPC_7	"I'M NOT SURE, BUT SOMETIMES I THINK THAT I KNOW SOME OF THE ACOLYTES."
#define PEASNPC_8	"THE ORDER'S GOT EVERYTHING AROUND HERE PRETTY WELL LOCKED UP TIGHT."
#define PEASNPC_9	"THERE'S NO WAY THAT THIS IS JUST A SECURITY FORCE."
#define PEASNPC_10	"I'VE HEARD THAT THE ORDER IS REALLY NERVOUS ABOUT THE FRONT'S ACTIONS AROUND HERE."

#define REBLNPC_1	"THERE'S NO WAY THE ORDER WILL STAND AGAINST US."
#define REBLNPC_2	"WE'RE ALMOST READY TO STRIKE. MACIL'S PLANS ARE FALLING IN PLACE."
#define REBLNPC_3	"WE'RE ALL BEHIND YOU, DON'T WORRY."
#define REBLNPC_4	"DON'T GET TOO CLOSE TO ANY OF THOSE BIG ROBOTS. THEY'LL MELT YOU DOWN FOR SCRAP!"
#define REBLNPC_5	"THE DAY OF OUR GLORY WILL SOON COME, AND THOSE WHO OPPOSE US WILL BE CRUSHED!"
#define REBLNPC_6	"DON'T GET TOO COMFORTABLE. WE'VE STILL GOT OUR WORK CUT OUT FOR US."
#define REBLNPC_7	"MACIL SAYS THAT YOU'RE THE NEW HOPE. BEAR THAT IN MIND."
#define REBLNPC_8	"ONCE WE'VE TAKEN THESE CHARLATANS DOWN, WE'LL BE ABLE TO REBUILD THIS WORLD AS IT SHOULD BE."
#define REBLNPC_9	"REMEMBER THAT YOU AREN'T FIGHTING JUST FOR YOURSELF, BUT FOR EVERYONE HERE AND OUTSIDE."
#define REBLNPC_10	"AS LONG AS ONE OF US STILL STANDS, WE WILL WIN."

#define ACLENPC_1	"MOVE ALONG,  PEASANT."
#define ACLENPC_2	"FOLLOW THE TRUE FAITH, ONLY THEN WILL YOU BEGIN TO UNDERSTAND."
#define ACLENPC_3	"ONLY THROUGH DEATH CAN ONE BE TRULY REBORN."
#define ACLENPC_4	"I'M NOT INTERESTED IN YOUR USELESS DRIVEL."
#define ACLENPC_5	"IF I HAD WANTED TO TALK TO YOU I WOULD HAVE TOLD YOU SO."
#define ACLENPC_6	"GO AND ANNOY SOMEONE ELSE!"
#define ACLENPC_7	"KEEP MOVING!"
#define ACLENPC_8	"IF THE ALARM GOES OFF, JUST STAY OUT OF OUR WAY!"
#define ACLENPC_9	"THE ORDER WILL CLEANSE THE WORLD AND USHER IT INTO THE NEW ERA."
#define ACLENPC_10	"PROBLEM?  NO, I THOUGHT NOT."

#define BEGGNPC_1	"ALMS FOR THE POOR?"
#define BEGGNPC_2	"WHAT ARE YOU LOOKING AT, SURFACER?"
#define BEGGNPC_3	"YOU WOULDN'T HAVE ANY EXTRA FOOD, WOULD YOU?"
#define BEGGNPC_4	"YOU  SURFACE PEOPLE WILL NEVER UNDERSTAND US."
#define BEGGNPC_5	"HA, THE GUARDS CAN'T FIND US.  THOSE IDIOTS DON'T EVEN KNOW WE EXIST. "
#define BEGGNPC_6	"ONE DAY EVERYONE BUT THOSE WHO SERVE THE ORDER WILL BE FORCED TO   JOIN US."
#define BEGGNPC_7	"STARE NOW,  BUT YOU KNOW THAT THIS WILL BE YOUR OWN FACE ONE DAY."
#define BEGGNPC_8	"THERE'S NOTHING THING MORE ANNOYING THAN A SURFACER WITH AN ATTITUDE!"
#define BEGGNPC_9	"THE ORDER WILL MAKE SHORT WORK OF YOUR PATHETIC FRONT."
#define BEGGNPC_10	"WATCH YOURSELF SURFACER. WE KNOW OUR ENEMIES!"

#define TMPRNPC_1	"WE ARE THE HANDS OF FATE. TO EARN OUR WRATH IS TO FIND OBLIVION!"
#define TMPRNPC_2	"THE ORDER WILL CLEANSE THE WORLD OF THE WEAK AND CORRUPT!"
#define TMPRNPC_3	"OBEY THE WILL OF THE MASTERS!"
#define TMPRNPC_4	"LONG LIFE TO THE BROTHERS OF THE ORDER!"
#define TMPRNPC_5	"FREE WILL IS AN ILLUSION THAT BINDS THE WEAK MINDED."
#define TMPRNPC_6	"POWER IS THE PATH TO GLORY. TO FOLLOW THE ORDER IS TO WALK THAT PATH!"
#define TMPRNPC_7	"TAKE YOUR PLACE AMONG THE RIGHTEOUS, JOIN US!"
#define TMPRNPC_8	"THE ORDER PROTECTS ITS OWN."
#define TMPRNPC_9	"ACOLYTES?  THEY HAVE YET TO SEE THE FULL GLORY OF THE ORDER."
#define TMPRNPC_10	"IF THERE IS ANY HONOR INSIDE THAT PATHETIC SHELL OF A BODY, YOU'LL ENTER INTO THE ARMS OF THE ORDER."


#endif
