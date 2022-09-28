# Makefile for CJet - LaserJet Emulation for CaPSL printers
# Copyright (C) 1996 Michael Huijsmans mgh@sbox.tu-graz.ac.at
#
# Makefile for UNIX systems
# 

# compiler, compiler and linker options 
CC	  = gcc
OPT	  = -O2 -Wall
LINKOPT   = -s

# you should not need to change anything below

SHELL     = /bin/sh

EXECNAME  = cjet

CFLAGS	  = $(OPT)
LDFLAGS   = $(LINKOPT)

OBJ = main.o parse.o function.o raster.o fonts.o misc.o symbol.o\
      page.o pjl.o macro.o io.o


cjet: $(OBJ)
	$(CC) $(LDFLAGS) -o $(EXECNAME) $(OBJ)

clean:
	rm -f *.o $(EXECNAME)


$(OBJ): pcl.h cjet.h capsl.h 
symbol.o: symbol.h 



