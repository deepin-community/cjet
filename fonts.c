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
 *  module :  fonts.c  -  download font stuff 
 *
 * Specify Font ID               - ESC *c # D 
 * Specify Character Code        - ESC *c # E 
 * Font and character control    - ESC *c # F 
 * Set download font             - ESC ( / ) # X 
 * download a character          - ESC (s # W  
 * Create Font Descriptor        - ESC )s # W 
 */


#include "cjet.h"


/* 
 * create a new font descriptor and add it to the list
 */

Fptr new_descriptor( void )
{
  Fptr p,q;

  q = (Fptr) malloc(sizeof(FontDesc));
  if(q == 0){
    MorePanic(UN_NOMEM," malloc() font descriptor failed");
    exit(1);
  }
  if(FontList == 0)       /* empty list */
    {
      FontList    = q;  
      q->fd_prev  = 0;
      q->fd_next  = 0;
    } 
  else
    {
      p = FontList;       /* insert at head of list */
      p->fd_prev = q;
      q->fd_next = p;
      q->fd_prev = 0;
      FontList   = q;
    }
  return(q);
}


/* 
 * kill_descriptors
 * walk through the list and free() each entry
 */

void kill_descriptors( void )
{
  Fptr p,q;

  p = FontList;
  while(p != NULL) {
    q = p->fd_next;
    free(p);
    p = q;
  }
  FontList = NULL;
}


/*
 * Specify Font ID - ESC *c # D 
 * pcl_font_id is current PCL font id for download font ops
 */
 
void SetFontID( int id )
{
  pcl_font_id = id;
}


/*
 * Specify Character Code - ESC *c # E 
 * char_code is current download op code 
 */

void SetCharCode( int code )
{
  char_code = code;
}


/*
 * search the PCL->CaPSL font id list and return pointer to the font
 * descriptor matching the PCL id <fontid>;
 * return NULL if no match was found; in this case no selection will
 * take place. This "fixes" (dvilj) PCL code which selects a non-existing
 * download font and then prints raster graphic characters
 * 
 */

Fptr get_handle(int fontid)
{
  Fptr p;

  p = FontList;    
  while (p!=0) {
    if(p->pcl_id == fontid)
      return(p);
    else
      p = p->fd_next;
  }
  MorePanic(UN_OTHER, "Unknown PCL font descriptor %d ignored", fontid);
  return(NULL);
}



/* 
 * Font and character control - ESC *c [op] F 
 * delete downloaded character sets 
 * make dl sets temp or permanent - slightly bogus as all CaPSL dl fonts
 * are sent as temporary fonts
 */

void FontControl( int op )
{
  int  kill;
  Fptr p, q, r;
  
  kill = PERMANENT;    /* TEMP < PERM < TEMPERM */
  switch(op) 
    {
    case 0 :   /* delete temporary and permanent dl fonts */
      kill = TEMPERM;
    case 1 :   /* delete temporary dl fonts */
      { 
	p = FontList;    
	while (p!=0) {
	  q = p->fd_next;
	  if( kill > p->pcl_temp )
	    {
	      PutString("\033[%d;1&p", p->c_id_p);
	      c_font_used--;
	      if(p->c_id_s > -1) { 
		PutString("\033[%d;1&p", p->c_id_s);
		c_font_used--;
	      }
	      free(p);
	    }
	  p = q;
	}
	break;
      } 
    case 2 :  /* delete last specified dl font */
      {    
	p = get_handle(pcl_font_id); 
	if(p) { 
	  q = p->fd_prev;
	  r = p->fd_next;
	  r->fd_prev = q;
	  q->fd_next = r;
	  PutString("\033[%d;1&p", p->c_id_p);
	  c_font_used--;
	  if(p->c_id_s > -1) {
	    PutString("\033[%d;1&p", p->c_id_s);
	    c_font_used--;
	  }
	  free(p);
	} /* don't do anything if no match PCL->CaPSL was found */
	break;
      }
    case 4 :   /* make last specified dl font temporary */ 
      { 
	p = get_handle(pcl_font_id);
	if(p)
	  p->pcl_temp = TEMPORARY;
	break;
      }
    case 5:   /* make last specified dl font permanent */ 
      {
	p = get_handle(pcl_font_id);
	if(p)
	  p->pcl_temp = PERMANENT;
	break;
      }
    case 6:   /* copy current font as specified dl font id */
      {
	break;
      }
    }
}



/*
 * set a download font and adjust current symbol set to avoid
 * translate() translating incorrectly
 */

void set_stuff( int id, int *set, int *save)
{
  CSSelect(id);             /* select CaPSL font */
  if ( *set != DOWNLD)      /* if prev selection was not dl font... */
    { 
      *save = *set;         /* save prev. symbol set id */
      *set  = DOWNLD;       /* turn off translate() */
    }
}


/* 
 * Set download font <fontid> as <which> font - ESC( # X 
 *
 *  
 * setting a PCL 8-bit download font sets the primary CaPSL font and 
 * - if available - the secondary CaPSL font
 */

void SetDownLoadFont( int which, int fontid)
{
  int type, old;

  old  = shift_state;             /* get current shift state */
  type = FindFont( fontid );      /* get CaPSL assign numbers */

  if(which == PRIMARY)            /* set as primary font */
    {
      SetShiftState(SI); 
      set_stuff(c_dl_prim, &set_p, &save_p);
      if (c_dl_sec != -1)         /* set secondary font because it's there */
	{
	  SetShiftState(SO); 
	  set_stuff(c_dl_sec, &set_s, &save_s);
	}
    }
  else                       /* use primary id to set secondary font */
    {                        /* useful only with 7-bit fonts...      */
      SetShiftState(SO);
      set_stuff(c_dl_prim, &set_s, &save_s);
    }
  SetShiftState(old);
}


/* 
 * select a CaPSL download character set by a number which is assigned 
 * at download font creation time. Also seems correct
 */

void CSSelect(int ID)
{
  if(ID != select_id) {
    PutString("\033[%d%%v", ID);
    select_id = ID;
  }
  /* else was already selected */
}


/* 
 * find the corresponding CaPSL font ids to a PCL font id           
 * there is no one-to-one correspondence as a possible PCL 8-bit set 
 * has to be `approximated' by two CaPSL 7-bit sets
 */

int FindFont(int pclid)
{
  Fptr p;

  p = get_handle(pclid);
  if(p) {
    c_dl_prim = p->c_id_p;
    c_dl_sec  = p->c_id_s;
    return(p->pcl_type);
  }
  else
    return(0);
}


/*
 * create a CaPSL font using a font descriptor generated from a PCL
 * font descriptor and an id number
 * control character flag '1 means control characters retain their control
 * functions even if characters have been downloaded at their positions;
 * this requires some trickery as to when to print control chars and when
 * printable chars. 
 * flag '0' causes other problems: all characters are printable, some 
 * control codes (e.g. FF) are printed even when a real FF was wanted...
 * (dvilj2p output)
 */

void create_canon_font( int id, Fptr f )
{
  PutString("\033P%dxFont%c.SET\033\\", id, id+65);
  PutString("\033[0;0;0;");  /* 0 bytes, 0 chars reg'd, no priority */
  PutString("%d;%d;", f->c_orient, f->c_gs);
  PutString("%d;%d;%d;", f->c_flag, f->c_pitch, f->c_csize);
  PutString("%d;%d;%d;%d;%d;", f->c_style, f->c_stroke, f->c_tface, f->c_cwidth, f->c_cheight);
  PutString("%d;0;0;0;0;1.p", f->c_bline); /* control char flag is 1 */
  c_font_id++;
  c_font_used++;
}



/* 
 * download a character of <size> bytes - ESC (s # W  
 * uses current character code in global var 'CharCode'.
 *
 * Font Orientation: if the PCL download font header specifies Landscape,
 * assume an old-style (?) landscape font definition and rotate the character 
 * pattern; else check the current page orientation ('landscape') and set
 * the dl font orientation accordingly.  
 */

void DownLoadChar( int in_size)
{
  UINT Assign, csize;
  WORD dummy;
  int orient;
  long pitch;
  Fptr f;

  BYTE *out_buf = 0, *out_line, *out_copy, *p;
  BYTE *in_line, *in_copy;
  WORD in_top, in_left, out_top, out_left;
  UINT out_w, out_h;
  int in_bpr, out_bpr, out_size, in_bits;
  int ls_lines, i, j;
  BYTE in_mask, out_mask;
  
  p = get_buffer(in_size, &buffer);

  pipe_read(p, in_size);
  orient = p[4];                    /* character orientation */
  pitch  = (p[14]*256+p[15])/4;
  if(orient == LANDSCAPE)           /* rotate a landscape character */ 
    {
      in_left = p[6]*256 + p[7];    /* raw left offset */
      in_top  = p[8]*256 + p[9];    /* raw top offset */

      out_w   = p[12]*256 + p[13];  /* portrait width is ls height */
      out_h   = p[10]*256 + p[11];  /* portrait height is ls width */
      in_bpr  = ( out_h + 7 ) / 8;  /* in bytes per row */
      out_bpr = ( out_w + 7 ) / 8;  /* out bytes per row */
      out_size = out_bpr * out_h;   /* size of output raster in bytes */

#ifdef FDEBUG
fprintf(stderr, "Rotate: in_bpr %d, out_bpr %d ow %d, oh %d, size %d\n",
 in_bpr, out_bpr, out_w, out_h, out_size);
#endif
      out_buf = get_buffer(out_size, &aux_buffer);
      for(i=0;i<out_size;i++) out_buf[i] = 0;
      in_line = &p[in_size - in_bpr];   /* last ls raster row */
      in_copy = in_line;                /* copy that */
      out_copy = out_buf;
      out_line = out_copy;
      out_mask = 0x80;

/* 
 * Character pattern rotation: the landscape pattern is the portrait pattern
 * rotated by +90 degrees. To rotate the pattern back (so the Canon can flip 
 * it into landscape all over again) :
 *
 * Scan all landscape raster lines from bottom to top;          
 *  ( the lowest landscape pattern line is the first column of portrait data )
 *   Scan all bytes in that row  ('move left to right' in that pattern line) 
 *     Check every bit:          ('hi order downto low order')
 *       If a bit is set, set the corresponding bit in the portrait data byte 
 *       ( portrait data is set up to down, left to right )
 */

      for(ls_lines = 0; ls_lines < out_w; ls_lines++) 
	{
	  out_line = out_copy;
	  for(i=0;i<in_bpr;i++)
	    {
	      in_mask = 0x80;
	      in_bits = out_h - (i * 8);
              if (in_bits > 8)
		in_bits = 8;
	      for(j=0;j<in_bits;j++) 
		{
		  if((in_line[i] & in_mask) != 0) 
		    (*out_line) |= out_mask;
		  out_line += out_bpr;
		  in_mask >>= 1;
		}
	    }
          in_copy -= in_bpr;
	  in_line = in_copy;
	  out_mask >>= 1;
	  if( out_mask == 0)
	    {
	      out_mask = 0x80;
	      out_copy++;
	      out_line = out_copy;
	    }
	}
      out_top = - in_left;              /* new top offset */
      out_left =  in_top - out_w;       /* new left offset */
    }
  else
    { /* portrait characters */
      out_w    = p[10]*256 + p[11];     /* pattern width */
      out_h    = p[12]*256 + p[13];     /* pattern height */
      out_left = p[6]*256  + p[7];      /* left offset */
      out_top  = p[8]*256  + p[9];      /* top offset */
      out_size = in_size -16;           /* subtract header info size */
  }

#ifdef FDEBUG
fprintf(stderr,"..HP: char %d, left %d, top %d, ",char_code, out_left, out_top); 
#endif
  /* convert PCL values to CaPSL values */

  out_top = out_top - out_h;
  out_top = - out_top;

  dummy = FindFont( pcl_font_id );   
  if (char_code >127){
    if (c_dl_sec == -1) {             /* now we need a new CaPSL font */
      f = get_handle(pcl_font_id);    /* get primary font descriptor */
      if(f == NULL) {                 /* trouble... */
	MorePanic(UN_OTHER,"Fatal: Missing PCL font descriptor %d",pcl_font_id);
	exit(1); /* this really is bad - should not happen, to coin a phrase */
      }
      c_dl_sec = c_font_id;           /* add secondary id */
      f->c_id_s = c_dl_sec;        
      create_canon_font(c_dl_sec,f);  /* and create font */
    }
    Assign = c_dl_sec; 
    char_code -= 128;
  } else {
    Assign = c_dl_prim;
  }
  /* Assign = Canon assign number */
  if(font_type==8)
    {
      if( out_left < 0 )       /* Canon doesn't allow negative offsets */
	out_left = 0;          /* in fixed pitch characters */
      csize = out_size + 9;    /* Fixed Pitch character header size */
    }
  else
    {
      csize = out_size + 11;   /* Proportional character header size */
    }

#ifdef FDEBUG
  fprintf(stderr,"..dl char %d, pwidth %d, pheight %d, left %d, top %d, pitch %ld\n",char_code, out_w, out_h, out_left, out_top, pitch  );
#endif

  PutString("\033[%d;%d;%d.q", csize, Assign, font_type);
  put_next_char(char_code);
  if(font_type == 9)
    PutWord(pitch);
  PutWord(out_w);
  PutWord(out_left);
  PutWord(out_h);
  PutWord(out_top);
  if(orient == LANDSCAPE)
    pipe_write(out_buf,out_size);
  else
    pipe_write( &p[16],out_size);    
}
 
 
#ifdef FDEBUG

void fnt_hdr(  BYTE *p )
/* print out HP font header info */
{
  UINT CHeight, BLine, CWidth, Orient, SpFlag, pitch, CSize;
  UINT Style, Stroke, TFace, Bitsize;

  CHeight  = p[10]*256 + p[11];            /* cell height */  
  BLine    = p[6]*256 + p[7];              /* Baseline position */   
  CWidth   = p[8]*256 + p[9];              /* cell width  */
  Orient   = p[12];
  SpFlag   = p[13];                    /* spacing Flag */
  pitch    = p[16]*256 + p[17];
  CSize    = p[18]*256 + p[19];
  Style    = p[23];
  Stroke   = p[24];
  TFace    = p[25];
  Bitsize  = p[3];
  fprintf(stderr, "Fnthdr(HP): Size %d, Cellh %d, Cellw %d, pitch %d, Csize %d, BaseLine %d, Orient %d\n",
           Bitsize, CHeight,CWidth,pitch,CSize,BLine,Orient);
}


#endif




/* 
 * Create Font Descriptor of <size> bytes - ESC )s # W 
 *
 * Apparently not all PCL 8-bit fonts use the code positions >127. 
 * I assume those sets are defined as PC8 sets to enable character 
 * printing in control code areas.
 * Therefore the PCL 7- and 8- bit font types generate one CaPSL
 * 7-bit font. An additional 7-bit CaPSL font is created only when
 * necessary, i.e.  when a PCL 8-bit font actually uses 8 bits.  
 *
 * The font is created in the orientation given by the PCL orientation
 * flag, or in the current page orientation in case of disagreement.
 * 
 */

void CreateFont ( int size )
{  
  long temp;
  int  cs;
  Fptr q;
  BYTE *p;

  if (c_font_used >= MAX_FONTS) {
    MorePanic(UN_OTHER, "Warning: Maximum CaPSL fonts per page reached");
  }

  p = get_buffer(size, &buffer);
  pipe_read(p, size);       /* read header */ 

/*   
  if(p[2] != 0)
    MorePanic(UN_OTHER,"Warning: Download font is not a LJ font");
*/

#ifdef FDEBUG
  fnt_hdr(p);
#endif  

  q = new_descriptor();  
  q->pcl_id     = pcl_font_id;
  q->pcl_temp   = TEMPORARY;                         /* default: temp */
  q->c_cwidth   = p[8]*256 + p[9];                   /* cell width  */
  q->c_orient   = p[12];                             /* orientation */
  q->pcl_type   = p[3];                              /* bit size */
  q->c_cheight  = p[10]*256 + p[11];                 /* cell height */  
  q->c_bline    = q->c_cheight - (p[6]*256 + p[7]);  /* Baseline position */   

/*
 * The CaPSL 'Orient' field is used when selecting char sets by
 * attribute. If PCL dl fonts are always selected by some
 * preassigned font id, then the corresponding CaPSL font will
 * always be found. There may be problems if a PCL dl font is
 * selected by attribute: depending on whether 'Orient' was set
 * correctly the CaPSL dl font may or may not be found.  
 * Dvi2xx for example downloads fonts for landscape printing in 
 * portrait format. I suppose downloading landscape data is obsolete
*/

  if (landscape == TRUE)                  /* page is currently lscaped... */ 
    q->c_orient = LANDSCAPE;

  q->c_flag   = p[13] + 8;                /* spacing Flag */
  q->c_gs     = 3840;                     /* FIXME */
  temp        = p[16]*256 + p[17];
  q->c_pitch  = (UINT) (DOT2PTS / temp);
  temp        = p[18]*256 + p[19];
  q->c_csize  = (UINT)  ((25 * temp) / 4.1666667); /* dots to points */
  q->c_style  = p[23];
     cs       = p[24];    /* Strokeweight */
  if(cs < 0)
    {
      q->c_stroke = 20 - cs;
    }
  else if (cs > 0)
    {
      q->c_stroke = cs + 10;
    }
  else q->c_stroke = 0;
  
  q->c_tface  = p[25];
  font_type   = q->c_flag;     /* fixed or prop., don't confuse with FType */
  q->c_id_p   = c_font_id;
  q->c_id_s   = -1;            /* no secondary font (yet) */
  create_canon_font(c_font_id, q);
}









