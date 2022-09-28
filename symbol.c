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
 *  symbol.c - symbol set stuff
 *
 * 
 */

#include "cjet.h"
#include "symbol.h"


/* 
 * print <accent>ed <letter>
 * Calculate amount to move accent right and over the letter.
 * The movement amounts depend on current character size, typeface
 * and type style. Fudge factors found by trial-and-error and some 
 * simplistic calculations...FIXME
 * 
 */
  
void letter_accent( int letter , int accent )
{
  long x,y;
  long c = char_size;

  x = 0;     /* offset to the right */
  y = 0;     /* offset upwards */

  /* 
   * save current graphic set and position and 
   * select PSR as secondary graphic set      
   */
  PutString("\033[0;5;6%%y\033)\042! 1");   
  SetModes(SIZEUNIT,DECIPTS);
  switch(letter) {
   case 'S' : {
     y = ( c * 175) / 1000; 
     if( tface_p == C_DUTCH )
       x = y / 2;
     else
       x = y;
     break;
   }
   case 'Y' : {
     y = ( c * 175) / 1000; 
     if( tface_p == C_DUTCH )
       x =  (c * 200) / 1000;
     else
       x = y;
     break;
   }
   case 's' : {
     if( tface_p == C_SWISS )
       x = (c * 90) / 1000;
   }
   default: break;
   }
  /* 
   * move right, move up, print accent, pop previous state, print letter
   */
  PutString("\033[%lda\033[%ldk%c\033[0;5;6%%z%c", x, y, accent, letter);
}



/*
 * Finally, really print a character 
 * Characters have been translated, symbol sets selected etc.
 */

void ReallyPrint( int what )
{
  
  /* 
   * force printing of control characters 0x00 - 0x20: any control 
   * characters which should actually function as control codes are 
   * trapped in the main parsing loop in main() 
   */
  if(what < 0x20) 
    PutString("\033[1.v%c", what); 
  else { 
    switch(what) {
    case 0x7f :  /* DEL is normally ignored by CaPSL */
    case 0x85 :  /* 8-bit control characters. According to the manual, */
    case 0x88 :  /* these control characters are printed as characters */
    case 0x8a :  /* if any IBM secondary set or PSR is selected. To    */
    case 0x8b :  /* ensure correct printing, esp. with download fonts, */
    case 0x8c :  /* force printing of characters anyway */
    case 0x8d :
    case 0x90 :
    case 0x9b :
    case 0x9c :
    case 0xff : PutString("\033[1.v%c", what); break;  
    default   : put_next_char(what); break;
    }
  }
}
    

/* 
 * print a character from another graphic set:
 * push current environment, select the new graphic set,
 * print the character and pop the previous state.
 * flag indicates primary or secondary set
 */

void print_alt_char( int c, BOOL flag, char *set )
{
  char which;

  if(flag == PRIMARY)
    which = '(';
  else
    which = ')';
  PutString("\033[0%%y\033%c%s", which, set);  /* push & select */
  ReallyPrint(c);
  PutString("\033[0%%z");                      /* pop previous state */
}


/* 
 * Print Trademark symbol from symbol typeface
 * Check current typeface to print correct TM symbol (Dutch or Swiss typeface)
 */

void PrintTM(void)
{
  if (tface_s == C_DUTCH)
    print_alt_char( 212, SECONDARY, C_SYMR);
  else
    print_alt_char( 228, SECONDARY, C_SYMR);
}
		


/* 
 * hack to print c/o character 
 * select smaller font size and print c o slash
 * FIXME
 */

void PrintCO(void) 
{
  int newsize, x, y;

  SetModes(SIZEUNIT,DECIPTS);  
  PutString("\033[0;2;5;6%%y");       /* save current environment */
  newsize = (char_size * 3) / 4;      /* new char size in decipoints */
  y = newsize / 2;
  x = newsize / 3;
  PutString("\033[%d C",newsize);
  /* move left, move up, print c, move down, print o */
  PutString("\033[%dj\033[%dkc\033[%deo", x, y, y );
  PutString("\033[0;2;5;6%%z");      /* pop previous environment */
  print_alt_char(164, SECONDARY, C_PSR);
}



/*
 * various hacks to print characters not normally existent
 * in CaPSL symbol sets; some can be constructed by printing two
 * characters, some can only be replaced by more or less adequate 
 * characters
 */

void PrintSpecial( int what )
{ 
  switch (what) {
  case HACEK_SMALL_S     : letter_accent('s', PSR_HACEK); break;
  case HACEK_CAP_S       : letter_accent('S', PSR_HACEK); break;
  case DIAERESIS_CAP_Y   : letter_accent('Y', PSR_DIAERESIS); break;
  case SINGLE_QUOTE_OPEN : print_alt_char( 169, SECONDARY, C_PSR); break;
  case SINGLE_QUOTE_CLOSE: print_alt_char( 169, SECONDARY, C_PSR); break;
  case TRADEMARK         : PrintTM(); break;
  case CEE_OH            : PrintCO(); break;
  case IGREK_SMALL       : print_alt_char(0x40, PRIMARY, "\042#2"); break;
  case IGREK_CAP         : { SetModes(SIZEUNIT,DECIPTS);
			     PutString("I\033[%djJ", (print_HMI / 300 ));
			     break;
			   }
/* 
 * Note: this assumes IBML is selected as primary set
 */
  case FILLCIRCLE :  PutString("\033[1.v%c", 7); break;
  case OPENCIRCLE :  PutString("\033[1.v%c", 9); break;  
  }
}



/*
 * PrintThrough: print a character, check if it's from another
 * symbol set or has the TSPECIAL flag 
 */

void PrintThrough(int what)
{
  int c, flags;

  c     = what & 0x00ff;
  flags = what & 0xffffff00;

  switch( flags) {
  case TSPECIAL : PrintSpecial(what); break;
  case TPSR     : print_alt_char( c, SECONDARY, C_PSR);   break;
  case TIBMR1   : print_alt_char( c, SECONDARY, C_IBMR1); break;
  case TIBMR2   : print_alt_char( c, SECONDARY, C_IBMR2); break;
  case TSYML    : print_alt_char( c, PRIMARY,   C_SYML);  break;
  case TSYMR    : print_alt_char( c, SECONDARY, C_SYMR);  break;
  default       : ReallyPrint(c); break;
  }
}



/*
 * HP Legal is a 7-bit set. 
 * When characters 128-255 are printed then this set
 * is a secondary set; the characters are mapped to 0..127 to
 * check for the special characters. 
 */

void translate_legal( int c )
{
  int what;

  if (c > 127)
    what = c - 128;
  else
    what = c;

  switch(what) {
  case  60 : print_alt_char( 242, SECONDARY, C_IBM850); break; /* dbl underl */
  case  62 : print_alt_char( 189, SECONDARY, C_IBM850); break; /* cents      */
  case  92 : print_alt_char( 169, SECONDARY, C_IBM850); break; /* r circle   */
  case  94 : print_alt_char( 184, SECONDARY, C_IBM850); break; /* copyright  */
  case  96 : print_alt_char( 248, SECONDARY, C_IBM850); break; /* circle     */
  case 123 : print_alt_char( 245, SECONDARY, C_IBM850); break; /* paragraph  */
  case 124 : print_alt_char( 244, SECONDARY, C_IBM850); break; /* thingy     */
  case 125 : print_alt_char( 178, SECONDARY, C_PSR);    break; /* dagger     */
  case 126 : PrintTM();                                 break; /* trademark  */
  default : ReallyPrint( c ); break;
  } 
}


/*
 * Translate PCL symbol set character positions to CaPSL positions.
 *
 * PCL characters 0..127 generally match CaPSL characters 0..127 (ASCII
 * and national variants). Current Exceptions are:
 *   HP Legal: 
 *
 * PCL characters 128..255 are matched to CaPSL characters
 *   by lookup tables
 *   or directly, e.g. for IBM and PS symbol sets
 *   
 * Things to look out for:
 *   shift state
 *   don't translate for download fonts
 *   
 * Changes:
 *   let ISOLATIN1 and WIN30 table equal WIN31 table
 */

void translate( int c)
{
  int set;

  if(c < 128) {                          /* print from primary set if the  */
    if( shift_state == SI ) {            /* character codes are 0..127 and */
      set = set_p;                       /* shift mode is SI (Shift In)    */
      switch(set) {
      case HP_LEGLSET : translate_legal(c);  break;
      default         : ReallyPrint(c);      break;
      }
      return; 
    }                                    /* shift mode is Shift Out: */
    c += 128;                            /* map 0..127 to 128..255   */
  }                                      /* fall through to:         */  
  set = set_s;                           /* print from secondary set */
  switch(set) { 
  case HP_ROMAN     : PrintThrough( roman_table  [c-128] ); break; 
  case HP_ISOLATIN1 :
  case HP_WIN30     : 
  case HP_WIN31     : PrintThrough( latin_table  [c-128] ); break;    
  case HP_DESKTOP   : PrintThrough( desktop_table[c-128] ); break;      
  case HP_MCTEXT    : PrintThrough( mctext_table [c-128] ); break;
  case HP_VINTL     : PrintThrough( v_intl_table [c-128] ); break;
  case HP_VMATH     : PrintThrough( vmath_table  [c-128] ); break;
  case HP_VUS       : PrintThrough( vus_table    [c-128] ); break;
  case HP_LEGLSET   : translate_legal(c); break;
  case DOWNLD       : 
  default           : ReallyPrint(c);                       break;
  }
}



void SetSymbolID( int id)
{
  UNIMPLEMENTED("Symbol set ID");
}

void LoadSymbol( int count )
{
  UNIMPLEMENTED("Download symbol set");
}

void SymbolControl( int op )
{
  UNIMPLEMENTED("Symbol set control");
}



