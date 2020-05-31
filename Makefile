all: main_iplib.c ip_lib.o bmp.o
	gcc main_iplib.c ip_lib.o bmp.o -o main_iplib -Wall --ansi --pedantic -lm -g -O1
	
ip_lib.o: ip_lib.c ip_lib.h
	gcc ip_lib.c -o ip_lib.o -c -lm -Wall --ansi --pedantic -g -O1
	
bmp.o: bmp.c bmp.h
	gcc bmp.c -o bmp.o -c -Wall -lm
	
clean:
	rm -f bmp.o
	rm -f ip_lib.o
	rm -f main_iplib.o
	rm -f main_iplib

