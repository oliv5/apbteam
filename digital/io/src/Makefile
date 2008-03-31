BASE = ../../avr
AVR_PROGS = io
io_SOURCES = main.c asserv.c servo.c eeprom.c trap.c sharp.c \
	     fsm.c getsamples.c getsamples_fsm.c getsamples_cb.c \
	     gutter_fsm.c gutter_cb.c
MODULES = proto uart twi utils adc
CONFIGFILE = avrconfig.h
# atmega8, atmega8535, atmega128...
AVR_MCU = atmega128
# -O2 : speed
# -Os : size
OPTIMIZE = -O2

include $(BASE)/make/Makefile.gen

# FSM build:
#
# The %_cb.c file can not be auto-generated.  When the %_cb_skel.c is changed,
# user should check if they still are synchronised using a merge program, like
# vimdiff.

# If templates are modified, files should be rebuilt.
FSM_TEMPLATES := template_cb.h template_cb_decl.h template_cb_impl.c \
		 template_cb_skel.c template_fsm.c template_fsm.h
FSM_TEMPLATES := $(FSM_TEMPLATES:%=fsm_templates/%)

# Theses files are auto-generated, remove them on clean.
EXTRA_CLEAN_FILES = getsamples_fsm.h getsamples_fsm.c getsamples_cb.h \
		    getsamples_cb_skel.c getsamples.png \
		    gutter_fsm.h gutter_fsm.c gutter_cb.h \
		    gutter_cb_skel.c gutter.png

# Bootstrap, fsm should be generated before fsm.h is used.
fsm.c: fsm.h
fsm.h: getsamples_fsm.h gutter_fsm.h

DFAGEN = python $(BASE)/../../tools/dfagen/dfagen.py

%_fsm.h %_fsm.c %_cb.h %_cb_skel.c: %.fsm fsm.conf $(FSM_TEMPLATES)
	$(DFAGEN) -o c -d $< -c fsm.conf -p $(<:%.fsm=%)

%_cb.c: %_cb_skel.c
	echo "Please update $@"
	false

png: getsamples.png gutter.png

%.png: %.dot
	dot -Tpng -o $@ $<

%.dot: %.fsm
	$(DFAGEN) -o dot -d $< -p $(<:%.fsm=%)
