BASE = ../../../..
AVR_PROGS = test_twi_soft
test_twi_soft_SOURCES = test_twi.c
MODULES = twi uart proto math/random utils
CONFIGFILE = avrconfig.h
# atmega8, atmega8535, atmega128...
AVR_MCU = atmega128
# -O2 : speed
# -Os : size
OPTIMIZE = -Os

vpath %.c ..

include $(BASE)/make/Makefile.gen
