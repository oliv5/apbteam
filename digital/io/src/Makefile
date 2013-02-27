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
	     angfsm.host.c init.c move.c top.c hola.c loader.c fsm_AI_gen.avr.c
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
LIBS = -lm

vpath %.c $(AI_MODULES:%=../../ai/src/%)
vpath %.h $(AI_MODULES:%=../../ai/src/%)
INCLUDES += -I. $(AI_MODULES:%=-I../../ai/src/%)

EXTRA_CLEAN_FILES = angfsm_gen_avr_AI.h fsm_AI_gen.avr.c

include $(BASE)/make/Makefile.gen

# FSM generation.
obj/main.avr.o: angfsm_gen_avr_AI.h
fsm_AI_gen.avr.c: angfsm_gen_avr_AI.h
angfsm_gen_avr_AI.h: io.host
	./$< --ang-gen avr
	mv angfsm_gen_avr_AI.c fsm_AI_gen.avr.c

events.h events.host.c: trace.trc $(BASE)/../../tools/trace/lib/parser.py
	PYTHONPATH=$(BASE)/../../host python $(BASE)/../../tools/trace/trace.py -t create -i $< -o

$(BASE)/../../tools/trace/lib/parser.py: $(BASE)/../../tools/trace/lib/parser.g
	yapps $<

main.c: events.h
