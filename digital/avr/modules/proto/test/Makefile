BASE = ../../..
PROGS = test_proto
test_proto_SOURCES = test_proto.c
MODULES = proto uart utils
CONFIGFILE = avrconfig.h
# atmega8, atmega8535, atmega128...
AVR_MCU = atmega8
# -O2 : speed
# -Os : size
OPTIMIZE = -O2

include $(BASE)/make/Makefile.gen
