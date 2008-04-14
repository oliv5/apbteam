BASE= ../../../../avr
HOST_PROGS = fsm
IOBASE = ../..
fsm_SOURCES = main.c $(IOBASE)/fsm.c $(IOBASE)/getsamples.c \
	      $(IOBASE)/getsamples_cb.c $(IOBASE)/getsamples_fsm.c \
	      $(IOBASE)/top.c $(IOBASE)/top_cb.c $(IOBASE)/top_fsm.c
MODULES =
CONFIGFILE =
# -O2 : speed
# -Os : size
OPTIMIZE = -O2

include $(BASE)/make/Makefile.gen
