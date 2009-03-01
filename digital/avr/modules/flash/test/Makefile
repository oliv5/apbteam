BASE = ../../..
AVR_PROGS = test-flash flash_dump

test-flash_SOURCES = test-flash.c
flash_dump_SOURCES = flash-dump.c

MODULES = utils spi flash proto uart
CONFIGFILE = avrconfig.h
# atmega8, atmega8535, atmega128...
AVR_MCU = atmega128
# -O2 : speed
# -Os : size
OPTIMIZE = -O2

include $(BASE)/make/Makefile.gen
