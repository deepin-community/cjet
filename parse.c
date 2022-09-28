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
 *  parse.c  -  disassemble PCL sequences
 *
 */
 

#include "cjet.h"


/*
 * extended isalpha() to include the character '@'
 */

int isxalpha( int c)
{
  if (c == '@')
    return(TRUE);
  else
    return(isalpha(c));
}


/*
 * extended isupper() to include '@' 
 */

int isxupper( int c )
{
  if (c == '@')
    return(TRUE);
  else
    return (isupper(c));
}


/*
 * A PCL escape sequence has the form: 
 * ESC `command class character(s)' `value field' `command letter'
 * Example: `ESC &a 123.456 B'
 * 
 * Sequences of the same class, e.g. `&a' can be combined by printing the
 * `command letter' (aka `terminator') in lowercase; the last command should
 * be uppercase.
 * Example: `ESC &a 123 b 456 c 789 D'
 *
 * Parse() scans a PCL escape sequence, extracts its argument and command 
 * letter and returns a flag to indicate whether more commands of the 
 * same class will follow.
 *  
 * The `value field' (aka `parameter') may be a decimal fraction, e.g.
 * '16.666'. Parse() keeps track of the decimal point and then divides
 * the resulting integer value (ignoring the decimal point) by the
 * correct factor. In the above example, 16666 / 1000 = 16.666.   
 */

int Parse(param *p)
{
  long  current;
  BOOL  real;        /* flag: parameter value is a real value */
  long  len;
  int   c, d, more;

  p->rel  = NONE; 
  p->i    = 0;
  p->f    = 0.0;  
  current = 0;
  real    = FALSE;
  len     = 0;

  while (!isxalpha(c = get_next_char())) { 
    switch(c) {
    case '+' : p->rel = POSITIVE; break;
    case '-' : p->rel = NEGATIVE; break;
    case '0' :
    case '1' :
    case '2' : 
    case '3' :
    case '4' :
    case '5' :
    case '6' :
    case '7' :
    case '8' :
    case '9' : { 
                 current = current*10 + (c-48);
		 len *= 10;
		 break;
	       }
    case '.' : {              /* after decimal point, keep track of number of*/
                 len = 1;     /* digits we have to divide by later in 'len'  */
 		 real = TRUE;
		 break;
	       }
      default  : MorePanic(UN_PARAM, "in Parse()"); break; 
    }
    if(real == TRUE) { /* value is too large by factor 'len' */
      p->f = (double) current / (double) len;
      p->i = (int) p->f;
    }
    else {
      p->i = current;
      p->f = (double) current;
    }
  }

  /* 
   * Some programs create files with PCL sequences in which the
   * terminating character is LOWER case; check for this here:
   * If an ESC or EOF character follows a lowercase terminator, 
   * then that letter should have been uppercase - end of command.
   * Assume valid continuation for any other following stuff
   */

  more = FALSE;         
  p->letter = toupper(c);  
  if (islower(c)) {         /* either continuation or special case */
    d = get_next_char();    /* peek at next character */ 
    unget_char(d);          /* put it back before it's missed */
    if (d != ESC)           /* ESC means a new command */
      if (d != EOF)         /* EOF means just that */
	more = TRUE;        /* else to be continued... */
  }
  return(more);
}


/* 
 * Download symbol set - ESC (f sequence (PCL5) 
 * FIXME - no manual
 */

void Paren_F(int gset)
{
  int more;
  param p;
 
  do {
    more = Parse (&p);
    switch(p.letter) {
    case 'W' : LoadSymbol( p.i); break;
    default  : MorePanic( UN_CMD,"ESC (f %c or ESC )f %c", p.letter, p.letter);
               break;
    }
  }while(more);
}



/* 
 * 'ESC (s' or 'ESC )s' sequence ...
 */

void Paren_S(int gset)
{
  int more;
  param p;

  do {
    more = Parse(&p);
    switch(p.letter){
    case 'P' : SetSpacing(gset, p.i);  break;
    case 'H' : SetPitch(gset, p.f);    break;
    case 'V' : SetHeight(gset, p.f);   break;
    case 'S' : SetStyle(gset, p.i);    break;
    case 'B' : SetStroke(gset, &p);    break;
    case 'T' : SetTypeface(gset, p.i); break;
    case 'W' : { if (gset == PRIMARY)
                   DownLoadChar( p.i);
                 else
                   CreateFont( p.i );
                 break;
	       }                         
    default  : MorePanic(UN_CMD, "ESC (s %c or ESC )s %c", p.letter, p.letter);
               break;
    }
  }while (more);
}


/* 
 * 'ESC (' or 'ESC )' sequence ...
 */

void Paren( int direction )
{
   int c, more, setnum, gset;
   param p;

   if(direction == LEFT)
     gset = PRIMARY;
   else
     gset = SECONDARY;
   c = get_next_char();
   if(c == 's')           /* ESC (s or ESC )s... */
     Paren_S(gset);
   else if(c == 'f')      /* ESC (f or ESC )f... */
     Paren_F(gset);
   else {                 /* ESC ( or ESC )... */
     unget_char(c);
     do {  
       more = Parse(&p);
       switch(p.letter) {
       case '@' : SetDefaultFont(gset, p.i); break;
       case 'X' : SetDownLoadFont(gset,p.i); break; 
       default  : { /* select symbol set */
	            setnum = p.i*32 + (p.letter-64);
		    SetSymbolSet(gset, setnum);
		    break;
		  }
       }
     } while(more);
   }
 }


void Amp_a( void )
{
  int more;
  param p;

  do {  
    more = Parse(&p);
    switch(p.letter) {
    case 'L' : SetLeftMargin(p.i);               break;
    case 'M' : SetRightMargin(p.i);              break;
    case 'C' : MoveHoriz(LINECOL, 0 ,&p);        break;
    case 'R' : MoveVert(LINECOL, 0, &p);         break;
    case 'H' : MoveHoriz(SIZEUNIT, DECIPTS, &p); break;
    case 'V' : MoveVert(SIZEUNIT, DECIPTS, &p);  break;
    case 'P' : logical_rotate(p.i);              break;
    case 'W' : logical_page(p.i);                break;
    default  : MorePanic( UN_CMD, "ESC &a %c", p.letter); break;
    }
  } while(more);
}


void Amp_d( void )
{ 
  int more;
  param p;

  do {  
    more = Parse(&p);
    switch(p.letter) {
    case 'D' : SetUnderLineMode(p.i); break;
    case '@' : DisableUnderLine(); break;
    default : MorePanic(UN_CMD, "ESC &d %c", p.letter); break;
    }
  } while(more);
}


void 
Amp_f( void )
{
  int more;
  param p;

  do {  
    more = Parse(&p);
    switch(p.letter) {
    case 'S' : PushPopPos(p.i);   break;
    case 'X' : MacroControl(p.i); break;
    case 'Y' : MacroID(p.i);      break;
    default  : MorePanic(UN_CMD, "ESC &f %c", p.letter); break;
    }
  } while(more);
}


void Amp_k( void )
{
  int more;
  param p;

  do {  
    more = Parse(&p);
    switch(p.letter) {
    case 'H' : SetHMI(p.f);          break;
    case 'S' : SetPrintPitch(p.i);   break;
    case 'G' : LineTermination(p.i); break;
    default  : MorePanic(UN_CMD, "ESC &k %c", p.letter); break; 
    }
  } while(more);
}


void Amp_p( void )
{
  int more;
  param p;

  do {  
    more = Parse(&p);
    switch(p.letter) {
    case 'X' : TransparentPrint(p.i); break;
    default  : MorePanic(UN_CMD,"ESC &p %c", p.letter); break; 
    }
  } while(more);
}


void Amp_l( void )
{  
  int more;
  param p;

  do {  
    more = Parse(&p);
    switch(p.letter) {
    case 'P' : SetPageLength(p.i);   break;
    case 'E' : SetTopMargin(p.i);    break;
    case 'F' : SetTextLength(p.i);   break;
    case 'D' : SetLinesPerIn(p.i);   break;
    case 'C' : SetVMI(p.f);          break;
    case 'O' : SetPageOrient(p.i);   break;
    case 'L' : PerfSkipMode(p.i);    break;
    case 'X' : NumCopies(p.i);       break;
    case 'H' : PaperControl( p.i);   break;
    case 'A' : SetPageSize(p.i);     break;   
    case 'G' : paper_tray(p.i);      break;  
    case 'U' : log_position(&p);     break;
    case 'Z' : log_position(&p);     break;
    default  : MorePanic(UN_CMD,"ESC &l %c", p.letter); break;
    }
  } while(more);
}


void Amp_s( void )
{  
  int more;
  param p;

  do {  
    more = Parse(&p);
    switch(p.letter) {
    case 'C' : SetWrapMode(p.i); break;
    default  : MorePanic(UN_CMD,"ESC &s %c", p.letter); break;
    }
  } while(more);
}


void Amp_u( void )
{  
  int more;
  param p;

  do {  
    more = Parse(&p);
    switch(p.letter) {
    case 'D' : SetUnitsPerInch(p.i); break;
      default  : MorePanic(UN_CMD,"ESC &u %c", p.letter); break;
    }
  } while(more);
}


void Amp_r( void )
{  
  int more;
  param p;

  do {  
    more = Parse(&p);
    switch(p.letter) {
    case 'F' : ClearPages(p.i); break;
      default  : MorePanic(UN_CMD,"ESC &r %c", p.letter); break;
    }
  } while(more);
}


void Ampersand( void )
/* 'ESC &' sequence ... */
{
   char c;
   /* get command class letter ... */
   c = get_next_char();
   switch(c){
   case 'a' : Amp_a(); break;
   case 'd' : Amp_d(); break;
   case 'f' : Amp_f(); break;
   case 'k' : Amp_k(); break;
   case 'l' : Amp_l(); break;
   case 'p' : Amp_p(); break;
   case 'r' : Amp_r(); break;
   case 's' : Amp_s(); break;
   case 'u' : Amp_u(); break;
   default  : MorePanic( UN_CLASS, "ESC &%c", c); break;
   }
}


void Ast_t(void)
{
  int more;
  param p;
  
  do {
    more = Parse(&p);
    switch(p.letter) {
      case 'R' : RasterRez(p.i);   break;
      default  : MorePanic(UN_CMD,"ESC *t %c", p.letter); break;
    }
  }while(more);
}


void Ast_b(void)
{
  int more;
  param p;
  
  do {
    more = Parse(&p);
    switch(p.letter) {
    case 'W' : TransferData(p.i); break;    
    case 'M' : Compression(p.i);  break; 
    case 'Y' : y_offset(p.i);     break;
    default  : MorePanic(UN_CMD,"ESC *b %c", p.letter); break;
    }
  }while(more);
}


void Ast_r(void)
{
  int more;
  param p;
  
  do {
    more = Parse(&p);
    switch(p.letter) {
    case 'A' : StartRaster(p.i);    break;
    case 'B' : EndRaster();         break;
    case 'C' : EndRaster();         break;
    case 'F' : rotate_raster(p.i);  break;
    case 'T' : raster_height(p.i);  break;
    case 'S' : raster_width(p.i);   break;
    default  : MorePanic(UN_CMD,"ESC *r %c", p.letter); break;
    }
  }while(more);
}


void Ast_p(void)
{
  int more;
  param p;

  do {
    more = Parse(&p);
    switch(p.letter) {
    case 'X' : MoveHoriz(SIZEUNIT, DOTS, &p);   break;
    case 'Y' : MoveVert(SIZEUNIT, DOTS, &p);    break;
    case 'R' : MacroRef(p.i); break;
    default  : MorePanic(UN_CMD, "ESC *p %c", p.letter); break;
    }
  }while(more);
}


void Ast_v(void)
{
  int more;
  param p;

  do {
    more = Parse(&p);
    switch(p.letter) {
    case 'T' : TPattern(p.i);    break;
    case 'N' : TSourceMode(p.i); break;
    case 'O' : TDestMode(p.i);   break;
    default  : MorePanic(UN_CMD, "ESC *v %c", p.letter); break;
    }
  }while(more);
}


void Ast_s(void)
{
  int more;
  param p;

  do {
    more = Parse(&p);
    switch(p.letter) {
    case 'T' : StatusMessageType(p.i); break;
    case 'U' : StatusMessageUnit(p.i); break;
    case 'I' : StatusMessage(p.i);     break;
    case 'M' : FreePrinterMemory(p.i); break;
    case 'X' : EchoValue(p.i);         break;
    default  : MorePanic(UN_CMD, "ESC *s %c", p.letter); break;
    }
  }while(more);
}


void Ast_c(void)
{
  int more;
  param p;

  do {
    more = Parse(&p);
    switch(p.letter) {
    case 'D' : SetFontID( p.i);             break;
    case 'E' : SetCharCode( p.i);           break;
    case 'F' : FontControl(p.i);            break;
    case 'A' : HorizRuleSize(DOTS, p.i);    break;
    case 'H' : HorizRuleSize(DECIPTS, p.i); break;
    case 'B' : VertRuleSize(DOTS, p.i);     break;
    case 'V' : VertRuleSize(DECIPTS, p.i);  break;
    case 'G' : PatternID(p.i);              break;
    case 'W' : DefPattern(p.i);             break;
    case 'Q' : PatternControl(p.i);         break;
    case 'P' : PrintRule(p.i);              break;
    case 'R' : SetSymbolID(p.i);            break;
    case 'S' : SymbolControl(p.i);          break;
    default  : MorePanic(UN_CMD,"ESC *c %c", p.letter); break;
    }
  }while(more);
}


/*
 * 'ESC *' sequences
 */

void Asterisk( void )
{
   int c;
   
   c = get_next_char();   /* get command class letter ... */
   switch(c){
   case 't' : Ast_t(); break;
   case 'b' : Ast_b(); break;
   case 'r' : Ast_r(); break;
   case 'p' : Ast_p(); break;
   case 'c' : Ast_c(); break;
   case 'v' : Ast_v(); break;
   case 's' : Ast_s(); break;
   default  : MorePanic( UN_CLASS, "ESC *%c", c); break;
   }
}


void percent( void )
/* no second command class letter; parameters/terminators follow immediately */
{
  int more;
  param p;

  do {
    more = Parse(&p);
    switch(p.letter) {
    case 'A' : UNIMPLEMENTED("Use Cursor position");      break;
    case 'X' : UnivLangExit(p.i);                         break;
    default  : MorePanic(UN_CLASS, "ESC %%%c", p.letter); break;
    }
  }while(more);
}



