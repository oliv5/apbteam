BASE = ../../avr
HOST_PROGS = beacon
beacon_SOURCES = main_simu.c position.c formula.c recovery.c update.c trust.c
MODULES = math/fixed utils
# atmega8, atmega8535, atmega128...
AVR_MCU = atmega128
# -O2 : speed
# -Os : size
OPTIMIZE = -O2 -lm -DSIMULATOR
include $(BASE)/make/Makefile.gen


BITCLOUD_PATH = $(PWD)/Bitcloud_stack
APB_AVR_PATH = $(PWD)/../../avr
CONFIG_NAME = All_ZigBit_Atmega1281_Rf230_8Mhz_Gcc
#CONFIG_NAME = Coordinator_ZigBit_Atmega1281_Rf230_8Mhz_Gcc
#CONFIG_NAME = Router_ZigBit_Atmega1281_Rf230_8Mhz_Gcc
#CONFIG_NAME = All_Sec_ZigBit_Atmega1281_Rf230_8Mhz_Gcc
#CONFIG_NAME = Coordinator_Sec_ZigBit_Atmega1281_Rf230_8Mhz_Gcc
#CONFIG_NAME = Router_Sec_ZigBit_Atmega1281_Rf230_8Mhz_Gcc

NUMBER:= 0 1 2 3

simu:host
	python simulator.py
	
avr:
	for count in $(NUMBER); do\
		$(MAKE) -C makefiles/ -f Makefile_$(CONFIG_NAME) clean BITCLOUD_PATH=$(BITCLOUD_PATH);\
		$(MAKE) -C makefiles/ -f Makefile_$(CONFIG_NAME) all APP_NAME=beacon_$$count BITCLOUD_PATH=$(BITCLOUD_PATH) APB_AVR_PATH=$(APB_AVR_PATH) LOL_NUMBER=$$count;\
	done;\

flash:
	avrdude -c stk500v2 -P /dev/ttyUSB1 -p atmega1281 -B3 -U flash:w:obj/beacon_$(TARGET).hex
	../../dev2/tools/dev2ctl.py -s 1

clean:
	for count in $(NUMBER); do\
		$(MAKE) -C makefiles/$(PROJECT_NAME) -f Makefile_$(CONFIG_NAME) clean APP_NAME=beacon_$$count BITCLOUD_PATH=$(BITCLOUD_PATH);\
	done;\
	



