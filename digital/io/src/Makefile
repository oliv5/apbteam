# Base directory of AVR.
BASE = ../../avr
# Name of the program to build.
PROGS = io
# Sources to compile.
io_SOURCES = main.c asserv.c servo.avr.c eeprom.avr.c trap.c sharp.c \
             switch.avr.c chrono.c main_timer.avr.c \
	     simu.host.c
# Modules needed for IO.
MODULES = proto uart twi utils adc math/fixed path
# Configuration file.
CONFIGFILE = avrconfig.h
# IO board use an ATMega128.
AVR_MCU = atmega128
# Optimize for speed.
OPTIMIZE = -O2

# FSMs.
FSM_SOURCES := getsamples gutter move top filterbridge elevator
# Include FSM makefile.
include $(BASE)/make/Makefile.fsm
