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
 *  pjl.c  -  check for and ignore (for now) PJL sequences
 *
 */
 

#include "cjet.h"

/* 
 * later model HP printers add a "Printer Job Language"
 * PJL commands are of the form "@PJL bla bla bla [CR]LF"
 */


/*
 * A '@' was detected in the input stream. Check for the string "PJL":
 * if "PJL" was found, ignore everything up to the next LF
 * else print the pending '@' and any read characters.
 */

void CheckPJL(void)
{
  
  int p, j, l;

  p = get_next_char();

  if (p == 'P') {
    j = get_next_char(); 
    if (j == 'J') {
      l = get_next_char();
      if (l == 'L') {           /* yes. @PJL found. now ignore it */
	do {
	  p = get_next_char();
	} while (p != LF);
      } else {                  /* print "@PJ" */
	translate('@');
	translate('P');
	translate('J');
	unget_char(l);          /* put back last character */
      }
    } else {                    /* print "@P" */
	translate('@');
	translate('P');
	unget_char(j);          /* put back last character */
      }
  }
  else {
    translate('@');
    unget_char(p);              /* put back last character */
  }
}
