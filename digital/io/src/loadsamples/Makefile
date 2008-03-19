CFLAGS = -O2 -Wall

all: loadsamples loadsamples.png 

loadsamples: loadsamples.o loadsamples.o 

loadsamples.c: loadsamples.fsm loadsamples.conf
	python ../../../../tools/dfagen/dfagen.py -o c -d loadsamples.fsm -c loadsamples.conf -p loadsamples

loadsamples.h loadsamples.cb_skel.c loadsamples.cb.h: loadsamples.c

loadsamples.o: loadsamples.cb.h loadsamples.h
loadsamples_cb.o: loadsamples.cb.h loadsamples.h

%.dot: %.fsm
	python ../../../../tools/dfagen/dfagen.py -o dot -d $< -p $(@:%.dot=%)

%.png: %.dot
	dot -Tpng $< -o $@

clean:
	rm -f loadsamples loadsamples.o loadballs_cb.o loadsamples.c loadsamples.h loadsamples_cb_skel.c loadsamples_cb.h loadsamples_cb.c
	rm -f loadsamples.dot loadsamples.png
