BASE = ../../../..
PROGS = test_output
test_output_SOURCES = test_output.c
MODULES = utils uart proto motor/output $(OUTPUT_MODULE)
CONFIGFILE = avrconfig_ocr.h
# atmega8, atmega8535, atmega128...
AVR_MCU = atmega32
# -O2 : speed
# -Os : size
OPTIMIZE = -Os

ifeq ($(CONFIGFILE),avrconfig_ocr.h)
OUTPUT_MODULE = motor/output/pwm_ocr
else ifeq ($(CONFIGFILE),avrconfig_mp.h)
OUTPUT_MODULE = motor/output/pwm_mp spi
else
OUTPUT_MODULE = motor/output/pwm_ocr motor/output/pwm_mp spi
endif

TEST_MCU = atmega128
TEST_CONFIGFILES = avrconfig_mp.h avrconfig_both.h

include $(BASE)/make/Makefile.gen
