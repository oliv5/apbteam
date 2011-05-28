BASE = ../../../avr
AVR_PROGS = codebar_reader
codebar_reader_SOURCES = codebar.c timer.avr.c
MODULES = proto twi uart utils
AI_MODULES = utils
CONFIGFILE = avrconfig.h
AVR_MCU = atmega164p
# -O2 : speed
# -Os : size
OPTIMIZE = -Os

vpath %.c $(AI_MODULES:%=../../../ai/src/%)
vpath %.h $(AI_MODULES:%=../../../ai/src/%)
INCLUDES += $(AI_MODULES:%=-I../../../ai/src/%)

include $(BASE)/make/Makefile.gen
