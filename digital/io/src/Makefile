# Base directory of AVR.
BASE = ../../avr
# Name of the program to build.
PROGS = io
# Sources to compile.
io_SOURCES = main.c servo.avr.c eeprom.avr.c pwm.c \
	     switch.avr.c chrono.c main_timer.avr.c servo_pos.c \
	     twi_master.c asserv.c mimot.c \
	     simu.host.c contact.c usdist.c radar.c \
	     path.c food.c events.host.c \
	     fsm.host.c init.c move.c top.c hola.c loader.c fsm_AI_gen.avr.c
# Modules needed for IO.
MODULES = proto uart twi utils adc math/fixed math/geometry path/astar \
	  trace flash spi
AI_MODULES = twi_master
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

include $(BASE)/make/Makefile.gen

#FSM generation
obj/main.avr.o: fsm_AI_gen.h
fsm_AI_gen.avr.c fsm_AI_gen.h: host
	./io.host --gen
	mv fsm_AI_gen.c fsm_AI_gen.avr.c

events.h events.host.c: trace.trc
	PYTHONPATH=$(BASE)/../../host python $(BASE)/../../tools/trace/trace.py -t create -i $< -o

main.c: events.h
