BASE = ../../../..
AVR_PROGS = test_servo
test_servo_SOURCES = test_servo.c
MODULES = proto devices/servo uart utils
CONFIGFILE = avrconfig.h
# atmega8, atmega8535, atmega128...
AVR_MCU = atmega128
# -O2 : speed
# -Os : size
OPTIMIZE = -O2

include $(BASE)/make/Makefile.gen
