BASE = ../../../avr
AVR_PROGS = dev2_gpio
dev2_gpio_SOURCES = main.c descriptors.c select.c gpio.c
MODULES = usb
CONFIGFILE = avrconfig.h
AVR_MCU = at90usb162
# -O2 : speed
# -Os : size
OPTIMIZE = -Os

INCLUDES = -I.. -I.
vpath %.c ../common

include $(BASE)/make/Makefile.gen
