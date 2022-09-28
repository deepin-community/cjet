/* 
 * CJet:  LaserJet emulation for Canon CaPSL III+ laser printers
 *        Copyright (C) 1996 Michael Huijsmans
 *        Email: mgh@sbox.tu-graz.ac.at
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 *  capsl.h - CaPSL defines
 *            includes configuration file "config.h" 
 */

/* 
 * constant to convert PCL 4*dot pitch to CaPSL 100*pts pitch values 
 * canval = (300 / (pclval / 4)) * 100 => (300 * 4 * 100) /pclval
 */

#define DOT2PTS           120000 


/* 
 * CaPSL typefaces
 */

#define C_COURIER   3     /* Courier is Courier is Courier */
#define C_SWISS     4     /* Swiss 721  - Univers / Helvetica */
#define C_DUTCH     5     /* Dutch 801  - CG Times */
#define C_SYMBOL    137   /* Symbol */


/* 
 * some CaPSL graphic sets, the user manual doesn't list all of them (sigh)
 * found by trial-and-error:
 *          IBM Portugal
 *          IBM CP 860 Portugal
 *          IBM CP 863 Canadian French
 *          IBM CP 865 Norway
 */

#define C_IBML   "'$2"          /* 742  IBM L */
#define C_IBMR1  "' 1"          /* 701  IBM R1 - Codepage 437 */
#define C_IBMR2  "' 2"          /* 702  IBM R2 - Danish / Norwegian */
#define C_IBM850 "' 3"          /* 703  Codepage 850 Multilingual */
#define C_IBM865 "'!6"          /* 716  Codepage 865 */
#define C_IBM863 "'!4"          /* 714  Codepage 863 */
#define C_IBM860 "'!1"          /* 711  Codepage 860 - Portugal */
#define C_IBMP   "'!0"          /* 710  IBM Portugal */
#define C_PSR    "\042! 1"      /* 2101 PostScript right */
#define C_SYML   "\042!!0"      /* 2110 Symbol left */
#define C_SYMR   "\042!!1"      /* 2111 Symbol right */
#define C_TURKISH "\042!\0421"  /* 2121 Turkish */
#define C_PSL     "'!$2"        /* 7142 PostScript left */


/* 
 * CaPSL movement modes and submodes: line/column or size unit; 
 * units in size unit mode are either dots or decipoints         
 * dots are 1/300 in; decipts are 1/720 in.
 */

#define LINECOL   0
#define SIZEUNIT  1
#define DOTS      7
#define DECIPTS   2


/* 
 * CaPSL portrait page size ids, add 1 to portrait id for landscape id
 */

#define C_A4      14   
#define C_LETTER  30
#define C_LEGAL   32
#define C_EXEC    40


/*
 * Memorized Position stuff. PCL stores up to 20 positions in a stack,
 * CaPSL saves up to 50 in a random access mode. Define some indices.
 */ 

#define STACKBEGIN    10      /* Bottom of stack  */
#define STACKLIMIT    29      /* 20 Positions     */
#define PATTERNINDEX  30      /* saved position for patterns */
#define RASTERINDEX   31      /* save position for Raster Graphics */
#define MARGININDEX   32      /* save position for margin setting */
#define S_LEFTMARGIN  33      /* save initial (adjusted) left margin */
#define S_RIGHTMARGIN 34      /* save initial (adjusted) right margin */
#define S_BOTMARGIN   35      /* save current bottom margin */
#define S_TOPMARGIN   36      /* save initial (adjusted) top margin */
#define S_CURRENTLEFT 37      /* save current left margin */
#define S_CURRENTTOP  38      /* save current top margin */


















