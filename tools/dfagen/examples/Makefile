CFLAGS = -O2 -Wall

all: ex1 ex2 ex1.png ex2.png ex2_tpl import.dump.check

ex1: ex1.o ex1_cb.o

ex1.c: ex1.fsm ex1.conf
	python ../dfagen.py -o c -d ex1.fsm -c ex1.conf -p ex1

ex1.h ex1_cb_skel.c ex1_cb.h: ex1.c

ex1_cb.c: ex1_cb_skel.c ex1_cb.c.patch
	cp $< $@
	patch $@ ex1_cb.c.patch
	rm -f $@.orig

ex1.o: ex1_cb.h ex1.h
ex1_cb.o: ex1_cb.h ex1.h

ex2: ex2.o ex2_cb.o ex2_robot.o

ex2.c: ex2.fsm ex2.conf
	python ../dfagen.py -o c -d ex2.fsm -c ex2.conf -p ex2

ex2.h ex2_cb_skel.c ex2_cb.h: ex2.c

ex2_cb.c: ex2_cb_skel.c ex2_cb.c.patch
	cp $< $@
	patch $@ ex2_cb.c.patch
	rm -f $@.orig

ex2_tpl: ex2.fsm ex2_tpl.conf
	python ../dfagen.py -o c -d $(filter %.fsm, $^) -c $(filter %.conf, $^) -p $@

%.dot: %.fsm
	python ../dfagen.py -o dot -d $< -p $(@:%.dot=%)

%.png: %.dot
	dot -Tpng $< -o $@

%.dump: %.fsm
	python ../dfagen.py --dump -d $< > $@

%.check: % %.ref
	diff $^

import.dump: import.fsm imported1.fsm imported2.fsm imported3.fsm

clean:
	rm -f ex1 ex1.o ex1_cb.o ex1.c ex1.h ex1_cb_skel.c ex1_cb.h ex1_cb.c
	rm -f ex1.dot ex1.png
	rm -f ex2 ex2.o ex2_cb.o ex2_robot.o ex2.c ex2.h ex2_cb_skel.c ex2_cb.h ex2_cb.c
	rm -f ex2.dot ex2.png
	rm -f ex2_tpl_defs.h ex2_tpl_table.h
	rm -f import.dump
