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
 *  config.h : setup printer defaults:
 *             - paper size
 *             - symbol set
 *             - typeface
 *             - top and bottom margins
 */

/*
 * Cassette paper size: Select one of these:
 * C_A4      for European A4 
 * C_LETTER  for US Letter size
 * C_LEGAL   for US Legal size
 * C_EXEC    for US Executive size 
 */

#define DEF_PAPER     C_A4


/* 
 * Default symbol set: (select one of the following)
 *  HP_ROMAN         
 *  HP_PC_8
 *  HP_ISOLATIN1
 *  HP_PC850
 */

#define DEF_SET       HP_ROMAN


/* 
 * Default Typeface: select one:
 * C_COURIER 
 */

#define DEF_TFACE    C_COURIER


/* 
 * default top and bottom margins in decipoints
 * default 0.5 inch margins => 0.5in * 720 dpts = 360
 */

#define DEF_TOP_MARGIN        360
#define DEF_BOT_MARGIN        360


/* end */






