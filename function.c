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
 *  function.c  -  misc functions 
 * 
 * Clear Side Margins             - ESC 9
 * Enter Display Functions Mode   - ESC Y
 * Exit Display Functions Mode    - ESC Z 
 * Half Line Feed                 - ESC =
 * Reset Printer                  - ESC E
 * Self Test                      - ESC z
 * set default fonts              - ESC ( # @ 
 * select symbol set              - ESC ( # ID
 * Set fixed/proportional set     - ESC (s # P
 * Set Pitch in cpi               - ESC (s # H 
 * Set character height in pts    - ESC (s # V 
 * Set character style            - ESC (s # S 
 * set character strokeweight     - ESC (s # B
 * select a typeface              - ESC (s # T  
 * set left margin                - ESC &a # L
 * set right margin               - ESC &a # M 
 * transparent print              - ESC &p # X 
 * Set underline mode             - ESC &d # d
 * Disable underline              - ESC &d @
 * Push / Pop position            - ESC &f # S 
 * Set HMI value in 1/120in units - ESC &k # H
 * Set Print Pitch                - ESC &k # S 
 * Line Termination mode          - ESC &k # G 
 * Set Lines per Inch             - ESC &l # D 
 * Set VMI in 1/48 in units       - ESC &l # C
 * Select Perforation Skip Mode   - ESC &l # L 
 * Set number of copies           - ESC &l # X 
 * Set word wrap mode             - ESC &s # C 
 * Set units per inch             - ESC &u # D      (PCL 5 - LJ4l)
 * Universal language exit        - ESC %-12345X    (PCL 5)
 *
 * UNIMPLEMENTED("Print test page");
 * UNIMPLEMENTED("Macro Reference point");
 * UNIMPLEMENTED("ESC *s <id> T\n");
 * UNIMPLEMENTED("ESC *s <id> U\n");
 * UNIMPLEMENTED("ESC *s <id> I\n");
 * UNIMPLEMENTED("ESC *s <id> M\n");
 * UNIMPLEMENTED("ESC *s <id> X\n");
 * UNIMPLEMENTED("ESC &r <op> F\n");
 */

#include "cjet.h"


/* 
 * orientation of user defined pattern ?
 */

void MacroRef( int op )
{
  UNIMPLEMENTED("Macro Reference point");
}


/*
 * Set new movement modes, if and only if necessary.
 */

void SetModes( int mode, int submode) 
{
  if(mode != pos_unit){               /* if new mode != current mode */
    pos_unit = mode;                  /* set a new mode              */    
    if(mode == LINECOL) 
      PutString("\033[11l");          /* set line/col mode */
    else 
      PutString("\033[11h");          /* set Size Unit Mode */
  }
  if(mode == SIZEUNIT) {              /* change submode only if in size */
    if (submode != pos_sub_unit) {    /* unit mode */
      pos_sub_unit = submode;
      PutString("\033[%d I",submode);
    }
  }
}


/*
 * Clear Side Margins - ESC 9
 * Sets the left margin saved in init_side_margin() as the new left margin.
 * Sets the right margin saved in init_side_margin() as the new right margin.
 * saved positions are adjusted for page_x_offset
 */

void ClearSideMargins( void )
{
  PutString("\033[0u");                     /* clear side margins */
  PutString("\033[1;%d;0x", MARGININDEX);   /* save CAP */
  PutString("\033[0;%d;1x", S_LEFTMARGIN);  /* restore init left margin */
  PutString("\033[1;%d;1x", S_CURRENTLEFT); /* save left margin */
  PutString("\033[0t");                     /* set left margin */
  PutString("\033[0;%d;1x", S_RIGHTMARGIN); /* restore init right marg. */
  PutString("\033[1t");                     /* set right margin */
  PutString("\033[0;%d;0x", MARGININDEX);   /* restore CAP */
  left_margin = 0;
  right_margin = 0;
}



/*
 * set a new Shift state (SI or SO), if necessary
 */

void SetShiftState( int mode )
{
  if (mode != shift_state){
    put_next_char(mode);
    shift_state = mode;
  }
}

 

/*
 * Enter Display Functions Mode - ESC Y
 * waits for ESC Z - Exit DF Mode, ignoring data
 */

void DisplayFunctions( void )
{
  BYTE c;
  do { 
   while((c=get_next_char())!=ESC);
  } while ((c=get_next_char())!='Z');
}


/*
 * Half Line Feed - ESC =
 */

void HalfLineFeed( void )
{
  PutString("\033K");
}


/*
 * Reset Printer - ESC E
 */

void Reset( void )
{
  InitCaPSL();
}


/*
 * Self Test - ESC z
 * prints a test page some day 
 */

void SelfTest( void )
{
  UNIMPLEMENTED("Print test page");
}



/* 
 * set default fonts - ESC ( # @ 
 * <action>: 0,1 ... set default symbol set as <which> symbol set (PCL 4)
 *           2   ... Reselect current font                        (PCL 4)
 *           3   ... Select default font as <which> font          (PCL 5)
 *
 * Ignore action 0-2 for symbol set selections other than to default, which
 * is HP-ROMAN (older PCL behaviour ?).
 * The Laserjet IIIp manual only lists action 3 - set default fonts.
 */

void SetDefaultFont( int which, int action )
{
  switch(action)
    {
    case 0 :
    case 1 : SetSymbolSet( PRIMARY, HP_ROMAN ); break;
    case 2 : break;
    case 3 : SetTypeface( which, HP_COURIER ); break;
    }
}



/* 
 * select symbol set - ESC ( # ID
 * Some PCL symbol sets have matching CaPSL graphic sets. Others are
 * emulated by printing available characters from other graphic sets.
 * (PC850 Multilingual, PostScript) This is done in symbol.c:translate()
 * 
 * PCL symbol set ids consist of 'val' and 'terminator', e.g. '8U'
 * Setnum is calculated as (val * 32) + (terminator - 64).
 * 
 * Setting 8-bit symbol sets as secondary sets: 
 * set secondary set of 8-bit symbol set as new secondary set;
 * leave primary set as is
 */

void SetSymbolSet( int which, int setnum )
{
  char sw; 

  if(which == PRIMARY) { /* only relevant for 7-bit symbol sets */
    set_p = setnum;
    sw    = '(';
  }
  else {
    set_s = setnum;
    sw    = ')';
  }
  switch(setnum) {
  case HP_LEGLSET : PutString("\033%cB",sw); break;  /* ASCII + subst */
  case HP_NORWEG  : PutString("\033%cE",sw); break;  /* Norwegian */
  case HP_ITAL    : PutString("\033%cY",sw); break;  /* Italian */
  case HP_SWED    : PutString("\033%cC",sw); break;  /* Swedish */
  case HP_ASCII   : PutString("\033%cB",sw); break;  /* US ASCII */
  case HP_UKENG   : PutString("\033%cA",sw); break;  /* UK ASCII */
  case HP_FRENCH  : PutString("\033%cR",sw); break;  /* French */
  case HP_GERMAN  : PutString("\033%cK",sw); break;  /* German */
  case HP_SPANISH : PutString("\033%cZ",sw); break;  /* Spanish */

  case HP_PSTEXT  : { if(which == PRIMARY) {
                        PutString("\033(%s\033)%s",C_PSL, C_PSR); 
			set_s = setnum;
		      } else 
			PutString("\033)%s", C_PSR); 
		      break;
		    }
  /* these two can be emulated using the Symbol typeface and graphic set */
  case HP_VMATH   : 
  case HP_PSMATH  : { if ( which == PRIMARY ) {
                        PutString("\033(%s\033)%s",C_SYML, C_SYMR);  
			set_s = setnum;
                      } else 
			PutString("\033)%s", C_SYMR);  
		      break;
		    }
  /* the next are emulated by IBML and PC850, PSR, Symbol(?) set(s) */
  case HP_ROMAN     :   /* Roman-8               */
  case HP_DESKTOP   :   /* Desktop               */
  case HP_ISOLATIN1 :   /* ISO 8859-1 Latin 1    */
  case HP_WIN30     :   /* Win 3.0 Latin 1       */
  case HP_WIN31     :   /* Win 3.1 Latin 1       */
  case HP_MCTEXT    :   /* MC Text               */
  case HP_VINTL     :   /* Ventura International */
  case HP_VUS       :   /* Ventura US            */
  case HP_PC850     : { if(which == PRIMARY) {
                          PutString("\033(%s\033)%s", C_IBML, C_IBM850);
			  set_s = setnum;
			} else 
                          PutString("\033)%s", C_IBM850);
			break; 
		      }
  case HP_PCDN : { if(which == PRIMARY) {
                     PutString("\033(%s\033)%s", C_IBML, C_IBMR2);
		     set_s = setnum;
		   } else 
		     PutString("\033)%s", C_IBMR2);
		   break;
		 }
  case HP_PC_8 : { if(which == PRIMARY) {
                     PutString("\033(%s\033)%s", C_IBML, C_IBMR1);
		     set_s = setnum;
		   } else
		     PutString("\033)%s", C_IBMR1);
		   break; 
		 }
  case HP_PC_TRK : { if(which == PRIMARY) {
                       PutString("\033(%s\033)%s", C_IBML, C_TURKISH);
                       set_s = setnum;
                     } else
		       PutString("\033)%s", C_TURKISH);
		     break;
		   }
  default : MorePanic(UN_OTHER,"Unknown symbol set: %d", setnum); break;
  }  
}


/*
 * Set fixed/proportional for <which> character set - ESC ( s # P
 * UNKNOWN if the Canon command works for both sets or for any set separately
 * value: 0 fixed, 1 proportional
 */

void SetSpacing(int which, int value)
{
  int old;
  
  old = shift_state;
  if(which == PRIMARY)
    SetShiftState(SI);
  else
    SetShiftState(SO);
  if (value == 0)       
    PutString("\033[?8h");      /* disable proportional spacing */
  else
    /* enable proportional spacing and set proportional pitch */
    PutString("\033[?8l\033[?0 K"); 
  SetShiftState(old);
}


/* 
 * Set Pitch in cpi -  ESC (s # H 
 */

void SetPitch( int which, double value)
{
 int old, pitch;
  
 old = shift_state;
 value *= 100;
 pitch = (int) (value);

 if(which == PRIMARY)
   SetShiftState(SI);
 else
   SetShiftState(SO);
 PutString("\033[?%d K",pitch);         /* select character pitch */
 SetShiftState(old);
 print_HMI = pitch;
}


/* 
 * Set character height in pts (1/72 in) - ESC (s # V 
 * Set Canon size in decipts, so multiply by 10 
 */

void SetHeight(int which, double value)
{
 int old, size;
  
 old = shift_state;
 value *=10;
 size = (int) value;
 SetModes(SIZEUNIT,DECIPTS);
 if(which == PRIMARY)
   SetShiftState(SI);
 else
   SetShiftState(SO);
 PutString("\033[%d C",size);          /* select character size */
 SetShiftState(old);
 char_size = size;
}


/* 
 * Set character style - ESC (s # S 
 * FIXME: PCL 5 allows more effects
 */

void SetStyle(int which, int value)
{ 
  int old;
  
  old = shift_state;
  if(which == PRIMARY)
    SetShiftState(SI);
  else
    SetShiftState(SO);
  if (value == 0)       
    PutString("\033[23m");    /* upright characters */
  else
    PutString("\033[3m");     /* italic characters */
  SetShiftState(old);
}


/* 
 * set character strokeweight - ESC (s # B
 */

void SetStroke(int which, param *p)
{
  int old, weight;
 
  old = shift_state;
  
  if(which == PRIMARY)
    SetShiftState(SI);
  else
    SetShiftState(SO);

  weight = p->i;
  if (p->rel == NEGATIVE) 
    weight = -weight;
  /* isn't this grotty ? */ 
  switch(weight) {
  case -7 : PutString("\033[2;2;2;2;2;2;2m"); break;
  case -6 : PutString("\033[2;2;2;2;2;2m");   break;
  case -5 : PutString("\033[2;2;2;2;2m");     break;
  case -4 : PutString("\033[2;2;2;2m");       break;
  case -3 : PutString("\033[2;2;2m");         break;
  case -2 : PutString("\033[2;2m");           break;
  case -1 : PutString("\033[2m");             break;
  case  0 : PutString("\033[22m");            break;
  case  1 : PutString("\033[1m");             break;
  case  2 : PutString("\033[1;1m");           break;
  case  3 : PutString("\033[1;1;1m");         break;
  case  4 : PutString("\033[1;1;1;1m");       break;
  case  5 : PutString("\033[1;1;1;1;1m");     break;
  case  6 : PutString("\033[1;1;1;1;1;1m");   break;
  case  7 : PutString("\033[1;1;1;1;1;1;1m"); break;
  }
  SetShiftState(old);
}


/* 
 * set left margin in columns in current pitch - ESC &a # L
 * adjust for page_x_offset
 */

void SetLeftMargin( int pos)
{ 
  int x = page_x_offset;

  if(pos == 0)           /* clear left ( and right... ) margins */
    ClearSideMargins();
  else {
    PutString("\033[1;%d;0x",MARGININDEX);       /* save CAP */
    SetModes(LINECOL,0);                         /* set line mode */
    PutString("\033[%d`",pos );                  /* move absolute horizontal */
    if(x != 0) {
      SetModes(SIZEUNIT,DECIPTS);                /* adjust for page_x_offset */
      if (x > 0) 
	PutString("\033[%da",x);                 /* move right */
      else 
	PutString("\033[%dj", -x);               /* move left */
    }
    PutString("\033[0t");                        /* set left margin */
    PutString("\033[1;%d;1x",S_CURRENTLEFT);     /* save left margin */
    PutString("\033[0;%d;0x",MARGININDEX);       /* restore CAP */
    left_margin = pos;
  }
}


/* 
 * set right margin in columns in current pitch - ESC &a # M 
 * adjust for page_x_offset
 */

void SetRightMargin( int pos)
{ 
  int x = page_x_offset;

  if(pos == 0)           /* clear left ( and right... ) margins */
    ClearSideMargins();
  else {
    PutString("\033[1;%d;0x",MARGININDEX);       /* save CAP */
    SetModes(LINECOL,0);                         /* set line mode */
    PutString("\033[%d`",pos);                   /* move absolute horizontal */
    if(x != 0) {
      SetModes(SIZEUNIT,DECIPTS);                /* adjust for page_x_offset */
      if (x > 0) 
	PutString("\033[%da",x);                 /* move right */
      else 
	PutString("\033[%dj", -x);               /* move left */
    }
    PutString("\033[1t");                        /* set right margin */
    PutString("\033[0;%d;0x",MARGININDEX);       /* restore CAP */
    right_margin = pos;
  }
}


/* 
 * MoveHoriz - move CAP horizontal, absolute or relative.
 * when moving absolutely, adjust by page_x_offset
 *
 * MoveVert - move CAP vertically 
 * when moving absolutely, adjust by page_y_offset
 */

void MoveHoriz(int mode, int submode, param *p)
{
  int x = page_x_offset;

  SetModes(mode, submode); /* set correct movement mode */
  if(r_start == TRUE ) {   /* we have to print any graphics */
    PurgeRaster(0);        /* before moving about the page */
    if( r_compress == 3 ) 
      clear_decompress_buffer();
  }
  switch( p->rel ) {
  case NONE : { 
    PutString("\033[%ld`", p->i);             /* do the move */
    if(x != 0) {                              /* if there's an offset */
      SetModes(SIZEUNIT,DECIPTS);
      if (x > 0)                              /* going right */
	PutString("\033[%da", x);
      else
	PutString("\033[%dj", -x);            /* going left */
    }
    break; 
  } 
  case POSITIVE : PutString("\033[%lda",p->i); break;       /* right */
  case NEGATIVE : PutString("\033[%ldj",p->i); break;       /* left */
  default : MorePanic(UN_PARAM, "Movement");
  }
}


void MoveVert(int mode, int submode, param *p)
{
  int y = page_y_offset;
  
  SetModes(mode, submode);  /* set correct movement mode */
  if(r_start == TRUE ) {    /* to avoid strange results, we have to print */
    PurgeRaster(0);         /* any graphics before moving about the page  */
    if( r_compress == 3 ) 
      clear_decompress_buffer();
  }
  switch( p->rel ) {
  case NONE : { 
    PutString("\033[%ldd", p->i);           /* do the move */
    if( y != 0) {                           /* if there's an offset ... */
      SetModes(SIZEUNIT, DECIPTS);
      if (y > 0)                            /* going down */
	PutString("\033[%de", y);
      else                                  /* going up */
	PutString("\033[%dk", -y);
    }
    break; 
  }
  case POSITIVE : PutString("\033[%lde",p->i); break;       /* down */
  case NEGATIVE : PutString("\033[%ldk",p->i); break;       /* up */
  default : MorePanic(UN_PARAM, "Movement" );
  }
}


/* 
 * transparent print - ESC &p [count] X 
 * print the next count bytes directly to the printer  
 */

void TransparentPrint( int count )
{
  BYTE *p;

  PutString("\033[%d.v",count); 
  get_buffer(count, &aux_buffer);
  p = aux_buffer.p;
  pipe_read(p, count);
  pipe_write(p, count);
}



/* 
 * Set underline mode - ESC &d # d
 *  ...0 Enable auto underline
 *  ...1 Single underline
 *  ...2 Double underline
 *  ...3 Single floating underline
 *  ...4 Double floating underline
 * LJ IIIp manual only lists modes 0 and 3. 
 */

void SetUnderLineMode( int mode )
{
  switch (mode) {
  case 0 : 
  case 1 : PutString("\033[4m");  break;       /* Single UL on */
  case 2 : PutString("\033[21m"); break;       /* Double UL on */

/* FIXME: This has to change to use the UL_... values or something... */

  case 3 : PutString("\033[4m");  break;       /* Single UL on */
  case 4 : PutString("\033[21m"); break;       /* Double UL on */
  default : break;
  }
}



/* 
 * Disable underline - ESC &d @
 */

void DisableUnderLine ( void )
{
  PutString("\033[24m");
}



/*
 * Push / Pop position - ESC &f # S 
 * Uses CaPSL memorized position CSI x and simulates a stack - will (perhaps)
 * be changed by also using a CaPSL stack system - push pop environment CSI %y
 * and CSI %z
 */

void PushPopPos( int op )
{
  if(op == 0) {                               /* Push current position */
    PutString("\033[1;%d;0x",pos_index++);    /* memorize position */
    if( pos_index>STACKLIMIT )                /* what if stack overruns */
      pos_index = STACKLIMIT;                 /* who cares ? */
  } else {
    PutString("\033[0;%d;0x",pos_index--);    /* pop current position */
    if( pos_index<STACKBEGIN)
      pos_index=STACKBEGIN;
  }
}


/*
 * Set HMI value in 1/120in units - ESC &k # H
 * convert to decipts (*6)
 */

void SetHMI( double f )
{
  int val;

  val = (int) (f*6);
  print_HMI = val;
  SetModes(SIZEUNIT,DECIPTS);
  PutString("\033[;%d G",val);
}


/*
 * Set Print Pitch - ESC &k # S 
 * # ... 0 Standard print pitch ( 10 cpi)
 * # ... 2 Compressed print pitch ( 16.66 cpi )
 * # ... 4 Elite ( 12 cpi ) ( PCL 5 )
 */

void SetPrintPitch( int set) 
{
  int v;

  switch(set)
    { 
    case  0 : v = 1000; break;
    case  2 : v = 1667; break;
    case  4 : v = 1200; break;
    default : v = 1000; break;
    }
  print_HMI = v;
  PutString("\033[?%d K",v);      /* select character pitch */
}


/*
 * Line Termination mode - ESC &k # G 
 * do it the messy way ...
 */

void LineTermination( int mode )
{
  switch(mode) {
  case 0 : PutString("\033[4l\033[?5l\033?6l");   break;
  case 1 : PutString("\033[?4h\033[?5l\033?6l");  break;
  case 2 : PutString("\033[4l\033[?5h\033[?6h");  break;
  case 3 : PutString("\033[?4h\033[?5h\033[?6h"); break;
  default: break;
  }
}



/*
 * Set Lines per Inch  - ESC &l #D 
 * convert lines per inch to VMI in 48ths inch and SetVMI
 */

void SetLinesPerIn( int lpi )
{
  double vmi;
  vmi = 48 / lpi;
  SetVMI(vmi);
}


/* 
 * Set VMI in 1/48in units - ESC &l # C
 * CaPSL: convert to decipoints
 */

void SetVMI(double value)
{
  int val;
  value *= 15;                        /* convert 1/48in to decipts */
  val = (int) value;
  SetModes(SIZEUNIT,DECIPTS);
  PutString("\033[%d; G",val);       /* Set VMI */
  line_VMI = val;                    /* VMI in decipoints */
}


/*
 * Select Perforation Skip Mode On / Off - ESC &l # L 
 */

void PerfSkipMode( int mode )
{
  if (mode == 1)
    PutString("\033[?2l");  /* Enable auto FF */
  else
    PutString("\033[?2h");  /* Disable auto FF */
}


/*
 * Set number of copies - ESC &l # X 
 */

void NumCopies(int num )
{
  PutString("\033[%dv", num);
}


/* 
 * select a typeface - ESC (s # T  
 * the CaPSL method wants both set at once *sigh*; so save the last values
 * and hope for the best...
 * pass through unrecognized values and hope the printer defaults 
 * to something readable
 */

void SetTypeface(int which, int value)
{ 
  int newvalue;

  switch(value) {
  case HP_UNIVERS  : newvalue = C_SWISS; break;
  case HP_COURIER  :
  case HP_COURIER2 : newvalue = C_COURIER; break;
  case HP_CGTIMES  : newvalue = C_DUTCH; break;
  default : newvalue = value; break;
  }
  if(which == PRIMARY)
    tface_p = newvalue;
  else
    tface_s = newvalue;
  PutString("\033[%d;%dy", tface_p, tface_s);
}



/* 
 * Set word wrap mode - ESC &s # C 
 * 0...on
 * 1...off
 */

void SetWrapMode(int mode)
{
  if(mode == 0)
    PutString("\033[?1h");
  else
    PutString("\033[?1l");
}


/*
 * units per inch for movement etc. - ESC & u # D
 */

void SetUnitsPerInch( int unit )
{
  switch (unit) {
  case 300 : SetModes(SIZEUNIT, DOTS); break;
  case 720 : SetModes(SIZEUNIT, DECIPTS); break;
  default : MorePanic(UN_OTHER, "function.c: SetUnitsPerInch(): unrecognized value %d", unit); break;
  }
}

/*
 * Universal language exit - ESC % value X
 * escape from current emulation to limbo
 */

void UnivLangExit(int what)
{
  /* does nothing yet. */
}


void StatusMessageType(int id)
{
  UNIMPLEMENTED("ESC *s <id> T\n");
}


void  StatusMessageUnit(int id)
{
  UNIMPLEMENTED("ESC *s <id> U\n");
}


void  StatusMessage(int id)
{
  UNIMPLEMENTED("ESC *s <id> I\n");
}


void  FreePrinterMemory(int op)
{
  UNIMPLEMENTED("ESC *s <id> M\n");
}


void  EchoValue(int value)
{
  UNIMPLEMENTED("ESC *s <id> X\n");
}


void ClearPages(int op)
{
  UNIMPLEMENTED("ESC &r <op> F\n");
}


