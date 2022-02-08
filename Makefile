####################
# C Compiler = GCC #
####################
CC=gcc

##################
# Compiler Flags #
##################
CFLAGS=-ansi -Wall -Wextra -pedantic -pedantic-errors

######################
# Optimization Level #
######################
OLEVEL=-O3

#######################
# Extra Optimizations #
#######################
OEXTRA=-fexpensive-optimizations -flto

####################################
# The following are subdirectories #
# of the current work directory.   #
####################################

###############################
# <Compiled Binary Directory> #
###############################
BDIR=bin

########################################
# <Header Include Directory>           #
# The "-I" flags specifies searching   #
# the immediate work directory for the #
# specified subdirectory.              #
########################################
IDIR=-Iinclude

####################################
# <Compiled Object Code Directory> #
####################################
ODIR=obj

_BINOBJ=utils.o dszslz11.o eszslz11.o search.o lz11sli.o
BINOBJ=$(patsubst %,$(ODIR)/%,$(_BINOBJ))

_OBJ=$(_BINOBJ)
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

####################################
# Executed by simply typing "make" #
####################################
all: $(BDIR)/lz11sli

$(ODIR)/%.o: src/%.c
	$(CC) $(CFLAGS) $(OLEVEL) $(IDIR) -c -o $@ $<

$(BDIR)/lz11sli: $(BINOBJ)
	$(CC) $(CFLAGS) $(OEXTRA) $(OLEVEL) -s\
    -o $(BDIR)/lz11sli\
    $(ODIR)/utils.o $(ODIR)/lz11sli.o $(ODIR)/search.o\
    $(ODIR)/dszslz11.o $(ODIR)/eszslz11.o

.PHONY: clean

clean:
	rm -rf $(BDIR)/* $(ODIR)/* 
