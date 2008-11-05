BASE = ../../..
HOST_PROGS = test-trace

test-trace_SOURCES = test-trace.c flash_stub.c

MODULES = trace
#CONFIGFILE = avrconfig.h
# atmega8, atmega8535, atmega128...
#AVR_MCU = atmega128
# -O2 : speed
# -Os : size
OPTIMIZE = -O2

include $(BASE)/make/Makefile.gen
