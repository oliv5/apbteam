BASE = ../../../avr
AVR_PROGS = dev2_twi
dev2_twi_SOURCES = main.c descriptors.c select.c usb_twi.c
MODULES = usb twi
CONFIGFILE = avrconfig.h
AVR_MCU = at90usb162
# -O2 : speed
# -Os : size
OPTIMIZE = -Os

INCLUDES = -I.. -I.
vpath %.c ../common

include $(BASE)/make/Makefile.gen
