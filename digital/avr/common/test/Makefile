BASE = ../..
PROGS = test_io_bus
ASM_FILES = test_asm_bus.lst test_asm_manual.lst
EXTRA_CLEAN_FILES = $(ASM_FILES)
test_io_bus_SOURCES = test_io_bus.c
MODULES = math/random
CONFIGFILE = /dev/null
AVR_MCU = atmega128
# -O2 : speed
# -Os : size
OPTIMIZE =

include $(BASE)/make/Makefile.gen

avr: $(ASM_FILES)
test_asm_%.lst: test_io_bus.avr.lst
	sed -ne '/test_asm_$*>:$$/,/ret$$/p' < $< \
		| sed -e 's/^[ 0-9a-f]\{4\}:\t[ 0-9a-f]\{5\}/\t/' > $@
