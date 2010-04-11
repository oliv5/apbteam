BASE = ../../..
TOOLS= $(BASE)/../../tools
AVR_PROGS = test_trace flood
HOST_PROGS = test_trace flood

test_trace_SOURCES = test-trace.c
flood_SOURCES = flood.c

MODULES = utils spi flash proto uart trace
CONFIGFILE = avrconfig.h
# atmega8, atmega8535, atmega128...
AVR_MCU = atmega128
# -O2 : speed
# -Os : size
OPTIMIZE = -O2

events:
	python $(TOOLS)/trace/trace.py -i $(TOOLS)/trace/example/trace.trc -t create -o
	mv events.h ..
	mv events.host.c ..

include $(BASE)/make/Makefile.gen
