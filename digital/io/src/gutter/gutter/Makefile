BASE= ..

all: main.c $(BASE)/gutter.c $(BASE)/gutter_cb.c
	gcc -g -O2 -Wall main.c $(BASE)/gutter.c $(BASE)/gutter_cb.c -o fsm

clean: fsm 
	rm -f fsm
