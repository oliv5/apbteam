BASE = ../../../avr
HOST_PROGS = simu_stub
simu_stub_SOURCES = simu_stub.host.c
MODULES = utils host twi
CONFIGFILE = avrconfig.h
# atmega8, atmega8535, atmega128...
AVR_MCU = atmega128
# -O2 : speed
# -Os : size
OPTIMIZE = -O2

include $(BASE)/make/Makefile.gen
