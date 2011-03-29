BASE = ../../../..
PROGS = test_encoder_ext
test_encoder_ext_SOURCES = test_encoder.c
MODULES = utils uart proto math/fixed motor/encoder motor/encoder/ext
CONFIGFILE = avrconfig_xmem.h
# atmega8, atmega8535, atmega128...
AVR_MCU = atmega128
# -O2 : speed
# -Os : size
OPTIMIZE = -Os

TEST_MCU = atmega128
TEST_CONFIGFILES = avrconfig_xmem.h avrconfig_io.h

include $(BASE)/make/Makefile.gen
