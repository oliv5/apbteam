BASE= ../../../../avr
HOST_PROGS = fsm
IOBASE = ../..
fsm_SOURCES = main.c $(IOBASE)/fsm.c \
	      $(IOBASE)/move_cb.c $(IOBASE)/move_fsm.c \
	      $(IOBASE)/move.c
MODULES = utils
CONFIGFILE =
# -O2 : speed
# -Os : size
OPTIMIZE = -O2

include $(BASE)/make/Makefile.gen
