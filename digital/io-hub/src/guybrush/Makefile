# Base directory of AVR.
BASE = ../../../avr
# Name of the program to build.
PROGS = io_hub
# Sources to compile.
io_hub_SOURCES = main.c top.c strat.c \
		 radar_defs.c radar.c path.c move.c \
		 pressure.c \
		 logger.avr.c \
		 init.c fsm.host.c fsm_AI_gen.avr.c fsm_queue.c \
		 contact.avr.c contact.host.c \
		 output.c output.host.c \
		 twi_master.c asserv.c mimot.c beacon.c \
		 chrono.c timer.avr.c simu.host.c debug_draw.host.c \
		 bottom_clamp.c
# Modules needed for IO.
MODULES = proto uart twi utils \
	  adc devices/usdist \
	  math/fixed math/geometry path/astar
AI_MODULES = twi_master common utils fsm move
# Configuration file.
CONFIGFILE = avrconfig.h
AVR_MCU = at90usb1287
# Optimize for speed.
OPTIMIZE = -O2
HOST_LIBS = -lm

vpath %.c ../common
vpath %.h ../common
vpath %.c $(AI_MODULES:%=../../../ai/src/%)
vpath %.h $(AI_MODULES:%=../../../ai/src/%)
INCLUDES += -I. -I../common $(AI_MODULES:%=-I../../../ai/src/%)

EXTRA_CLEAN_FILES = fsm_AI_gen.h fsm_AI_gen.avr.c \
		    dot_AI_1.dot dot_AI_1.png dot_AI_1.svg

include $(BASE)/make/Makefile.gen

# FSM generation.
include ../../../ai/src/fsm/Makefile.fsm

obj/main.avr.o: fsm_AI_gen.h
