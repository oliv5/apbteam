BASE= ..

all: main.c $(BASE)/getsamples.c $(BASE)/getsamples_cb.c
	gcc -g -O2 -Wall main.c $(BASE)/getsamples.c $(BASE)/getsamples_cb.c -o fsm

clean: fsm 
	rm -f fsm
