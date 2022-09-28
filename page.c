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
 * page.c - tedious page stuff
 *
 * Set Text length          - ESC &l # F
 * select output paper tray - ESC &l # G
 * Select Page Orientation  - ESC &l # O 
 * Set Page Length          - ESC &l # P 
 * Set Top Margin           - ESC &l # E
 * logical_page             - ESC &a # W [data]
 * Log. position across ?   - ESC &l # U
 * Log. position down ?     - ESC &l # Z
 * Logical page rotate      - ESC &a # P 
 * Paper Input Control      - ESC &l # H
 * SetPageSize              - ESC &l # A
 * UNIMPLEMENTED("Logical page positioning");
 */



#include "cjet.h"

/* page stuff debugging */
/* #define PDEBUG  */


struct PAPER {
  int p_length;             /* page length in decipoints, portrait */        
  int l_length;             /* page length in decipoints, landscape */
};

typedef struct PAPER paper;


/* 
 * Define some common (physical) paper lengths in DECIPTS for portrait and 
 * landscape orientations. Dubious. 
 *
 * A4     : 210mm x 297mm       (width x height) 
 * letter : 8.5in x 11in
 * legal  : 8.5in x 14in
 * exec   : 7.25in x 10.5 in
 * B5     : 180mm x 257mm ?
 * 
 */

paper A4_size      = {  8418, 5950 };
paper letter_size  = {  7920, 6120 };
paper legal_size   = { 10080, 6120 };
paper exec_size    = {  7560, 5220 };
paper B5_size      = {  7284, 5157 };


#ifdef PDEBUG
void paper_state( void )
{
  fprintf(stderr,"papersize = %d, orientation = %d, page length = %d\n",
	  paper_size, landscape, page_length);
  fprintf(stderr,"text length = %d, topmargin = %d, bottom margin = %d ",
	  text_length, top_margin, bottom_margin);
  if((text_length + top_margin + bottom_margin) == page_length)
    fprintf(stderr,"\n");
  else
    fprintf(stderr,"diffs!\n");
}
#endif



/*
 * initialize left and right margins; 
 * consider page_x_offset
 */

void init_side_margin( void )
{
  int x = page_x_offset;

  PutString("\033[1;%d;0x",MARGININDEX);   /* save CAP */
  PutString("\033[0;1;0x");                /* move to physical 0,0 */
  SetModes(SIZEUNIT,DECIPTS);
  if (x != 0) {
    if (x > 0) 
      PutString("\033[%da", x);            /* move to margin */
    else 
      PutString("\033[%dj", -x);           /* move left to margin */
  }
  PutString("\033[0t");                    /* set left margin */
  PutString("\033[1;%d;1x",S_LEFTMARGIN);  /* save left margin */
  PutString("\033[1;%d;1x",S_CURRENTLEFT); /* save left margin */
  PutString("\033[0;2;0x");                /* move to right corner */
  if (x != 0) {
    if (x > 0) 
      PutString("\033[%da", x);            /* move right to margin */
    else 
      PutString("\033[%dj", -x);           /* move left to margin */
  }
  PutString("\033[1t");                          /* set right margin */
  PutString("\033[1;%d;1x", S_RIGHTMARGIN);      /* save right margin */
  PutString("\033[0;%d;0x", MARGININDEX);        /* restore CAP */
}



/*
 * initialize top margin; 
 * consider page_y_offset, but don't set the margin yet
 */

void init_top_margin( void )
{
  int y = page_y_offset;

  PutString("\033[1;%d;0x", MARGININDEX);       /* save CAP */
  PutString("\033[0;1;0x");                     /* move to physical 0,0 */
  SetModes(SIZEUNIT,DECIPTS);
  if( y!= 0 )
    if (y > 0) 
      PutString("\033[%de", y);                 /* move down */
    else 
      PutString("\033[%dk", -y);                /* move up */
  PutString("\033[1;%d;2x", S_TOPMARGIN);       /* save init top margin */
  PutString("\033[0;%d;0x", MARGININDEX);       /* restore CAP */
}



/* 
 * CaPSL: Set current top_margin as the top margin in decipts 
 * If the top margin was already set to the same amount, ignore.
 * set: restore top of (physical) page position and move down from there
 * page_y_adjust has been considered in init_top_margin()
 */

void 
C_set_top_margin( void )
{
  if (top_margin != last_top) {
    PutString("\033[1;%d;0x", MARGININDEX);      /* save CAP */
    PutString("\033[0;%d;2x", S_TOPMARGIN);      /* restore top edge */
    SetModes(SIZEUNIT,DECIPTS);                  /* set DECIPTS mode */
    PutString("\033[%de", top_margin);           /* move down vertical */
    PutString("\033[2t");                        /* set top margin */
    PutString("\033[1;%d;2x", S_CURRENTTOP);     /* save curr. top margin */
    PutString("\033[0;%d;0x", MARGININDEX);      /* restore CAP */
    last_top = top_margin;
  }
}



/* 
 * CaPSL: Set the current bottom_margin in decipts 
 * Calculate position from top_margin and text_length
 * pos = top_margin + text_length + 1(?)
 * as page_length = top_margin + text_length + bottom_margin
 */

void
C_set_bot_margin( void )
{
  int pos;
  if(bottom_margin != last_bot)
    {
      pos =  top_margin + text_length + 1;
      PutString("\033[1;%d;0x", MARGININDEX);     /* save CAP */
      PutString("\033[0;%d;2x", S_TOPMARGIN);     /* restore top edge */
      SetModes(SIZEUNIT,DECIPTS);                 /* set DECIPTS mode */
      PutString("\033[%de", pos);                 /* move down vertical */
      PutString("\033[3t");                       /* set bottom margin */
      PutString("\033[0;%d;0x", MARGININDEX);     /* restore CAP */
      last_bot = bottom_margin;
    }
}


/*
 * Set default text length from page_length and top_margin. The bottom_margin
 * is set to the default value. The top_margin value is either the default 
 * value or has been set previously
 */

void
C_default_text_length( void )
{
  bottom_margin = DEF_BOT_MARGIN;
  text_length = page_length - bottom_margin - top_margin;
  C_set_bot_margin();
}


/* 
 * sets the top margin to default 
 */

void
C_default_top_margin( void )
{
  top_margin = DEF_TOP_MARGIN;
  C_set_top_margin();
}


/* 
 * Get page length in decipoints for given CaPSL page size parameter and 
 * current orientation
 */

void
C_default_page_length( void )
{
  paper *p;

  switch(paper_size)
    {
    case C_LETTER : p = &letter_size; break; 
    case C_LEGAL  : p = &legal_size;  break;
    case C_EXEC   : p = &exec_size;   break;
    case C_A4     :    
    default       : p = &A4_size;     break;
    }
  page_length = (landscape ? p->l_length : p->p_length); 
  C_default_top_margin();
  C_default_text_length();
}

 
/* 
 * Set Text length - ESC &l # F
 *
 * Set number of printable lines in the current line spacing. This 
 * implicitly sets the bottom margin.
 * bottom margin = pagelength - topmargin - textlength  
 */

void
SetTextLength( int lines )
{
  if (lines == 0) 
    C_default_text_length();
  else
    {
      text_length = lines * line_VMI;             /* convert to decipoints */
      bottom_margin = page_length - text_length - top_margin;
      if (bottom_margin < 0) {
	bottom_margin = 0;
	MorePanic(UN_OTHER,"SetTextLength(): Bottom Margin exceeds page");
      } else
	C_set_bot_margin();
    }
#ifdef PDEBUG
  fprintf(stderr,"SetTextlength():\n");
  paper_state();
#endif
}


/*
 * Select Page Orientation - ESC &l # O 
 * 0...select Portrait mode
 * 1...select Landscape Mode 
 * 2...select reverse Portrait (PCL 5)
 * 3...select reverse Landscape (PCL 5)
 * CaPSL: select current paper size command for portrait (paper size x)
 *        or landscape (paper size x + 1)
 * The PCL SetPageOrientation command is usually (?) sent after the 
 * SetPageSize command. If the -p command line option was set, only the
 * CaPSL Orientation will be changed. Otherwise the Page Size will also
 * be set. 
 */

void
SetPageOrient( int mode )
{ 
  switch(mode) {
  case 0:
  case 2: landscape = FALSE; break;
  case 1:
  case 3: landscape = TRUE; break;
  default: break;
  }
  mode = paper_size + landscape;
  PutString("\033[%dp", mode);
  last_top = 0;
  last_bot = 0;
  C_default_page_length();
#ifdef PDEBUG
  fprintf(stderr,"SetPageOrient():\n");
  paper_state();
#endif
}


/* 
 * Set Page Length - ESC &l # P 
 * set number of lines in the current line spacing 
 * calculate length in decipts
 */

void
SetPageLength( int lines )
{
  page_length = lines * line_VMI;
  C_default_top_margin();
  C_default_text_length();
#ifdef PDEBUG
  fprintf(stderr,"SetPageLength():\n");
  paper_state();
#endif

}



/* 
 * Set Top Margin - ESC &l # E
 * convert top margin in #lines in current line spacing to decipts 
 *
 */

void 
SetTopMargin( int lines )
{ 
  if(lines == 0)  /* clear top ( and bottom ...) margins */
    {
      PutString("\033[1u");
      bottom_margin = 0;
      last_top = -1;              /* will be set in C_set_top_margin */
      last_bot = -1;
      text_length = page_length;
    }
  top_margin = lines * line_VMI;    /* convert margin to decipts */
  C_set_top_margin(); 
  C_default_text_length();
  
#ifdef PDEBUG
  fprintf(stderr,"SetTopMargin():\n");
  paper_state();
#endif

}


/*
 * select output paper tray - ESC &l # G
 * 1..upper output tray; 2..rear output tray
 */

void 
paper_tray( int sel )
{
  if((sel==2) && (verbose))
    MorePanic(UN_OTHER,"Open rear paper output tray...");
  /* or plain ignore it */
}


/* 
 * Set Page Size - ESC &l # A
 * FIXME for other page and envelope sizes
 * 
 */

void 
SetPageSize( int size )
{
  paper_size = DEF_PAPER;
  if(ignorepaper == FALSE) {
    switch(size) {
    case HP_LETTER : paper_size = C_LETTER; break;
    case HP_EXEC   : paper_size = C_EXEC;   break;
    case HP_A4     : paper_size = C_A4;     break;
    case HP_LEGAL  : paper_size = C_LEGAL;  break;
    default: MorePanic(UN_OTHER,"Ignored SetPageSize(): Unknown size %d",size);
      break;
    } 
    /*  FIXME ignore ? (This is also set in SetPageOrientation())*/
    /*   PutString("\033[%dp", csize); */
  } /* ignorepaper */
  C_default_page_length();

#ifdef PDEBUG
  fprintf(stderr,"SetTopMargin():\n");
  paper_state();
#endif

}


/*
 * Paper Input Control - ESC &l # H
 */

void 
PaperControl( int op )
{
  switch(op) {
  case 0 : put_next_char(FF);     break;      /* eject the current page */
  case 2 : PutString("\033[1q");  break;      /* manual feed */
  case 1 :                                    /* feed paper from cassette */
  case 3 :                                    /* manual envelope feed */
  case 4 :                                    /* lower slot ? */
  case 5 :                                    /* lower cassette */
  default : PutString("\033[0q"); break;
  }
}


/* 
 * logical_page - ESC &a # W [data]
 * specify user defined logical page data 
 * FIXME (unknown)
 * read binary data and ignore for now
 */

void 
logical_page( int size )
{
  BYTE *p;
  p = get_buffer(size, &buffer);
  pipe_read(p,size);
}


/* 
 * position the logical page by some amount in decipts in some direction.
 * *if* this is: position the logical page over the physical page, then 
 * approximate the effects like the -x and -y command line switches.
 * FIXME
 *
 * ESC &l # U - position across the page (?)
 * ESC &l # Z - position down the page (?)
 */

void 
log_position( param *p)
{
  UNIMPLEMENTED("Logical page positioning");
}


/*
 * Logical rotate ESC &a # P - rotate page about 
 * CaPSL: use Page rotation command CSI %r to approximate this function;
 * PCL allows multiples of 90 degrees 
 */

void 
logical_rotate( int deg )
{
  switch(deg) {
  case 90  :
  case 270 : { PutString("\033[1%%r");  /* enable Landscape */ 
	       landscape = TRUE;
	       break; 
	     }
  case 0   : 
  case 180 : { PutString("\033[0%%r");  /* enable Portrait */ 
	       landscape = FALSE;
	       break;
	     }
  default : break;
  }
  top_margin = 0;     /* rotate command clears all margins */
  bottom_margin = 0;
  last_top = 0;
  last_bot = 0;
  text_length = page_length;

#ifdef PDEBUG
  fprintf(stderr,"SetPageRotation():\n");
  paper_state();
#endif

}


void 
paperinit( void )
{
  init_top_margin();
  init_side_margin();
  C_default_page_length();

#ifdef PDEBUG
  fprintf(stderr,"paperinit():\n");
  paper_state();
#endif

}

