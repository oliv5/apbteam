ifndef APB_REPO_PATH
$(error Please set your environment variable APB_REPO_PATH with the path where is installed your APB repository)
endif

# Simulator makefile part
BASE = $(APB_REPO_PATH)/digital/avr
HOST_PROGS = beacon
beacon_SOURCES = main_simu.c position.c formula.c recovery.c update.c trust.c
MODULES = math/fixed utils
AVR_MCU = atmega1281
OPTIMIZE = -O2 -lm -DSIMULATOR
include $(BASE)/make/Makefile.gen


# Bitcloud makefile part
BITCLOUD_BASE = $(APB_REPO_PATH)/digital/zigbit/bitcloud
BITCLOUD_MAKEFILE = $(BITCLOUD_BASE)/makefiles
BITCLOUD_STACK = $(BITCLOUD_BASE)/stack
PROJECT_BASE = $(APB_REPO_PATH)/digital/beacon/src

CONFIG_NAME = All_ZigBit_Atmega1281_Rf230_8Mhz_Gcc
#CONFIG_NAME = Coordinator_ZigBit_Atmega1281_Rf230_8Mhz_Gcc
#CONFIG_NAME = Router_ZigBit_Atmega1281_Rf230_8Mhz_Gcc
#CONFIG_NAME = All_Sec_ZigBit_Atmega1281_Rf230_8Mhz_Gcc
#CONFIG_NAME = Coordinator_Sec_ZigBit_Atmega1281_Rf230_8Mhz_Gcc
#CONFIG_NAME = Router_Sec_ZigBit_Atmega1281_Rf230_8Mhz_Gcc

NUMBER:= 0 1 2 3
DEV2 := 	0

simu:host
	python simulator.py
	
avr:
	for count in $(NUMBER); do\
		$(MAKE) -C $(BITCLOUD_MAKEFILE) -f Makefile_$(CONFIG_NAME) clean BITCLOUD_PATH=$(BITCLOUD_STACK);\
		$(MAKE) -C $(BITCLOUD_MAKEFILE) -f Makefile_$(CONFIG_NAME) all APP_NAME=beacon_$$count BITCLOUD_PATH=$(BITCLOUD_STACK) APB_AVR_PATH=$(BASE) PROJECT_BASE=$(PROJECT_BASE) LOL_NUMBER=$$count;\
	done;\

flash0:
	make NUMBER=0 avr
	$(APB_REPO_PATH)/digital/dev2/tools/dev2ctl.py -s 1
	avrdude -c stk500v2 -P /dev/ttyUSB1 -p atmega1281 -B3 -U flash:w:obj/beacon_0.hex

flash1:
	make NUMBER=1 avr
	$(APB_REPO_PATH)/digital/dev2/tools/dev2ctl.py -s 1
	avrdude -c stk500v2 -P /dev/ttyUSB1 -p atmega1281 -B3 -U flash:w:obj/beacon_1.hex
	
flash2:
	make NUMBER=2 avr
	$(APB_REPO_PATH)/digital/dev2/tools/dev2ctl.py -s 1
	avrdude -c stk500v2 -P /dev/ttyUSB1 -p atmega1281 -B3 -U flash:w:obj/beacon_2.hex
	
flash3:
	make NUMBER=3 avr
	$(APB_REPO_PATH)/digital/dev2/tools/dev2ctl.py -s 1
	avrdude -c stk500v2 -P /dev/ttyUSB1 -p atmega1281 -B3 -U flash:w:obj/beacon_3.hex
	
clean:
	for count in $(NUMBER); do\
		$(MAKE) -C $(BITCLOUD_MAKEFILE) -f Makefile_$(CONFIG_NAME) clean APP_NAME=beacon_$$count PROJECT_BASE=$(PROJECT_BASE) BITCLOUD_PATH=$(BITCLOUD_STACK);\
	done;\

