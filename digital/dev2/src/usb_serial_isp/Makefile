BASE = ../../../avr
AVR_PROGS = dev2_serial_isp
dev2_serial_isp_SOURCES = main.c descriptors.c serial.c select.c usb_isp.c gpio.c
MODULES = usb uart isp spi
CONFIGFILE = avrconfig.h
AVR_MCU = at90usb162
# -O2 : speed
# -Os : size
OPTIMIZE = -Os

INCLUDES = -I.. -I.
vpath %.c ../common

include $(BASE)/make/Makefile.gen
