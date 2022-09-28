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
 *  main.c  -  main loop
 *
 */


#define MAIN
#include "cjet.h"


#ifdef MSDOS
#include <fcntl.h>
#include <io.h>
#ifdef BCC              /* for Borland C 3.1 */
#define fileno(f)       ((f)->fd) 
#endif
#endif


void usage( char c )
{
  fprintf(stderr,"%s: unrecognized option -- %c\n",prog_name, c);
  fprintf(stderr,"\n  Usage: %s [options] <PCL >CaPSL\n\n",prog_name);
/*  fprintf(stderr,"   Options:\n");*/
  fprintf(stderr,"    -f         full paint mode\n");
  fprintf(stderr,"    -p         ignore paper size commands\n");
  fprintf(stderr,"    -q         quiet mode\n");
  fprintf(stderr,"    -x <d>     shift output horizontally by <d> dots\n");
  fprintf(stderr,"    -y <d>     shift output vertically by <d> dots\n");
  fprintf(stderr,"               (dots are 1/300 inch)\n");
/*  fprintf(stderr,"    -i  replace very small download fonts with internal scaled fonts\n\n");*/
  exit(0);
}
  

/* 
 * get program options:
 * FIXME: use getopt() sometime
 *
 * -p   ignore paper size commands; use this for example if a text is formatted
 *      for letter paper size, but the cassette paper size is A4
 *
 * -q   quiet mode, don't print anything
 *      
 * -i   ignore download fonts with point size < 8,
 *      substitute internal scaled font (not implemented yet)
 *
 * -f   full paint mode for vector graphics; this requires >= 1.5 MB
 *      printer memory...
 *
 * -y arg  shift output by arg dots in y direction + = up, - = down
 *         values entered as dots, save in DECIPOINTS
 *
 * -x arg  shift output by arg dots in x direction + = right, - = left
 *         values entered as dots, save in DECIPOINTS
 * 
 */
 
void parse_args(int argc, char **argv)
{
  char *s, *o, c = 0;
  int err = 0;
  int argi = 1;

  /* get program name */
  if(*argv[0] != '\0')
    prog_name = argv[0];
  else 
    {
      prog_name = (char *) malloc(5);
      strcpy(prog_name,"CJet");
    }

  while(argi < argc ) {
    s = argv[argi];
    argi++;
    if (*s++ == '-') {
      while(*s != 0) {
	switch(c = (*s++) ) {
	case 'f' : paintmode = 2;      break;  /* full paint mode */
	case 'p' : ignorepaper = TRUE; break;
	case 'q' : verbose = FALSE;    break;
	case 'i' : ignore  = TRUE;     break;
	case 'y' : {
	              o = argv[argi];
		      page_y_offset = (int) (strtol(o, NULL, 10) * 2.4);
		      argi++;
		      break;
		    }
	case 'x' : {
	              o = argv[argi];
		      page_x_offset = (int) (strtol(o, NULL, 10) * 2.4);
		      argi++;
		      break;
		    }
        default  : err++; break; 
	}
      }
    }
  }
  if(verbose) {
    fprintf(stderr,"%s version %s - LaserJet emulation for CaPSL printers\n",
	    prog_name, VERSION);
    if(err > 0)
      usage(c);
  }
}



int main(int argc, char **argv)
{  
  int c;

#ifdef MSDOS
  setmode( ( fileno(stdin)) , O_BINARY);
  setmode( ( fileno(stdout)) , O_BINARY);
#endif

  infile  = stdin;
  outfile = stdout;

  atexit(terminate);
  cold_init();
  parse_args(argc, argv);
  InitCaPSL();

  while (( c = get_next_char())!= EOF) {
    switch(c) {
    case ESC: {                    
      c = get_next_char();
      switch(c) {  /* single character ESC codes */
      case '9' : ClearSideMargins(); break;
      case 'Y' : DisplayFunctions(); break;
      case 'Z' : break; /* dummy; is handled by ESC Y */
      case '=' : HalfLineFeed();     break;
      case 'E' : Reset();            break;
      case 'z' : SelfTest();         break;
      case '(' : Paren(LEFT);        break;
      case ')' : Paren(RIGHT);       break;
      case '&' : Ampersand();        break;
      case '*' : Asterisk();         break;
      case '%' : percent();          break;
      default  : MorePanic( UN_CMD, "ESC %c", c); break; 
      }
      break;
    }
    case BEL  :  /* REAL control characters */
    case BS   :
    case HTAB :
    case LF   :
    case FF   : 
    case CR   :
    case VTAB : put_next_char(c);     break;
    case '@'  : CheckPJL();           break;
    case SI   : SetShiftState(SI);    break;
    case SO   : SetShiftState(SO);    break;
    default   : translate(c);         break;
    }
  }
  exit(0);         /* ...made it to the bitter end */
}








