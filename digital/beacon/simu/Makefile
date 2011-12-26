BASE = ../../avr
HOST_PROGS = beacon
beacon_SOURCES = beacon.c position.c
MODULES = math/fixed utils
CONFIGFILE = avrconfig.h
# atmega8, atmega8535, atmega128...
AVR_MCU = atmega128
# -O2 : speed
# -Os : size
OPTIMIZE = -O2 -lm

include $(BASE)/make/Makefile.gen
