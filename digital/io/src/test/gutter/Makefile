BASE= ../../../../avr
HOST_PROGS = fsm
IOBASE = ../..
fsm_SOURCES = main.c $(IOBASE)/fsm.c \
	      $(IOBASE)/gutter_cb.c $(IOBASE)/gutter_fsm.c
MODULES =
CONFIGFILE =
# -O2 : speed
# -Os : size
OPTIMIZE = -O2

include $(BASE)/make/Makefile.gen
