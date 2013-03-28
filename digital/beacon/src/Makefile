ifndef APB_REPO_PATH
$(error Please set your environment variable APB_REPO_PATH with the path where is installed your APB repository)
endif

# Simulator makefile part
BASE = $(APB_REPO_PATH)/digital/avr
HOST_PROGS = beacon
beacon_SOURCES = main_simu.c position.c formula.c recovery.c update.c trust.c
MODULES = math/fixed utils
AVR_MCU = atmega1281
OPTIMIZE = -O2 -lm -DSIMULATOR -I$(APB_REPO_PATH)/digital/zigbit/common
include $(BASE)/make/Makefile.gen


# Bitcloud makefile part
BITCLOUD_BASE = $(APB_REPO_PATH)/digital/zigbit/bitcloud
BITCLOUD_MAKEFILE = $(BITCLOUD_BASE)/makefiles
BITCLOUD_STACK = $(BITCLOUD_BASE)/stack
BITCLOUD_COMMON_SOURCES = $(APB_REPO_PATH)/digital/zigbit/common
PROJECT_BASE = $(APB_REPO_PATH)/digital/beacon/src

CONFIG_NAME = All_ZigBit_Atmega1281_Rf230_8Mhz_Gcc
#CONFIG_NAME = Coordinator_ZigBit_Atmega1281_Rf230_8Mhz_Gcc
#CONFIG_NAME = Router_ZigBit_Atmega1281_Rf230_8Mhz_Gcc
#CONFIG_NAME = All_Sec_ZigBit_Atmega1281_Rf230_8Mhz_Gcc
#CONFIG_NAME = Coordinator_Sec_ZigBit_Atmega1281_Rf230_8Mhz_Gcc
#CONFIG_NAME = Router_Sec_ZigBit_Atmega1281_Rf230_8Mhz_Gcc

DEV2 := 	0
PROGRAMMING_INTERFACE:=/dev/ttyUSB1
simu:host
	python simulator.py

avr:
	$(MAKE) -C $(BITCLOUD_MAKEFILE) -f Makefile_$(CONFIG_NAME) clean BITCLOUD_PATH=$(BITCLOUD_STACK)
	$(MAKE) -C $(BITCLOUD_MAKEFILE) -f Makefile_$(CONFIG_NAME) all APP_NAME=beacon BITCLOUD_PATH=$(BITCLOUD_STACK) APB_AVR_PATH=$(BASE) PROJECT_BASE=$(PROJECT_BASE) BITCLOUD_COMMON_SOURCES=$(BITCLOUD_COMMON_SOURCES)

fuses:
	$(APB_REPO_PATH)/digital/dev2/tools/dev2ctl.py -s 1
	avrdude -c stk500v2 -P $(PROGRAMMING_INTERFACE) -p atmega1281 -B3 -U lfuse:w:0x62:m -U hfuse:w:0xDC:m

flash:
	make avr
	$(APB_REPO_PATH)/digital/dev2/tools/dev2ctl.py -s 1
	avrdude -c stk500v2 -P $(PROGRAMMING_INTERFACE) -p atmega1281 -B3 -U flash:w:obj/beacon.hex
	
clean:
	$(MAKE) -C $(BITCLOUD_MAKEFILE) -f Makefile_$(CONFIG_NAME) clean APP_NAME=beacon PROJECT_BASE=$(PROJECT_BASE) BITCLOUD_PATH=$(BITCLOUD_STACK);
	\