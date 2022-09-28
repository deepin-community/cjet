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
 *  cjet.h - common defines, typedefs, global vars etc.
 *  includes configuration file "config.h" 
 */

#define VERSION  "0.8.9"

#ifdef MAIN
#define EXTERN 
#else 
#define EXTERN extern
#endif

#ifdef MSDOS
#ifdef BCC 
#include <string.h>
#endif
#endif

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>


typedef short int      WORD;
typedef unsigned int   UINT;
typedef unsigned char  BYTE;
typedef char           BOOL ;



#define UNIMPLEMENTED(a) if(verbose)\
                           fprintf(stderr,"%s not yet implemented.\n",a)

#define get_next_char()     fgetc(infile)
#define unget_char(a)       ungetc(a,infile)

#include "pcl.h"
#include "capsl.h"


/* 
 * printer resolution
 */

#define DPI   300

/* 
 * some control codes common to all(?) civilized printers
 */

#define BEL   0x07        /* the bell */
#define CR    0x0d        /* carriage return */
#define FF    0x0c        /* form feed */
#define LF    0x0a        /* line feed */
#define VTAB  0x0b        /* vertical tab */
#define ESC   0x1b        /* escape character */
#define SPACE 0x20        /* space character */
#define BS    0x08        /* backspace */
#define SI    0x0f        /* shift in */
#define SO    0x0e        /* shift out */
#define HTAB  0x09        /* horizontal tab */

/* various */
#ifndef NULL
#define NULL      0
#endif
#define TRUE      1
#define FALSE     0
#define PRIMARY   0     
#define SECONDARY 1
#define LEFT      0
#define RIGHT     1
#define PORTRAIT  0
#define LANDSCAPE 1


/* homegrown error codes */
#define UN_CLASS  -300  /* unknown PCL command class */
#define UN_CMD    -301  /* unknown PCL command */
#define UN_PARAM  -302  /* mangled PCL parameter string */
#define UN_OTHER  -303  /* some other error */
#define UN_OTHER1 -304
#define UN_NOMEM  -305  /* generic out of memory */
#define UN_WFILE  -306  /* file error on write */
#define UN_RFILE  -307  /* file error on read */



/* 
 * Doubly linked list of download font descriptors. These contain PCL to
 * CaPSL id number assignments and various fields not actually used in a 
 * CaPSL font descriptor, but perhaps in some command later on. 
 */

typedef struct FONT_DESC FontDesc;
typedef FontDesc* Fptr;

struct FONT_DESC {
  int   pcl_id;        /* PCL font ID number */
  BYTE  pcl_type;      /* font size: 7- or 8-bit font  */
  BYTE  pcl_temp;      /* status: temporary / permanent */
  char  pcl_ul_dist;   /* underline dist:  + above, - below baseline */
  BYTE  pcl_ul_height; /* Underline height, default 3 */
  int   c_id_p;        /* font id primary */
  int   c_id_s;        /* font id secondary */
  int   c_orient;      /* orientation */
  int   c_gs;          /* graphic set number */
  int   c_flag;        /* spacing flag */
  int   c_pitch;       /* character pitch */
  int   c_csize;       /* character size */
  int   c_style;       /* character style */
  int   c_stroke;      /* character stroke */
  int   c_tface;       /* typeface */
  int   c_cwidth;      /* Cell width */
  int   c_cheight;     /* Cell height */
  int   c_bline;       /* baseline distance */
  Fptr  fd_next;       /* next descriptor in list */
  Fptr  fd_prev;       /* previous fd */
};

#define DOWNLD          -1    /* flag: indicates download font */

EXTERN BOOL ignore;           /* flag: ignore very small dl fonts */
EXTERN BOOL ignorepaper;      /* flag: ignore paper size setting commands */
EXTERN BOOL verbose;          /* flag: message output yes / no */
EXTERN int  paintmode;        /* CaPSL paint mode: partial / full */
EXTERN FILE *infile;          
EXTERN FILE *outfile;
EXTERN char *prog_name;


/* variables to keep track of PCL & CaPSL printer states */
EXTERN BOOL landscape;          /* current page orientation */
EXTERN int  CR_status;          /* CR / LF status, default 0 */
EXTERN int  left_margin;        /* in DECIPTS, default 0 */
EXTERN int  right_margin;       /* default max right pos on current page */
EXTERN int  top_margin;         /* current top margin in DECIPTS */
EXTERN int  bottom_margin;      /* in DECIPTS, set by text_length */
EXTERN int  last_top;           /* last set top margin in DECIPTS */
EXTERN int  last_bot;           /* last set bottom margin in DECIPTS */
EXTERN int  page_length;        /* current page length in DECIPTS */
EXTERN int  paper_size;         /* paper size as CaPSL ID */
EXTERN int  text_length;        /* current text length in DECIPTS */

EXTERN int  resolution;         /* current graphics resolution in dpi */
EXTERN int  line_spacing;       /* in LPI; default 6lpi */
EXTERN int  line_VMI;           /* current VMI in decipoints */
EXTERN int  print_pitch;        /* in CPI; default 10 cpi */
EXTERN int  print_HMI;          /* current HMI in CPI * 100 */
EXTERN int  char_size;          /* default 120 decipoints */
EXTERN int  shift_state;        /* CaPSL: SI or SO ? */
EXTERN int  pos_unit;           /* movement mode: line/Col or size unit */
EXTERN int  pos_sub_unit;       /* movement submode: dots or decipoints */
EXTERN int  tface_p;            /* current primary typeface */
EXTERN int  tface_s;            /* current secondary typeface */
EXTERN int  set_p;              /* current PCL primary symbol set */
EXTERN int  set_s;              /* current PCL secondary symbol set */
EXTERN int  save_p;             /* saved PCL primary symbol set */
EXTERN int  save_s;             /* saved PCL secondary symbol set */
EXTERN int  macro_id;           /* current macro id */
EXTERN BOOL recording;          /* macro registration in progress */
EXTERN long macro_free;         /* remaining space in macro buffer */
EXTERN int pos_index;           /* current memorized position index */ 
EXTERN int page_y_offset;       /* shift printout in y direction */
EXTERN int page_x_offset;       /* shift printout in x direction */

/* misc buffer stuff (raster & font download) */

#define M_BUFFER_SIZE  16384   /* initial buffer size */
#define M_DEC_SIZE      2048   /* decompression buffer size */
#define M_RASTER_SIZE  16384   /* raster buffer size */
#define M_MACRO_SIZE    2048   /* macro buffer size */

typedef struct BUF_DESC bdesc;

struct BUF_DESC {
  long size;                   /* current size */
  BYTE *p;                     /* where it is */
};

EXTERN bdesc buffer;           /* the 'main' buffer */
EXTERN bdesc aux_buffer;       /* the 'aux' buffer */
EXTERN bdesc dec_buffer;       /* the 'decompression' buffer */
EXTERN bdesc mac_buffer;       /* macro defs here */

EXTERN BYTE  *bufptr;          /* global buffer.p */
EXTERN BYTE  *macroptr;        /* current mac_buffer ptr */


/* Raster Graphics stuff */
EXTERN int old_usize;          /* last raster data line size (bytes) */
EXTERN int r_start;            /* flag: Raster transfer in progress */
EXTERN int r_width;            /* current raster line width (bytes) */
EXTERN int r_lines;            /* current raster line count */
EXTERN int r_mode;             /* flag: PCL begin raster mode */
EXTERN long r_free;            /* free buffer size */
EXTERN int r_compress;         /* Compression Mode */



/* 
 * MAX_FONTS is max. number of fonts which can be printed per page, 
 * which also appears to be the max. number of downloadable fonts on
 * the LBP 8. The CaPSL fonts are 7 bit, so only (MAX_FONTS / 2) 8-bit
 * fonts can be downloaded.
 */

#define MAX_FONTS      32     /* max. fonts per page */

EXTERN Fptr FontList;         /* root of font descriptor list */
EXTERN int  c_font_used;      /* number of used fonts (CaPSL) */
EXTERN int  pcl_font_id;      /* current PCL download Font ID */
EXTERN int  c_font_id;        /* next free CaPSL dl Font ID */
EXTERN int  char_code;        /* current download character code */
EXTERN int  font_type;        /* current dl Fonttype fixed / proport. */
EXTERN int  select_id;        /* current CaPSL DownLoad Font ID */
EXTERN int  c_dl_prim;        /* current CaPSL dl font id - primary */
EXTERN int  c_dl_sec;         /* and secondary */


/* Pattern Stuff */
EXTERN int last_pattern;      /* PCL or grey scale pattern id */
EXTERN int patt_width;
EXTERN int patt_height;      

/* include printer defaults */
#include "config.h"

/* prototypes */
void CheckPJL( void );
void put_next_char( int );
void PutString( char *, ... );
void InitCaPSL( void );
void init_side_margin( void );
void init_top_margin( void );
void cold_init( void );
void terminate( void );
int Parse(param *p);
void MorePanic( int, char* , ... );
void SetModes( int, int );
void SetShiftState(  int );
void CSSelect( int );
void PutWord( int );
void PurgeRaster( int ); 
void clear_decompress_buffer( void);
void kill_descriptors( void );
int  FindFont (int );
BYTE *get_buffer( int, bdesc * );
void pipe_read( BYTE *, int );
void pipe_write( BYTE * , int );
void translate( int );
void ReallyPrint( int );
void paperwork( void );
void paperinit( void );

void ClearSideMargins( void );
void DisplayFunctions( void );
void HalfLineFeed( void );
void Reset( void );
void SelfTest( void );

/* ESC ( */
void Paren( int );
void SetDefaultFont( int, int);
void SetDownLoadFont( int, int);
void SetSymbolSet( int, int );

/* ESC ( s */
void Paren_S( int );
void SetSpacing( int, int );
void SetPitch( int, double );
void SetHeight( int, double );
void SetStyle( int, int );
void SetStroke( int, param *);
void SetTypeface( int, int);
void DownLoadChar( int );
void CreateFont( int ); 

/* ESC ( f */
void Paren_F( int );
void LoadSymbol( int );

/* ESC & */
void Ampersand( void );

/* ESC & a */
void Amp_a(void);
void SetLeftMargin(int);
void SetRightMargin(int);
void MoveHoriz( int, int,  param *);
void MoveVert( int, int, param *);
void logical_page( int );
void logical_rotate( int );

/* ESC & d */
void Amp_d(void);
void SetUnderLineMode( int );
void DisableUnderLine( void );

/* ESC & f */
void Amp_f(void);
void PushPopPos( int );
void MacroControl( int );
void MacroID( int );

/* ESC & k */
void Amp_k(void);
void SetHMI( double );
void SetPrintPitch( int );
void LineTermination( int );

/* ESC & p */
void Amp_p(void);
void TransparentPrint( int );

/* ESC & l */
void Amp_l(void);
void SetPageLength( int );
void SetTopMargin( int ); 
void SetTextLength( int );
void SetLinesPerIn( int);
void SetVMI( double ); 
void SetPageOrient( int );
void PerfSkipMode( int ); 
void NumCopies( int );    
void PaperControl( int );
void SetPageSize( int );  
void paper_tray( int );
void log_position( param *);

/* ESC &r */
void ClearPages(int);

/* ESC & s */
void Amp_s(void);
void SetWrapMode( int );

/* ESC & u */
void Amp_u(void);
void SetUnitsPerInch( int );

/* ESC * */
void Asterisk( void );

/* ESC * t */
void Ast_t(void);
void RasterRez( int );

/* ESC * b */
void Ast_b(void);
void TransferData( int );
void Compression( int );
void y_offset( int );

/* ESC * r */
void Ast_r(void);
void StartRaster( int );
void EndRaster ( void );
void rotate_raster( int);
void raster_height( int );
void raster_width( int );

/* ESC * c */
void Ast_c(void); 
void SetFontID( int );
void SetCharCode( int );
void FontControl( int );
void HorizRuleSize( int, int );
void VertRuleSize( int, int );
void PatternID( int );
void PrintRule( int );
void SetSymbolID( int );
void SymbolControl( int );
void DefPattern( int );
void PatternControl( int );

/* ESC *p */
void Ast_p(void);
void MacroRef( int );

/* ESC *s */
void StatusMessageType(int);
void StatusMessageUnit(int);
void StatusMessage(int);
void FreePrinterMemory(int);
void EchoValue(int value);

/* ESC *v */
void Ast_v( void);
void TPattern( int );
void TSourceMode( int );
void TDestMode( int );

/* ESC % */
void percent( void );
void UnivLangExit( int );



















