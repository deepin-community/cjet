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
 *  misc.c  - misc / aux functions and initialization
 *
 */


#include "cjet.h"


/*
 * Requests for temporary buffers (raster data decompression, dumps
 * and font downloads) pass or die here. Allocates a buffer once and
 * realloc() to grow on request. Avoids excessive malloc()ing. 
 * bdesc structure has to be initialized with initial block size !
 *
 * reqsize  = new required size         
 * bdesc *f = pointer to buffer descriptor with current size
 * returns pointer to block
 * 
 */

BYTE *get_buffer( int reqsize, bdesc *f)
{
  if(f->p == NULL) {                      /* buffer is unallocated */
    f->p = (BYTE *) malloc(f->size);      /* allocate a block */     
    if(f->p == NULL) {                    /* that went wrong... */
      MorePanic(UN_NOMEM,"malloc() of temporary storage buffer failed");
      exit(1);                            /* ... and die. */
    }
  }
  /* if we get to here, the buffer exists. Check for size */
  if(reqsize > f->size) {
    reqsize *= 2;                    /* make it larger for the future */
    f->p = (BYTE *) realloc( f->p, reqsize);
    f->size = reqsize;
    if(f->p == NULL) {
      MorePanic(UN_NOMEM," realloc() of storage buffer failed");
      exit(1); 
    }
  }
  return (f->p);
}


/* 
 * program startup initialisation
 * FIXME: do printer hard reset here; download new startup macro 
 */

void cold_init( void )
{
  buffer.p        = NULL;            /* no main buffer yet */
  buffer.size     = M_BUFFER_SIZE;   /* initial size */
  aux_buffer.p    = NULL;            /* no aux buffer yet */
  aux_buffer.size = M_BUFFER_SIZE;
  dec_buffer.p    = NULL;            /* no decompression buffer */
  dec_buffer.size = M_DEC_SIZE;
  mac_buffer.p    = NULL;            /* no macro buffer */
  mac_buffer.size = M_MACRO_SIZE;

  verbose       = TRUE;              /* noisy running */ 
  ignorepaper   = FALSE;             /* don't ignore this */
  ignore        = FALSE;             /* don't ignore download fonts */
  paintmode     = 0;                 /* use CaPSL partial paint mode */
  page_x_offset = 0;
  page_y_offset = 0;
}


/*
 * reset printer defaults
 * As this function can be called during execution, make sure no options 
 * from the command line are 'overruled'
 * FIXME: let fonts registered as permanent survive
 * FIXME: call startup macro defined by cold_init()
 */

void InitCaPSL(void)
{ 
  paper_size    = DEF_PAPER;   /* default paper size is set in config.h */
  landscape     = FALSE; 
  line_spacing  = 6;           /* 6 lines per inch */
  line_VMI      = 120;         /* 120/720 inch per line */
  print_pitch   = 10;          /* 10 cpi */
  print_HMI     = 1000;        /* 10 cpi * 100 */
  top_margin    = DEF_TOP_MARGIN; 
  bottom_margin = DEF_BOT_MARGIN;
  last_top      = -1;           /* CaPSL: no margins are set on power up */
  last_bot      = -1; 
  left_margin   = 0;   
  right_margin  = 0;           /* i.e. rightmost position */
  shift_state   = SI;         

  resolution    = 75;          /* startup resolution */
  r_start       = FALSE;       /* no raster graphics */
  r_compress    = 0;           /* no compression */

  tface_p       = DEF_TFACE;   /* primary typeface; default Courier */
  tface_s       = DEF_TFACE;   /* secondary typeface; default Courier */
  set_p         = DEF_SET;     /* default symbol set */  
  set_s         = DEF_SET;
  char_size     = 120;         /* default font is 12pt */
  CR_status     = 0;           /* CR=CR LF=LF FF=FF  */

  pos_unit      = LINECOL;     /* CaPSL default movement mode */
  pos_sub_unit  = DECIPTS;     /* CaPSL default submode       */

  r_free        = 0;         
  pos_index     = STACKBEGIN;  /* memorized position stack */
  pcl_font_id   = 0;           /* PCL download font numbering begins here */
  c_font_id     = 0;           /* CaPSL download font numbering */
  c_font_used   = 0;           /* CaPSL number of used fonts */

  kill_descriptors();          /* remove any font descriptor entries */
  select_id     = -1;          /* no selected character set */
  recording     = FALSE;       /* no macro recording in progress */
  macro_id      = -1;          /* no current macro id */

  /* 
   * Begin ISO mode; soft reset; paint mode; disable auto ff;
   * enable scalable fonts
   * select default symbol set
   */

  PutString("\033;\033<\033[%d&z\033[?2h\033[?32h", paintmode);    
  SetSymbolSet( PRIMARY, DEF_SET);
  paperinit();       /* init paper defaults */  

  /* 
   * move to new (adjusted) origin: 
   */
  PutString("\033[0;%d;2x", S_CURRENTTOP);    /* restore top margin */
  PutString("\033[0;%d;1x", S_CURRENTLEFT);   /* restore left margin */
}


/* 
 * called at program termination:
 * free lists and any used malloc()ed blocks.
 */

void terminate( void )
{
  kill_descriptors();
  if(buffer.p != NULL) {
    MorePanic(UN_OTHER, "Buffer used: %d kB", (buffer.size / 1024));
    free(buffer.p);
  }
  if(aux_buffer.p != NULL) {
    MorePanic(UN_OTHER, "Aux buffer used: %d kB", (aux_buffer.size / 1024));
    free(aux_buffer.p);
  }
  if(dec_buffer.p != NULL) {
    MorePanic(UN_OTHER, "Dec buffer used: %d kB", (dec_buffer.size / 1024));
    free(dec_buffer.p);
  }
  if(mac_buffer.p != NULL) {
    MorePanic(UN_OTHER, "Macro buffer used: %d kB", (mac_buffer.size / 1024));
    free(mac_buffer.p);
  }
}


void MorePanic( int errtype, char *format, ...)
{
  va_list stuff;
  va_start(stuff, format);

  if (verbose) {
    fprintf(stderr, "%s: ", prog_name);
    switch(errtype) {
    case UN_CMD:   fprintf(stderr, "Unrecognized command: ");         break;
    case UN_CLASS: fprintf(stderr, "Unrecognized class: ");           break;
    case UN_PARAM: fprintf(stderr, "Garbage in command parameter: "); break;
    case UN_OTHER:                                                    break;
    case UN_NOMEM: fprintf(stderr, "Out of memory: ");                break; 
    case UN_RFILE: fprintf(stderr, "File read error: ");              break;
    case UN_WFILE: fprintf(stderr, "File write error: ");             break;
    }
    vfprintf(stderr, format, stuff);
    fprintf(stderr,"\n");
  }
  va_end(stuff);
}


