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
 * symbol.h : PCL -> CaPSL graphic set tables
 *
 * X_table[] entry value:    prints a char from:
 *
 *    0x00XX         :        C_IBM850 
 *    0x00XX + TPSR  :        C_PSR 
 *    etc.
 *
 *  example: N tilde:
 *  is Roman-8 char 182 -> roman_table[182-160] = 165 C_IBM850 
 *
 *  TSPECIAL flags nonexistent characters built from existing ones
 */

#define TPSR     0x0100
#define TIBMR1   0x0200
#define TIBMR2   0x0400
#define TSYML    0x0800
#define TSYMR    0x1000
#define TSPECIAL 0x2000


/* 
 * some characters do not exist in CaPSL sets; these are "built" from
 * available characters
 */

#define HACEK_SMALL_S      TSPECIAL + 1
#define HACEK_CAP_S        TSPECIAL + 2
#define DIAERESIS_CAP_Y    TSPECIAL + 3
#define SINGLE_QUOTE_OPEN  TSPECIAL + 4
#define SINGLE_QUOTE_CLOSE TSPECIAL + 5
#define TRADEMARK          TSPECIAL + 6
#define CEE_OH             TSPECIAL + 7  /* c/o character */
#define IGREK_SMALL        TSPECIAL + 8  /* ij  ligature */
#define IGREK_CAP          TSPECIAL + 9  /* IJ  ligature */
#define FILLCIRCLE         TSPECIAL + 10 /* IBML 07 */
#define OPENCIRCLE         TSPECIAL + 11 /* IBML 09 */

/*
 * define some PSR diacritics used to construct nonexistent characters
 */

#define PSR_HACEK          207
#define PSR_DIAERESIS      200


/*
 * HP Roman-8:
 * Table has 128 entries; the upper 96 characters are the actual 
 * Roman-8 characters,
 */

WORD roman_table[128] = 
{ 
 255,      255,      255,      255,      255,      255,      255,      255,
 255,      255,      255,      255,      255,      255,      255,      255,
 255,      255,      255,      255,      255,      255,      255,      255,
 255,      255,      255,      255,      255,      255,      255,      255,
 255,      183,      182,      212,      210,      211,      215,      216,
 TPSR+194, TPSR+193, TPSR+195, 249,      TPSR+196, 235,      234,      156,
 TPSR+197, 237,      236,      248,      128,      135,      165,      164,
 173,      168,      207,      156,      190,      245,      159,      189,
 131,      136,      147,      150,      160,      130,      162,      163,
 133,      138,      149,      151,      132,      137,      148,      129,
 143,      140,      157,      146,      134,      161,      155,      145,
 142,      141,      153,      154,      144,      139,      TPSR+251, 226,
 181,      199,      198,      209,      208,      214,      222,      224,
 227,      229,      228,      HACEK_CAP_S,
 HACEK_SMALL_S,      233,      DIAERESIS_CAP_Y,    152,
 232,      231,      250,      230,      244,      243,      TPSR+208, 172,
 171,      166,      167,      174,      254,      175,      241,      255
};


/*
 * ISO 8859-1 (ECMA-94) Latin 1:
 * Windoze 3.0 Latin 1:
 * Windoze 3.1 Latin 1:
 * 
 * (Almost) all ISO characters are available in code page 850
 * Exceptions: 
 *   character 223  ( German sharp s)  replaced by PSR 251
 *   (CaPSL PC850 character 225 is a Greek Beta)
 */

WORD latin_table[128] =
{
  255,       255,               TPSR+184,           159,
  TPSR+185,  TPSR+188,          TPSR+178,           TPSR+179,
  TPSR+195,  TPSR+189,          HACEK_CAP_S,        TPSR+172,
  TPSR+234,  255,               255,                255,
  255,       SINGLE_QUOTE_OPEN, SINGLE_QUOTE_CLOSE, TPSR+170,
  TPSR+186,  TPSR+183,          TPSR+177,           TPSR+208,
  TPSR+196,  TRADEMARK,         HACEK_SMALL_S,      TPSR+173,
  TPSR+250,  255,               255,                DIAERESIS_CAP_Y,
  255, 173, 189, 156, 207, 190, 221, 245, 
  249, 184, 166, 174, 170, 240, 169, TPSR+197,
  248, 241, 253, 252, 239, 230, 244, 250,
  247, 251, 167, 175, 172, 171, 243, 168,
  183, 181, 182, 199, 142, 143, 146, 128, 
  212, 144, 210, 211, 222, 214, 215, 216,
  209, 165, 227, 224, 226, 229, 153, 158, 
  157, 235, 233, 234, 154, 237, 232, TPSR+251,
  133, 160, 131, 198, 132, 134, 145, 135,
  138, 130, 136, 137, 141, 161, 140, 139,
  208, 164, 149, 162, 147, 228, 148, 246,
  155, 151, 163, 150, 129, 236, 231, 152
};



/*
 * Desktop character set
 * 128-159 blank spaces, actual character set is from 160 to 255
 */

WORD desktop_table[128] = {
  255, 255, 255, 255,  255, 255, 255, 255,
  255, 255, 255, 255,  255, 255, 255, 255,
  255, 255, 255, 255,  255, 255, 255, 255,
  255, 255, 255, 255,  255, 255, 255, 255,
  255,        244,         245,          TPSR+178, 
  TPSR+179,   184,         169,          TRADEMARK, 
  CEE_OH,     189,         240,          196,   
  TPSR+188,   TPSR+174,    TPSR+175,     255, 
  TPSR+170,   TPSR+186,    230,          TPSR+189,
  TPSR+183,   FILLCIRCLE,  OPENCIRCLE,   OPENCIRCLE,
  254,        254,         30,           31,
  TPSR+169,   170,         221,          242,
  TPSR+177,   241,         158,          246,  
  248,        TSYMR+162,   TSYMR+178,    172,
  171,        243,         251,          253,
  252,        TPSR+164,    255,          255, 
  TPSR+172,   TPSR+173,    174,          175,
  TPSR+184,   TPSR+185,    250,          173,
  168,        TIBMR1+158,  TIBMR2+171,   156,      
  190,        207,         159,          TPSR+251,
  166,        167,         145,          146,
  208,        209,         IGREK_SMALL,  IGREK_CAP,
  TPSR+248,   TPSR+232,    TPSR+250,     TPSR+234,
  155,        157,         231,          232,
  TPSR+194,   TPSR+193,    TPSR+195,     249,
  TPSR+196,   TPSR+207,    TPSR+198,     TPSR+205,
  TPSR+202,   TPSR+199,    TPSR+197,     TPSR+203,
  TPSR+206,   250,         TPSR+245,     255
};


/*
 * Ventura International
 * spaces from 128 - 159
 * actual characters 160 - 255
 */

WORD v_intl_table[128] = {
  255, 255, 255, 255,  255, 255, 255, 255,
  255, 255, 255, 255,  255, 255, 255, 255,
  255, 255, 255, 255,  255, 255, 255, 255,
  255, 255, 255, 255,  255, 255, 255, 255,
  TPSR+185,       183,       182,             212,
  210,            211,       215,             216,
  184,            169,       TRADEMARK,       TPSR+172,
  TPSR+173,       235,       234,             255, 
  TPSR+189,       TPSR+170,  TPSR+186,        248,
  128,            135,       165,             164,
  173,            168,       207,             156,
  190,            245,       159,             189,
  131,            136,       147,             150,
  160,            130,       162,             163, 
  133,            138,       149,             151,
  132,            137,       148,             129, 
  143,            140,       157,             146, 
  134,            161,       155,             145,
  142,            141,       153,             154,
  144,            139,       TPSR+251,        226, 
  181,            199,       198,             255, 
  255,            214,       222,             224,
  227,            229,       228,             HACEK_CAP_S,
  HACEK_SMALL_S,  233,       DIAERESIS_CAP_Y,  152,
  TPSR+234,       TPSR+250,  244,             TPSR+178,
  TPSR+179,       TPSR+208,  TPSR+177,        255,
  255,            166,       167,             174,
  TPSR+183,       175,       255,             TPSR+188
};



/*
 * Ventura US: 
 */

WORD vus_table[128] = {
  255,      255,      255,     255,      255,     255,     255,     255,
  255,      255,      255,     255,      255,     255,     255,     255,
  255,      255,      255,     255,      255,     255,     255,     255,
  255,      255,      255,     255,      255,     255,     255,     255,
TPSR+185,   255,      255,     255,      255,     255,     255,     255,
  184,      169,   TRADEMARK,  255,      255,     255,     255,     255,
TPSR+189, TPSR+170, TPSR+186,  248,      255,     255,     255,     255,
  255,      255,      255,     255,      255,     245,     255,     189,
  255,      255,      255,     255,      255,     255,     255,     255,
  255,      255,      255,     255,      255,     255,     255,     255,
  255,      255,      255,     255,      255,     255,     255,     255,
  255,      255,      255,     255,      255,     255,     255,     255,
  255,      255,      255,     255,      255,     255,     255,     255,
  255,      255,      255,     255,      255,     255,     255,     255,
  255,      255,      244,  TPSR+178, TPSR+179, TPSR+208,TPSR+177,  255,
  255,      255,      255,     255,   TPSR+183,   255,     255,   TPSR+188,
};


/* HP Legal : no table */


/* 
 * MC Text 
 * 128 characters
 */

WORD mctext_table[128] = {
   142,      143,      128,      144,      165,     153,      154,      160, 
   133,      131,      132,      198,      134,     135,      130,      138,
   136,      137,      161,      141,      140,     139,      164,      162, 
   149,      147,      148,      228,      163,     151,      150,      129,
 TPSR+178,   248,      189,      156,      245,   TPSR+183,   244,   TPSR+251,
   169,      184,   TRADEMARK,   239,      249,   TSYMR+185,  146,      157,
 TSYMR+165,  241,  TIBMR1+243, TIBMR1+242, 190,     230,  TSYMR+182, TSYMR+229,
 TSYMR+213, TIBMR1+227, TSYMR+242, 166,    167,   TIBMR1+234, 145,      155, 
   168,      173,      170,  TIBMR1+251,   159,   TIBMR1+247, TSYML+68, 174,
   175,     TPSR+188,  255,      183,      199,     229,    TPSR+234, TPSR+250,
 TPSR+177,  TPSR+208, TPSR+170, TPSR+186, 
 SINGLE_QUOTE_OPEN, SINGLE_QUOTE_CLOSE, 246, TSYMR+224, 
   152,   DIAERESIS_CAP_Y, TPSR+164, 207, 
 TPSR+172,  TPSR+173, TPSR+174, TPSR+175,
 TPSR+179,   250,   TPSR+184,  TPSR+185, TPSR+189,  182,      210,      181, 
   211,      212,      214,      215,      216,     222,      224,      226, 
   255,      227,      233,      234,      235,     213,    TPSR+195, TPSR+196,
 TPSR+197, TPSR+198, TPSR+199, TPSR+202,   247,   TPSR+205, TPSR+206, TPSR+207
};


/*
 * Ventura Math : 128 entries
 * Ventura Math characters 160 - 255
 * table entries are from C_SYMR i.e. PS_MATH!
 */

WORD vmath_table[128] = {
  160, 160, 160, 160, 160, 160, 160, 160,
  160, 160, 160, 160, 160, 160, 160, 160,
  160, 160, 160, 160, 160, 160, 160, 160,
  160, 160, 160, 160, 160, 160, 160, 160,
  224, 214, 232, 233, 234, 235, 238, 231,
  221, 222, 223, 203, 204, 245, 244, 253,
  226, 201, 202, 230, 160, 167, 197, 196,
  205, 200, 190, 188, 189, 217, 191, 187,
  163, 168, 179, 182, 192, 162, 194, 195,
  165, 170, 181, 183, 164, 169, 180, 161,
  175, 172, 210, 178, 166, 193, 211, 177,
  174, 173, 185, 186, 176, 171, 249, 242,
  247, 215, 208, 248, 240, 237, 236, 241,
  255, 216, 209, 243, 244, 252, 254, 184,
  213, 212, 220, 219, 218, 229, 228, 250, 
  251, 198, 199, 206, 227, 207, 246, 225
};
