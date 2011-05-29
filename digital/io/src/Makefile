# Base directory of AVR.
BASE = ../../avr
# Name of the program to build.
PROGS = io
# Sources to compile.
io_SOURCES = main.c fsm_queue.c eeprom.avr.c pwm.c \
	     switch.avr.c chrono.c timer.avr.c \
	     twi_master.c asserv.c mimot.c \
	     simu.host.c contact.c radar.c radar_defs.c \
	     path.c food.c events.host.c \
	     fsm.host.c init.c move.c top.c hola.c loader.c fsm_AI_gen.avr.c
# Modules needed for IO.
MODULES = proto uart twi utils adc math/fixed math/geometry path/astar \
	  devices/usdist devices/servo \
	  trace flash spi
AI_MODULES = twi_master common utils fsm move
# Configuration file.
CONFIGFILE = avrconfig.h
# IO board use an ATMega128.
AVR_MCU = atmega128
# Optimize for speed.
OPTIMIZE = -O2
HOST_LIBS = -lm

vpath %.c $(AI_MODULES:%=../../ai/src/%)
vpath %.h $(AI_MODULES:%=../../ai/src/%)
INCLUDES += -I. $(AI_MODULES:%=-I../../ai/src/%)

EXTRA_CLEAN_FILES = fsm_AI_gen.h fsm_AI_gen.avr.c

include $(BASE)/make/Makefile.gen

# FSM generation.
obj/main.avr.o: fsm_AI_gen.h
fsm_AI_gen.avr.c: fsm_AI_gen.h
fsm_AI_gen.h: io.host
	./$< --gen
	mv fsm_AI_gen.c fsm_AI_gen.avr.c

events.h events.host.c: trace.trc
	PYTHONPATH=$(BASE)/../../host python $(BASE)/../../tools/trace/trace.py -t create -i $< -o

main.c: events.h
