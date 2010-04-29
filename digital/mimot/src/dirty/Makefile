BASE = ../../../avr
PROGS = dirty
dirty_SOURCES = main.c timer.avr.c counter_ext.avr.c pwm.avr.c \
		pwm_ocr.avr.c pos.c speed.c aux.c \
		twi_proto.c eeprom.avr.c state.c \
		simu.host.c motor_model.host.c models.host.c
MODULES = proto uart utils math/fixed twi
CONFIGFILE = avrconfig.h
# atmega8, atmega8535, atmega128...
AVR_MCU = atmega32
# -O2 : speed
# -Os : size
OPTIMIZE = -Os
HOST_LIBS = -lm

include $(BASE)/make/Makefile.gen
