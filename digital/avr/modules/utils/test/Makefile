BASE = ../../..
PROGS = test_utils test_byte test_crc
AVR_PROGS = test_byte_methods
test_utils_SOURCES = test_utils.c
test_byte_SOURCES = test_byte.c
test_byte_methods_SOURCES = test_byte_methods.c
test_crc_SOURCES = test_crc.c
MODULES = utils
CONFIGFILE = avrconfig.h
# atmega8, atmega8535, atmega128...
AVR_MCU = atmega128
# -O2 : speed
# -Os : size
OPTIMIZE = -O2

include $(BASE)/make/Makefile.gen
