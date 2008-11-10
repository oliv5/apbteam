BASE = ../../../..
AVR_PROGS = test-trace flood

test-trace_SOURCES = test-trace.c
flood_SOURCES = flood.c

MODULES = utils spi flash proto uart trace
CONFIGFILE = avrconfig.h
# atmega8, atmega8535, atmega128...
AVR_MCU = atmega128
# -O2 : speed
# -Os : size
OPTIMIZE = -O2

include $(BASE)/make/Makefile.gen
