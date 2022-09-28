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
 * macro.c - macro and overlay stuff (under construction)
 *
 *  EXTERN int  macro_id;            current macro id 
 *  EXTERN BOOL recording;           macro registration in progress 
 *  EXTERN long macro_free;          remaining space in macro buffer
 *  EXTERN bdesc mac_buffer;         macro defs here 
 *  EXTERN BYTE  *macroptr;          current mac_buffer ptr 
 *
 *  UNIMPLEMENTED("MacroControl");
 *  UNIMPLEMENTED("MacroID");
 *  UNIMPLEMENTED("MacroRef");
 */


#include "cjet.h"


/* 
 * macro control - ESC &f <op> X
 *
 */
 
void MacroControl( int op )
{
  switch(op) {
  case 0 : /* start macro recording */
  case 1 : /* stop macro recording */
  case 2 : /* execute macro */
  case 3 : /* call macro */
  case 4 : /* enable macro for automatic overlay */
  case 5 : /* disable automatic overlay */
  case 6 : /* delete all macros */
  case 7 : /* delete all temporary macros */
  case 8 : /* delete current macro */
  case 9 : /* make macro temporary */
  case 10 : /* make macro permanent */
  default: break;
  }
  UNIMPLEMENTED("MacroControl");
}


/* 
 * assign macro ID number - ESC &f <id> Y
 */

void MacroID( int id )
{
  macro_id = id;
}
