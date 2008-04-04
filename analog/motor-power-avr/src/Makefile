BASE = ../../../digital/avr
AVR_PROGS = mp
mp_SOURCES = main.c mp_pwm_L_.c mp_pwm_R_.c mp_pwm_LR_.c
MODULES = proto uart utils math/fixed
CONFIGFILE = avrconfig.h
# atmega8, atmega8535, atmega128...
AVR_MCU = atmega16
# -O2 : speed
# -Os : size
OPTIMIZE = -O2
HOST_LIBS = -lm

include $(BASE)/make/Makefile.gen

SED_L_R = sed -e 's/_L_/_R_/g'
EXTRA_CLEAN_FILES = mp_pwm_R_.c mp_pwm_R_.h

mp_pwm_R_.c: mp_pwm_L_.c
	${SED_L_R} $< > $@

mp_pwm_R_.h: mp_pwm_L_.h
	${SED_L_R} $< > $@
	
# Extra dependencies for bootstrap.
main.c mp_pwm_LR_.c mp_pwm_R_.c: mp_pwm_R_.h
