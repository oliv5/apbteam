BASE = ../../..
HOST_PROGS = test_host test_mex
test_host_SOURCES = test_host.c
test_mex_SOURCES = test_mex.c
MODULES = host
CONFIGFILE = avrconfig.h
# atmega8, atmega8535, atmega128...
AVR_MCU = none
# -O2 : speed
# -Os : size
OPTIMIZE = -O2

include $(BASE)/make/Makefile.gen
