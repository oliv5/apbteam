# Base directory of AVR.
BASE = ../../avr
# Name of the program to build.
PROGS = io
# Sources to compile.
io_SOURCES = main.c servo.avr.c eeprom.avr.c pwm.c \
	     switch.avr.c chrono.c main_timer.avr.c servo_pos.c \
	     twi_master.c asserv.c mimot.c \
	     simu.host.c contact.c usdist.c radar.c \
	     path.c food.c
# Modules needed for IO.
MODULES = proto uart twi utils adc math/fixed math/geometry path/astar \
	  trace flash spi
# Configuration file.
CONFIGFILE = avrconfig.h
# IO board use an ATMega128.
AVR_MCU = atmega128
# Optimize for speed.
OPTIMIZE = -O2
HOST_LIBS = -lm

# FSM.
FSM = ai
FSM_SOURCES := top init move
# Include FSM makefile.
include $(BASE)/make/Makefile.fsm

trace_event.h: trace.trc
	PYTHONPATH=$(BASE)/../../host python $(BASE)/../../tools/trace/trace.py -t create -e $(@:%.h=%_t) -i $< -o $@

main.c: trace_event.h
