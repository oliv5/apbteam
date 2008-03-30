CFLAGS = -O2 -Wall

all: prgm exe dot png 

prgm: gutter.fsm gutter.conf
	python ../../../../tools/dfagen/dfagen.py -o c -d $< -c gutter.conf -p gutter 

exe: gutter.fsm gutter.conf
	make prgm

dot: gutter.fsm gutter.conf
	python ../../../../tools/dfagen/dfagen.py -o dot -d $< -c gutter.conf -p gutter

png: gutter.dot
	dot -Tpng -o gutter.png gutter.dot

clean:
	rm -f gutter.h
	rm -f gutter_cb.h
	rm -f gutter.dot
	rm -f gutter.png
	rm -f gutter.c
