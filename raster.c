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
 *
 * raster.c  -  raster graphics stuff 
 *  
 * rotate raster                   - ESC *r # F
 * Set Raster pattern height       - ESC *r # T
 * set raster width                - ESC *r # S
 * raster y offset in raster lines - ESC *b # Y
 * Set Compression mode            - ESC *b # M 
 * set Raster Resolution           - ESC *r # R 
 * Transfer Raster Graphics        - ESC *b # W [data] 
 * Start Raster Graphics           - ESC *r # A 
 * End Raster Graphics             - ESC *r B  
 * End Raster Graphics             - ESC *r C  
 * Horizontal Rule Size            - ESC *c # H
 * Vertical Rule Size              - ESC *c # B 
 * Specify Pattern ID              - ESC *c # G 
 * Print Rule / Pattern            - ESC *c # P
 *
 * UNIMPLEMENTED("Rotate raster method 3")        - ESC *r F
 * UNIMPLEMENTED("Set raster height")             - ESC *r T
 * UNIMPLEMENTED("Set Raster width")              - ESC *r S
 * UNIMPLEMENTED("User defined pattern");         - ESC *c W
 * UNIMPLEMENTED("Pattern control");              - ESC *c Q
 * UNIMPLEMENTED("Transparency pattern");         - ESC *v T
 * UNIMPLEMENTED("Transparency source mode");     - ESC *v N
 * UNIMPLEMENTED("Transparency destination mode");- ESC *v O
 */

#include <string.h>
#include "cjet.h"

/*#define RDEBUG */

/* 
 * clear the decompression buffer;
 */

void clear_decompress_buffer()
{
  BYTE *p;
  p = get_buffer(0, &dec_buffer); 
  memset(p, 0, dec_buffer.size);
  old_usize = 0;
}



/* 
 * rotate raster - ESC *r # F
 * method 0 : rotate portrait data for landscape printing
 * method 3 : old PCL landscape data format
 */

void 
rotate_raster(int method)
{
  switch(method){
  case 0 : break; /* CaPSL rotates automatically */
  case 3 : UNIMPLEMENTED("Rotate raster method 3"); break;
  }
}


/*
 * Set Raster pattern height - ESC *r # T
 * 
 */

void 
raster_height( int height)
{
  UNIMPLEMENTED("Set raster height");
}


/* 
 * set raster width - ESC *r # S
 */

void 
raster_width( int width)
{
  UNIMPLEMENTED("Set Raster width");
}


/* 
 * raster y offset in raster lines - ESC *b # Y
 * print pending raster data and move CAP by #lines downwards
 * FIXME for different transparency modes ?
 */

void y_offset( int lines)
{
#ifdef RDEBUG
  fprintf(stderr,"Y Offset %d lines\n",lines);
#endif
  PurgeRaster(0);
  if(lines > 0) {
    SetModes(SIZEUNIT,DOTS);
    PutString("\033[%de",lines); 
  }
  clear_decompress_buffer();
}



/* 
 * Set Compression mode - ESC *b # M 
 * 0...no compression
 * 1...run length encoding
 * 2...TIFF encoding
 * 3...delta row compression
 */
 
void
Compression( int mode )
{ 
#ifdef RDEBUG
  fprintf(stderr,"Compression mode set from %d to %d\n",r_compress, mode);
#endif

  r_compress = mode;

  if (mode > 3) 
    MorePanic(UN_OTHER,"Unsupported data compression mode %d",mode);
  /* FIXME: now what ? */
}


/*
 * set Raster Resolution - ESC *r # R
 * value res is 75, 100, 150, or 300 dpi. In the CaPSL scheme of things, 
 * the raster resolution is sent along with the actual graphics command. 
 */

void 
RasterRez(int res)
{
  resolution = res;
}


/* 
 * dump raster graphics collected in the buffer to the printer;
 * move printer CAP past last line;
 * 
 */
void 
PurgeRaster( int size )
{
  long count;

  count = buffer.size - r_free;

#ifdef RDEBUG
  fprintf(stderr,"PurgeRaster() %ld count, %d lines, %d oldsize, %d size\n",count, r_lines, old_usize, size);
#endif

  if(count > 0) {
    PutString("\033[%ld;%d;%d.r", count, r_width, resolution);
    pipe_write(buffer.p, count);
  }
  r_free  = buffer.size;
  bufptr  = buffer.p;
  r_width = size;
  if(r_lines > 0) {
    SetModes(SIZEUNIT,DOTS);
    PutString("\033[%de",r_lines); /* move down */  
  }
  r_lines = 0;
}




/* 
 * Transfer_ModeX: 
 * - reads a mode X compressed raster data row into dec_buffer
 * - returns uncompressed length 
 * - size is always > 0 except for mode 3
 *
 */

int Transfer_Mode0( int size )
{
  pipe_read(dec_buffer.p, size);
  return(size);
}


int Transfer_Mode1( int size )
{
  BYTE *dec;
  int usize, count, i, j, db, rep;

  usize = 0;
  dec   = dec_buffer.p;
  count = size / 2;                 /* this had better be even */
  for(i=0;i<count;i++) {
    rep = get_next_char();          /* get repeat count */
    db  = get_next_char();          /* get data count */
    *dec++ = db;                    /* write data */
    usize++;
    for(j=0;j<rep;j++) {            /* now repeat data byte */
      *dec++ = db;
      usize++;
    }
  }
  return (usize);
}


int Transfer_Mode2( int size )
{
  BYTE *dec;
  int usize, count, j, db, rep;

  usize = 0;
  dec   = dec_buffer.p;
  count = size;
  do {
    rep = get_next_char(); count--;    /* get mode byte */
    if(rep != 128) {                   /* mode != ignore ? */
      db = get_next_char(); count--;   /* get data byte */
      *dec++ = db;                     /* and write */
      usize++;
      if(rep > 128) {                  /* determine sub mode */
	rep = 256 - rep;               /* sub mode 1: real rep count */
	for(j=0;j<rep;j++) {           /* replicate db */
	  *dec++ = db;
	  usize++;
	}
      } else {      /* sub mode 0 : transfer next rep bytes literally */
	for(j=0;j<rep;j++) {
	  *dec++ = get_next_char();
	  count--;
	  usize++;
	}
      }
    }
  } while(count>0);
  return (usize);
}


int Transfer_Mode3( int size )
{
  BYTE *dec;
  int usize, count, i, j, db, rep;

  usize = 0;
  dec   = dec_buffer.p;
  if(size > 0) {
    count = size;
    while (count > 0) {
      db    = get_next_char(); count--;   /* get command byte */
      rep   = (db >> 5) + 1;              /* replace count */
      i     = db & 0x1f;                  /* offset */
      dec   += i;                         /* move pointer */
      usize += i;                         /* keep track of length */
      if( i == 31 )
	do {
	  i = get_next_char(); count--;
	  dec += i;
	  usize += i;
	} while( i == 255 );
      for(j=0;j<rep;j++) {
	*dec++ = get_next_char();
	usize++;
	count--;
      }
    }
    /* if previous row was longer: use old length to avoid lossage */
    if( usize < old_usize )
      usize = old_usize;
  } else  /* size was 0 -> copy previous row  */
    usize = old_usize;
  return(usize);
}


/* 
 * Transfer Raster Graphics ESC *b # W [data] 
 * PCL raster data rows are first read/decompressed into dec_buffer.
 * Then the resulting rows with the same width are collected into 
 * CaPSL raster data chunks.
 * For mode 3 (delta row) compression, the previous raster row (regardless
 * of any previous compression mode) is still in dec_buffer[] and can be
 * used as the seed row for the current row.
 *
 */

void
TransferData( int size )
{
  BYTE *dec;
  int newsize, i; 

  newsize = 0;
  
  if(size == 0) {            /* check for 0 raster data size */
    if (r_compress != 3) {   /* for modes 0..2 move down */
      y_offset(1);          
      return;
    } 
    if ((r_compress == 3) && (old_usize == 0)) { /* move down for mode 3 */
      y_offset(1);                               /* if seed row is empty */
      return;
    }
  }
  
  switch (r_compress) 
    { 
    case 0:  /* no compression */
      newsize = Transfer_Mode0(size);
      break;
      
    case 1:  /* run length encoding */
      newsize = Transfer_Mode1(size);
      break;
      /* mode 1 */
      
    case 2:  /* TIFF mode */
      newsize = Transfer_Mode2(size);
      break;
      /* case 2 */
      
    case 3:  /* delta row compression */
      newsize = Transfer_Mode3(size);
      break;
      /* case 3 */
    }
  
  /* 
   * modes 0-2: if the new line is shorter: zero previous size of buffer
   * mode 3: newsize is always >= old_usize: no effect
   */
  for(i = newsize;i < old_usize; i++) dec_buffer.p[i] = 0;
  
  /* 
   * zero strip
   */
  
  dec = dec_buffer.p + newsize -1;                     
  while ((*dec-- == 0) && (dec != dec_buffer.p)) newsize--; 
  
  /* 
   * if r_width is 0 => init: set r_width to newsize
   */
  if (r_width == 0) r_width = newsize;
  
  /* if buffer is full or new raster data has a different width, purge */
  if((newsize > r_free) || (newsize != r_width))
    PurgeRaster( newsize );

  /* add new data to buffer: row size is the same or new */
  dec = dec_buffer.p;
  for(i=0;i<newsize;i++) *bufptr++ = *dec++;
  old_usize = newsize;	
  r_free -= newsize;
#ifdef RDEBUG
  fprintf(stderr, "Adding %d bytes to buffer. Free = %ld\n", newsize, r_free);
#endif
  r_lines++;
}



/* 
 * Start Raster Graphics - ESC *r # A 
 * mode = 0 : start at left margin
 * mode = 1 : start at current cursor position.
 */

void StartRaster( int mode )
{ 
  bufptr = get_buffer(M_RASTER_SIZE, &buffer);  /* get main buffer */
  clear_decompress_buffer();
  r_free = buffer.size; 
  r_width = 0;
  r_lines = 0;
  r_start = TRUE;               /* OK, Raster Graphics in progress... */
  r_mode = mode;
  
  if (mode == 0) {
    /* save current horizontal position */
    PutString("\033[1;%d;1x", RASTERINDEX);

    /* recall current left margin and move there */
    PutString("\033[0;%d;1x", S_CURRENTLEFT);
  }
  /* else do nothing.  */
}


/* 
 * End Raster Graphics - ESC *r B  
 * Newer PCL Versions  - ESC *r C
 */

void EndRaster( void)
{
  if(r_start == TRUE) {
    r_start = FALSE;
    PurgeRaster(0);      /* clear out any remaining print data */
    if(r_mode == 0) {
      /* recall previous horizontal position */
      PutString("\033[0;%d;1x", RASTERINDEX);
      /* new position is one dot line below last raster line */
    }
  }
}


/*
 * Horizontal Rule Size - ESC *c # H
 * convert from decipoints to dots
 */

void HorizRuleSize( int mode, int size )
{
  if(mode == DECIPTS) 
   size = (int) ((size / 2.4)+1);
  patt_width = size;
}


/*
 * Vertical Rule Size - ESC *c # B 
 * convert from decipoints to dots
 */

void VertRuleSize( int mode, int size )
{
  if(mode == DECIPTS) 
   size = (int) ((size / 2.4)+1);
  patt_height = size - 1;
}


/*
 * Specify Pattern ID - ESC *c # G 
 */

void PatternID( int pat )
{
  last_pattern = pat;
}


/*
 * match CaPSL grey scale patterns to PCL values;
 * 'last_pattern' should contain PCL grey scale value 
 */

int greyscale( void )
{ 
  if(last_pattern < 3)  return(0);
  if(last_pattern < 11) return(1);
  if(last_pattern < 21) return(2);
  if(last_pattern < 36) return(4);
  if(last_pattern < 56) return(5);
  if(last_pattern < 81) return(6);
  if(last_pattern < 100) return(7);
  return(3);
}


/*
 * match CaPSL defined fill patterns to PCL patterns;
 * 'last_pattern' should contain PCL pattern number
 * 's' points to a mode string defining CaPSL pattern mods (rotation etc)
 */

int hppattern( char *s)
{
  int pat;

  switch(last_pattern){
  case 1 : { pat = 17; strcpy(s,"001"); break; }
  case 2 :   pat = 17; break;
  case 3 :   pat = 19; break;
  case 4 : { pat = 19; strcpy(s,"010"); break; } 
  case 5 :   pat = 25; break;
  case 6 :   pat = 27; break;
    default : pat = 64; break; 
  }
  return(pat);
}


/* 
 * Print Rule / Pattern - ESC *c # P
 * Sequence: set dot mode;
 *           Save current position; 
 *           Begin Box Shading;
 *           move to new position 
 *           End Box Shading; 
 *           Return to saved position
 */

void PrintRule( int op )
{ 
  int cpat;
  char cmode[4];
  
  strcpy(cmode,"000");   /* CaPSL 'pattern' condition thing */  
  SetModes(SIZEUNIT,DOTS);
  /* save current position */
  PutString("\033[1;%d;0x", PATTERNINDEX);
  switch(op) {
    case 0 : cpat = 3; break;                 /* black rule */
    case 2 : cpat = greyscale(); break;       /* grey scale pattern */ 
    case 3 : cpat = hppattern(cmode); break;  /* other patterns */
      default : cpat = 3; break;
    }
  PutString("\033[%d;9;%ss", cpat, cmode);    /* Begin box shading */
  PutString("\033[%da\033[%de", patt_width, patt_height); /* move */
  PutString("\033[9r");                       /* End Box Shading */
  /* recall stored position and move CAP there */
  PutString("\033[0;%d;0x", PATTERNINDEX);
}


void
DefPattern( int count )
{
  UNIMPLEMENTED("User defined pattern");
}

void
PatternControl( int op )
{
  UNIMPLEMENTED("Pattern control");
}


void
TPattern( int pat )
{
  UNIMPLEMENTED("Transparency pattern");
}

void
TSourceMode( int mode )
{
  UNIMPLEMENTED("Transparency source mode");
}

void
TDestMode( int mode )
{
  UNIMPLEMENTED("Transparency destination mode");
}

