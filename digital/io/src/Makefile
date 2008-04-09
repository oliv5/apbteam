BASE = ../../avr
PROGS = io
io_SOURCES = main.c asserv.c servo.avr.c eeprom.avr.c trap.c sharp.c \
	     simu.host.c \
	     fsm.c getsamples.c getsamples_fsm.c getsamples_cb.c \
	     gutter_fsm.c gutter_cb.c move_fsm.c move_cb.c move.c 
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
#
# To help user, the user can define the CB variable to request a particular
# behaviour.
#
# If CB=touch, ignore the auto-generation and touch the %_cb.c file.
#
# If CB=vimdiff, use vimdiff to merge differences.
#
# If CB=merge, try to merge automatically.  In this case, it uses a old copy
# of %_cb_skel.c to merge.  This will fail the first time if no old copy is
# available.

# If templates are modified, files should be rebuilt.
FSM_TEMPLATES := template_cb.h template_cb_decl.h template_cb_impl.c \
		 template_cb_skel.c template_fsm.c template_fsm.h
FSM_TEMPLATES := $(FSM_TEMPLATES:%=fsm_templates/%)

# Theses files are auto-generated, remove them on clean.
EXTRA_CLEAN_FILES = getsamples_fsm.h getsamples_fsm.c getsamples_cb.h \
		    getsamples_cb_skel.c getsamples.png \
		    gutter_fsm.h gutter_fsm.c gutter_cb.h \
		    gutter_cb_skel.c gutter.png \
		    move_fsm.h move_fsm.c move.png move_cb.h move_cb_skel.c \
		    mainFsm.h mainFsm.c mainFsm.png mainFsm_cb.h \
		    mainFsm_cb_skel.c

# Bootstrap, fsm should be generated before fsm.h is used.
fsm.c: fsm.h
fsm.h: getsamples_fsm.h gutter_fsm.h move_fsm.h

DFAGEN = python $(BASE)/../../tools/dfagen/dfagen.py

%_fsm.h %_fsm.c %_cb.h %_cb_skel.c: %.fsm fsm.conf $(FSM_TEMPLATES)
	$(DFAGEN) -o c -d $< -c fsm.conf -p $(<:%.fsm=%)

ifndef CB
%_cb.c: %_cb_skel.c
	@echo "Please update $@"
	@false
else ifeq ($(CB),touch)
%_cb.c: %_cb_skel.c
	@echo "Updating $@ blindly"
	cp $< .$<.old
	touch $@
else ifeq ($(CB),merge)
%_cb.c: %_cb_skel.c
	@echo "Try to merge $@..."
	@if [ -r .$<.old ]; then \
		merge $@ .$<.old $<; \
		RET=$$?; \
	else \
		echo "No history, cannot merge"; \
		RET=1; \
	fi; \
	cp $< .$<.old; \
	exit $$RET
else
%_cb.c: %_cb_skel.c
	@echo "Using $(CB) to update $@"
	cp $< .$<.old
	$(CB) $@ $<
endif


png: getsamples.png gutter.png move.png mainFsm.png

%.png: %.dot
	dot -Tpng -o $@ $<

%.dot: %.fsm
	$(DFAGEN) -o dot -d $< -p $(<:%.fsm=%)
