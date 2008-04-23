BASE= ../../../../avr
HOST_PROGS = gutter
IOBASE = ../..
gutter_SOURCES = main.c ../common.host.c $(IOBASE)/fsm.c \
	      $(IOBASE)/gutter_cb.c $(IOBASE)/gutter_fsm.c $(IOBASE)/gutter.c \
	      $(IOBASE)/getsamples_cb.c $(IOBASE)/getsamples_fsm.c $(IOBASE)/getsamples.c \
	      $(IOBASE)/top.c $(IOBASE)/top_cb.c $(IOBASE)/top_fsm.c \
	      $(IOBASE)/move.c $(IOBASE)/move_cb.c $(IOBASE)/move_fsm.c \
	      $(IOBASE)/simu.host.c

MODULES =
CONFIGFILE = $(IOBASE)/avrconfig.h
# -O2 : speed
# -Os : size
OPTIMIZE = -O2

include $(BASE)/make/Makefile.gen
