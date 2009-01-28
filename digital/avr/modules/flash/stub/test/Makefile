BASE = ../../../..
HOST_PROGS = test_flash

test_flash_SOURCES = test_flash.c

MODULES = utils flash/stub
OPTIMIZE = -O2

include $(BASE)/make/Makefile.gen

all:
	touch flash.apb
