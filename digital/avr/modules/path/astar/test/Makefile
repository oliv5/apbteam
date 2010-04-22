BASE = ../../../..
HOST_PROGS = test_astar
test_astar_SOURCES = test_astar.c
MODULES = path/astar
CONFIGFILE = avrconfig.h
# atmega8, atmega8535, atmega128...
AVR_MCU = atmega128
# -O2 : speed
# -Os : size
OPTIMIZE = -O2

include $(BASE)/make/Makefile.gen

check: test_astar.pl test_astar.host
	perl $^
