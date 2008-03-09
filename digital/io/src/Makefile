BASE = ../../avr
AVR_PROGS = io
io_SOURCES = main.c asserv.c
MODULES = proto uart twi
CONFIGFILE = avrconfig.h
# atmega8, atmega8535, atmega128...
AVR_MCU = atmega128
# -O2 : speed
# -Os : size
OPTIMIZE = -O2

include $(BASE)/make/Makefile.gen
