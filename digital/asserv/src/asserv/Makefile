BASE = ../../../avr
PROGS = asserv
HOST_PROGS = test_motor_model
asserv_SOURCES = main.c twi_proto.c counter_ext.avr.c \
		 simu.host.c motor_model.host.c models.host.c
test_motor_model_SOURCES = test_motor_model.c motor_model.host.c models.host.c
MODULES = proto uart utils math/fixed twi
test_motor_model_MODULES =
CONFIGFILE = avrconfig.h
# atmega8, atmega8535, atmega128...
AVR_MCU = atmega128
# -O2 : speed
# -Os : size
OPTIMIZE = -O2
HOST_LIBS = -lm

include $(BASE)/make/Makefile.gen
