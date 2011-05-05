# Base directory of AVR.
BASE = ../../../avr
# Name of the program to build.
PROGS = io_hub
# Sources to compile.
io_hub_SOURCES = main.c \
		 pwm.avr.c pwm.host.c \
		 twi_master.c asserv.c mimot.c \
		 chrono.c timer.avr.c simu.host.c
# Modules needed for IO.
MODULES = proto uart twi utils \
	  math/fixed math/geometry
AI_MODULES = twi_master common utils
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

include $(BASE)/make/Makefile.gen
