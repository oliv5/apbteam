# Welcome to APBTeam repository!
#
# This Makefile will compile all projects used for a simulated environment.
# To do it, use:
#
#  make all
#
# You can limit to compile only host version (no AVR) using:
#
#  make host
#
# To define environment to be used for python scripts, use:
#
#  eval $(make env)
#

DIRS_2013 = \
	digital/mimot/src/asserv \
	digital/mimot/src/dirty \
	digital/beacon/src/stub \
	digital/io-hub/src/apbirthday

DIRS_2012 = \
	digital/mimot/src/asserv \
	digital/mimot/src/dirty \
	digital/beacon/src/stub \
	digital/io-hub/src/guybrush

DIRS = $(sort \
	digital/asserv/src/asserv \
	digital/io/src \
	digital/io-hub/src/robospierre \
	$(DIRS_2012) \
	$(DIRS_2013) \
	)

PYTHON_DIRS = \
	digital/dev2/tools \
	digital/asserv/tools \
	digital/mimot/tools \
	digital/beacon/tools \
	digital/io/tools \
	digital/io-hub/tools \
	host

default: $(if $(APBTEAM_DEFAULT),$(APBTEAM_DEFAULT),help)

help:
	@sed -n 's/^# \?//p' < Makefile

all host clean:
	$(foreach dir,$(DIRS),$(MAKE) -C $(dir) $@ && ) true

all.% host.% clean.%:
	$(foreach dir,$(DIRS_$*),$(MAKE) -C $(dir) $(@:.$*=) && ) true

all: libopencm3
libopencm3:
	@if test \! -f digital/ucoolib/lib/libopencm3/Makefile; then \
		echo "no libopencm3, this usually means that you should run" \
		"git submodule init, then git submodule update" >&2; \
		echo "another option is to run make host if you are only" \
		"interrested by simulation"; \
		exit 1; \
	fi
	$(MAKE) -C digital/ucoolib/lib/libopencm3

space :=
space +=
env:
	@echo 'b=$(CURDIR); export b;'
	@echo 'PYTHONPATH=$(subst $(space),:,$(PYTHON_DIRS:%=$$b/%)); export PYTHONPATH;'
	@echo '# Run as "eval $$(make env)"'

.PHONY: help all host env
