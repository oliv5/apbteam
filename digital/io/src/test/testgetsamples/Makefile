BASE= ../../../../avr
HOST_PROGS = getsamples
IOBASE = ../..
getsamples_SOURCES = main.c ../common.host.c $(IOBASE)/fsm.c $(IOBASE)/getsamples.c \
	      $(IOBASE)/getsamples_cb.c $(IOBASE)/getsamples_fsm.c \
	      $(IOBASE)/top.c $(IOBASE)/top_cb.c $(IOBASE)/top_fsm.c \
	      $(IOBASE)/move.c $(IOBASE)/move_cb.c $(IOBASE)/move_fsm.c \
	      $(IOBASE)/simu.host.c
MODULES =
CONFIGFILE = $(IOBASE)/avrconfig.h
# -O2 : speed
# -Os : size
OPTIMIZE = -O2

include $(BASE)/make/Makefile.gen
