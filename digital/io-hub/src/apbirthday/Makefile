BASE = ../../../ucoolib

TARGETS = host stm32f4
PROGS = apbirthday
apbirthday_SOURCES = main.cc robot.cc hardware.host.cc hardware.stm32.cc

MODULES = hal/uart hal/usb base/proto

include $(BASE)/build/top.mk

clean: clean.project
clean.project:
	rm -f usb zb_uart
