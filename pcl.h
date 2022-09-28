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
 * pcl.h - PCL defines
 */


/*
 * PCL symbol set numbers,  number= value*32 + terminator-64 
 */

/* 7-bit sets */
#define HP_NORWEG      4   /* ISO 60 Norwegian 0D */
#define HP_ITAL        9   /* ISO 15 Italian   0I */ 
#define HP_UKENG      37   /* ISO 4 UK         1E */
#define HP_FRENCH     38   /* ISO 69 French    1F */
#define HP_GERMAN     39   /* ISO 21 German    1G */
#define HP_SWED       19   /* ISO 11 Swedish   0S */
#define HP_ASCII      21   /* ISO 6 ASCII      0U */
#define HP_SPANISH    83   /* ISO 17 Spanish   2S */
#define HP_LEGLSET    53   /* HP Legal         1U */

/* 8-bit sets              * = equiv. CaPSL sets exist */
#define HP_ROMAN     277   /*   Roman-8               8U */
#define HP_DESKTOP   234   /*   Desktop               7J */
#define HP_WIN30     309   /*   Windows 3.0 Latin 1   9U */ 
#define HP_WIN31     629   /*   Windows 3.1 Latin 1  19U */
#define HP_ISOLATIN1  14   /*   ISO 8859-1 Latin 1    0N */ 
#define HP_PC_8      341   /* * PC-8 CP 437          10U */ 
#define HP_PCDN      373   /* * PC Dan/Nor           11U */
#define HP_PC850     405   /* * PC-850               12U */
#define HP_PC_TRK    308   /* * PC Turkish            9T */
#define HP_PSTEXT    330   /* * PS TEXT              10J */
#define HP_MCTEXT    394   /*   MC Text              12J */
#define HP_VMATH     205   /*   Ventura Math          6M */
#define HP_PSMATH    173   /* * PS Math               5M */
#define HP_VINTL     426   /*   Ventura Int'l        13J */
#define HP_VUS       458   /*   Ventura US           14J */


/* 
 * PCL typefaces for which CaPSL equivalents exist
 * PCL 4 typeface numbers 0-5 match CaPSL numbers:
 *  0 Line Printer
 *  1 Pica
 *  2 Elite 
 *  3 Courier 
 *  4 Helvetica   (Swiss)
 *  5 Times Roman (Dutch)
 */

#define HP_COURIER   3     /* default typeface is 3 (Courier) */
#define HP_UNIVERS   4148  /* PCL CG Univers is CaPSL Swiss   */
#define HP_CGTIMES   4101  /* PCL CG Times is CaPSL Dutch     */

/* various other PCL typefaces  */
#define HP_COURIER2  4099  /* PCL5 code for Courier */
#define HP_ALBERTUS  4362  /* Albertus */
#define HP_ANOLIVE   4168  /* Antique Olive */
#define HP_CORONET   4116  /* Coronet */
#define HP_LGOTHIC   4102  /* Letter Gothic */
#define HP_WDINGS    6826  /* Intellifont Wingdings */


/* PCL Page Sizes */
#define HP_A4     26
#define HP_LETTER 2
#define HP_EXEC   1
#define HP_LEGAL  3


/* defines for PCL relative positioning */
#define NONE      0 
#define POSITIVE  1
#define NEGATIVE  2


/* PCL Font Header - Font Type field */
#define SEVENBIT  0 
#define EIGHTBIT  1
#define IBMFONT   2 


/* defines for font / macro registration modes */
#define TEMPORARY      0 
#define PERMANENT      1
#define TEMPERM        2


/* PCL Escape value & command letter struct */

struct PARAM{      
  long   i;        /* parameter value as integer */
  double f;        /* parameter value as real*/
  BOOL   rel;      /* relative coordinate flag */
  int    letter;   /* command letter */
};

typedef struct PARAM param;




















