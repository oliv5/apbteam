BASE = ../../..
PROGS = test_uart
SIMU_PROGS = test_uart
test_uart_SOURCES = test_uart.c
MODULES = uart utils
CONFIGFILE = avrconfig_ring.h
# atmega8, atmega8535, atmega128...
AVR_MCU = atmega8
# -O2 : speed
# -Os : size
OPTIMIZE = -O2

# Test compilations.
TEST_MCU = atmega8 atmega8535 atmega16 atmega32 atmega64 atmega128 at90usb162
TEST_CONFIGFILES = avrconfig_ring.h avrconfig_polling.h avrconfig_twoports.h
avrconfig_twoports_TEST_MCU = atmega128

include $(BASE)/make/Makefile.gen
