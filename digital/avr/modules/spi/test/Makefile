BASE = ../../..
AVR_PROGS = test_spi
test_spi_SOURCES = test_spi.c 
MODULES = utils spi uart proto 
CONFIGFILE = avrconfig.h
# atmega8, atmega8535, atmega128...
AVR_MCU = atmega128
# -O2 : speed
# -Os : size
OPTIMIZE = -O2

# Test compilations.
#TEST_MCU = atmega8 atmega8535 atmega128
#TEST_CONFIGFILES = avrconfig_ring.h avrconfig_polling.h avrconfig_twoports.h
#avrconfig_twoports_TEST_MCU = atmega128

include $(BASE)/make/Makefile.gen
