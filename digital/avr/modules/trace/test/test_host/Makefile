BASE = ../../../..

HOST_PROGS = test-trace

test-trace_SOURCES = test-trace.c 

MODULES = trace flash/stub
#OPTIMIZE = -O2
OPTIMIZE = 

include $(BASE)/make/Makefile.gen

all: 
	touch flash.apb
