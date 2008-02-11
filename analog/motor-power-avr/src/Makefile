BASE = ../../../digital/avr
PROGS = mp
HOST_PROGS = test_mp
mp_SOURCES = main.c mp_pwm_L_.c mp_pwm_LR_.c adc.c
test_mp_SOURCES = test_mp.c 
MODULES = adc proto uart utils math/fixed
test_mp_MODULES =
CONFIGFILE = avrconfig.h
# atmega8, atmega8535, atmega128...
AVR_MCU = atmega16
# -O2 : speed
# -Os : size
OPTIMIZE = -O2
HOST_LIBS = -lm

include $(BASE)/make/Makefile.gen
