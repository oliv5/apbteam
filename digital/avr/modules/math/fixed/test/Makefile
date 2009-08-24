BASE = ../../../..
PROGS = test_fixed
SIMU_PROGS = test_fixed
test_fixed_SOURCES = test_fixed.c
MODULES = uart proto utils math/fixed math/random
CONFIGFILE = avrconfig.h
# atmega8, atmega8535, atmega128...
AVR_MCU = atmega128
# -O2 : speed
# -Os : size
OPTIMIZE = -O2

HOST_LIBS = -lm

include $(BASE)/make/Makefile.gen

# Automatic tests.

TEST_FILES = test.mul test.div test.cos test.sqrt
TEST_FILES := $(TEST_FILES:%=%.host) $(TEST_FILES:%=%.simu)
code_mul = m
code_div = d
code_cos = c
code_sqrt = s
code = $(code_$(1:test.%.$(2)=%))

clean.test:
	rm -f $(TEST_FILES)

SIMULAVR = simulavr -d $(AVR_MCU) -W 0x22,- -R 0x20,- -T uart0_exit
CR2NL = tr '\r' '\n'

.PRECIOUS: $(TEST_FILES)

test.%.host: test_fixed.host
	echo '!$(call code,$@,host)' | ./$< > $@

test.%.simu: test_fixed.avr.simu.elf
	echo -e '!$(call code,$@,simu)\r' | $(SIMULAVR) -f $< | $(CR2NL) > $@
