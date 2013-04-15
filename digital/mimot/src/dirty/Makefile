BASE = ../../../avr
PROGS = dirty
dirty_SOURCES = main.c timer.avr.c \
		aux_traj.c cs.c \
		twi_proto.c eeprom.avr.c seq.c \
		simu.host.c models.host.c
MODULES = proto uart utils math/fixed twi \
	  motor/encoder motor/encoder/ext motor/output motor/output/pwm_ocr \
	  motor/control_system motor/speed_control motor/pos_control \
	  motor/blocking_detection motor/motor_model
CONFIGFILE = avrconfig.h
# atmega8, atmega8535, atmega128...
AVR_MCU = atmega32
# -O2 : speed
# -Os : size
OPTIMIZE = -Os
HOST_LIBS = -lm

AVR_DEFS = $(EEPROM_DEFAULTS:%=-DEEPROM_DEFAULTS=\"%\")

include $(BASE)/make/Makefile.gen

include eeprom.mak
