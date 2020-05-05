all: main_iplib.o ip_lib.o bmp.o
	gcc main_iplib.o ip_lib.o bmp.o -o main -Wall --ansi --pedantic -ggdb -lm

main_iplib.o: main_iplib.c bmp.h
	gcc main_iplib.c -o main_iplib.o -Wall --ansi --pedantic -c -ggdb

ip_lib.o: ip_lib.c ip_lib.h bmp.h
	gcc ip_lib.c -o ip_lib.o -Wall --ansi --pedantic -c -ggdb

test_main: test.o bmp.o
	gcc test.o bmp.o -o test -Wall -lm
	
test.o: test_bmp.c bmp.o
	gcc test_bmp.c -o test.o -Wall -c

bmp.o: bmp.c bmp.h
	gcc bmp.c -o bmp.o -Wall -c

clean:
	rm bmp.o
	rm test.o
	rm ip_lib.o
	rm main
	rm test
