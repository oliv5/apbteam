CFLAGS = -O2 -Wall

all: getsamples getsamples.png 

getsamples: getsamples.o getsamples_cb.o 

getsamples.c: getSamplesFsm.fsm getSamplesFsm.conf
	python ../../../../tools/dfagen/dfagen.py -o c -d getSamplesFsm.fsm -c getSamplesFsm.conf -p getsamples

getsamples.h getsamples_cb_skel.c getsamples_cb.h: getsamples.c

getsamples.o: getsamples_cb.h getsamples.h
getsamples_cb.o: getsamples_cb.h getsamples.h

dot: getSamplesFsm.fsm
	python ../../../../tools/dfagen/dfagen.py -o dot -d getSamplesFsm.fsm -p getsamples

png: getSamplesFsm.fsm 
	make dot
	dot -Tpng getsamples.dot -o getsamples.png

clean:
	rm -f getsamples getsamples.o getsamples_cb.o getsamples.c getsamples.h getsamples_cb_skel.c getsamples_cb.h getsamples_cb.c
	rm -f getsamples.dot getsamples.png
