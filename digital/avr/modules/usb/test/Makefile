BASE = ../../..
AVR_PROGS = test_usb
test_usb_SOURCES = test_usb.c descriptors.c
MODULES = usb
CONFIGFILE = avrconfig.h
AVR_MCU = at90usb162
# -O2 : speed
# -Os : size
OPTIMIZE = -Os

include $(BASE)/make/Makefile.gen
