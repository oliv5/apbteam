# Base directory of AVR.
BASE = ../../../avr
# Name of the program to build.
PROGS = io_hub
# Sources to compile.
io_hub_SOURCES = main.c \
		 clamp.c \
		 fsm.host.c fsm_AI_gen.avr.c \
		 pwm.avr.c pwm.host.c \
		 contact.avr.c contact.host.c \
		 twi_master.c asserv.c mimot.c \
		 chrono.c timer.avr.c simu.host.c
# Modules needed for IO.
MODULES = proto uart twi utils \
	  math/fixed math/geometry
AI_MODULES = twi_master common utils fsm
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

EXTRA_CLEAN_FILES = fsm_AI_gen.h fsm_AI_gen.avr.c

include $(BASE)/make/Makefile.gen

# FSM generation.
obj/main.avr.o: fsm_AI_gen.h
fsm_AI_gen.avr.c: fsm_AI_gen.h
fsm_AI_gen.h: io_hub.host
	./$< --gen
	mv fsm_AI_gen.c fsm_AI_gen.avr.c