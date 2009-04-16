BASE = ../../..
AVR_PROGS = test_isp
test_isp_SOURCES = test_isp.c
MODULES = isp utils
CONFIGFILE = avrconfig.h
# atmega8, atmega8535, atmega128...
AVR_MCU = atmega8
# -O2 : speed
# -Os : size
OPTIMIZE = -O2

include $(BASE)/make/Makefile.gen
