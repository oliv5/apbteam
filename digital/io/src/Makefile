# Base directory of AVR.
BASE = ../../avr
# Name of the program to build.
PROGS = io
# Sources to compile.
io_SOURCES = main.c asserv.c servo.avr.c eeprom.avr.c sharp.c pwm.c \
             switch.avr.c chrono.c main_timer.avr.c servo_pos.c \
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
FSM_SOURCES := top move filterbridge elevator cylinder init
# Include FSM makefile.
include $(BASE)/make/Makefile.fsm
