BASE = ../../../avr
PROGS = asserv
asserv_SOURCES = main.c timer.avr.c \
		 postrack.c traj.c cs.c \
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
LIBS = -lm

vpath %.c ../../../asserv/src/asserv
INCLUDES += -I. -I../../../asserv/src/asserv

include $(BASE)/make/Makefile.gen
