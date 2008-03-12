BASE = ../../..
AVR_PROGS = test-flash test-write-onbyte test-erase test-write-anarray test-init

test-write-onbyte_SOURCES = test-write-onebyte.c
test-erase_SOURCES = test-erase.c
test-write-anarray_SOURCES = test-write-anarray.c
test-init_SOURCES = test-init.c
test-flash_SOURCES = test-flash.c

MODULES = utils spi flash proto uart
CONFIGFILE = avrconfig.h
# atmega8, atmega8535, atmega128...
AVR_MCU = atmega128
# -O2 : speed
# -Os : size
OPTIMIZE = -O2

include $(BASE)/make/Makefile.gen
