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
 *  io.c - input/output
 *
 *  #define get_next_char()     fgetc(infile)
 *  #define unget_char(a)       ungetc(a,infile)
 */

#include "cjet.h"


/* 
 * output a string, either directly to the printer or to the 
 * macro recording buffer 
 */

void PutString(char *format, ... )
{
  va_list stuff;
  va_start(stuff, format);
  vfprintf(outfile, format, stuff);
  va_end(stuff);
}


void put_next_char( int c)
{
  if(recording == TRUE) {
    /* record in macro buffer */
  }
  else
    fputc (c,outfile);
}


/* 
 * read file/pipe routine with `helpful' error message 
 */

void pipe_read( BYTE *p, int size)
{
  if ( fread(p,1,size,infile) != size) {
    MorePanic(UN_RFILE, "input", 0);
    exit(1);           /* yes, a bit abrupt */
  }
}



/* 
 * file/pipe write routine with `helpful' error message
 */

void pipe_write( BYTE *p, int size)
{
  if(recording == TRUE) {
    /* macro recording */
  } else {
    if (fwrite(p, 1, size, outfile) != size) {
      MorePanic(UN_WFILE, "output", 0);
      exit(1);           /* yes, a bit abrupt */
    }
  }
}



void PutWord(int a)
/* the stupid way */
{
  WORD hi, lo;
  hi = (char) ((a>>8) & 0xff);
  lo = (char) (a & 0xff);
  put_next_char(hi);
  put_next_char(lo);
}


 



