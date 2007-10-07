BASE = ../../../avr
AVR_PROGS = avrcounter-port-comb avrcounter-port-one avrcounter-port-two \
	    avrcounter-spi-comb
avrcounter-port-comb_SOURCES = avrcounter-port-comb.c
avrcounter-port-one_SOURCES = avrcounter-port-one.c
avrcounter-port-two_SOURCES = avrcounter-port-two.c
avrcounter-spi-comb_SOURCES = avrcounter-spi-comb.c
# atmega8, atmega8535, atmega128...
AVR_MCU = atmega8
# -O2 : speed
# -Os : size
OPTIMIZE = -O2 -mint8

include $(BASE)/make/Makefile.gen
