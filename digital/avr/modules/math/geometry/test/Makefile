BASE = ../../../..
HOST_PROGS = test_geometry
SIMU_PROGS = test_geometry
test_geometry_SOURCES = test_geometry.c
MODULES = utils uart math/fixed math/geometry
CONFIGFILE = avrconfig.h
# atmega8, atmega8535, atmega128...
AVR_MCU = atmega128
# -O2 : speed
# -Os : size
OPTIMIZE = -O2

include $(BASE)/make/Makefile.gen
