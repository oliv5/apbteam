BASE = ../../../avr
PROGS = asserv
AVR_PROGS = test_counter
HOST_PROGS = test_motor_model
asserv_SOURCES = main.c timer.avr.c counter_ext.avr.c pwm.avr.c pos.c speed.c \
		 postrack.c traj.c twi_proto.c eeprom.avr.c state.c \
		 simu.host.c motor_model.host.c models.host.c
test_counter_SOURCES = test_counter.c timer.avr.c
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
