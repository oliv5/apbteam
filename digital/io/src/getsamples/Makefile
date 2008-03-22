CFLAGS = -O2 -Wall

all: getSamplesFsm.fsm getSamplesFsm.conf
	make prgm
	make exe
	make dot
	make png

prgm: getSamplesFsm.fsm getSamplesFsm.conf
	python ../../../../tools/dfagen/dfagen.py -o c -d $< -c getSamplesFsm.conf -p getsamples 

exe: getSamplesFsm.fsm getSamplesFsm.conf
	make prgm
	if [ -f getsamples_cb.c ]; then patch getsamples_cb.c getsamples_cb_skel.c; else mv getsamples_cb_skel.c getsamples_cb.c; fi;

dot: getSamplesFsm.fsm getSamplesFsm.conf
	python ../../../../tools/dfagen/dfagen.py -o dot -d $< -c getSamplesFsm.conf -p getsamples

png: getsamples.dot
	dot -Tpng -o getsamples.png getsamples.dot

clean:
	rm -f getsamples*
