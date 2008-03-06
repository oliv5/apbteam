# Quick dirty makefile to help you build the test program for twi.
# It is required to clean before building each program because they share the
# same object with different defines.
# Files are copied in *.keep.hex.
all: subclean
	# Slave
	$(MAKE) -f Makefile.slave
	cp test_twi_sl.hex test_twi_sl.keep.hex
	$(MAKE) -f Makefile.slave clean
	# Master
	$(MAKE) -f Makefile.master
	cp test_twi_master.hex test_twi_master.keep.hex
	$(MAKE) -f Makefile.master clean

subclean:
	# General cleaning
	$(MAKE) -f Makefile.slave clean
	$(MAKE) -f Makefile.master clean

clean: subclean
	rm -f test_twi_master.keep.hex test_twi_sl.keep.hex
