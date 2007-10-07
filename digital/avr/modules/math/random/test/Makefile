BASE = ../../../..
PROGS = test_random
test_random_SOURCES = test_random.c
MODULES = uart proto math/random
CONFIGFILE = avrconfig.h
# atmega8, atmega8535, atmega128...
AVR_MCU = atmega8
# -O2 : speed
# -Os : size
OPTIMIZE = -Os

include $(BASE)/make/Makefile.gen
