all: main_iplib.o iplib.o bmp.o
	gcc main_iplib.o iplib.o bmp.o -o main -Wall --ansi --pedantic -ggdb -lm

main_iplib.o: main_iplib. bmp.h
	gcc main_iplib.c -o main_iplib.o -Wall --ansi --pedantic -c -ggdb

iplib.o: iplib.c iplib.h bmp.h
	gcc iplib.c -o iplib.o -Wall --ansi --pedantic -c -ggdb

test_main: test.o bmp.o
	gcc test.o bmp.o -o test -Wall -lm
	
test.o: test_bmp.c bmp.o
	gcc test_bmp.c -o test.o -Wall -c

bmp.o: bmp.c bmp.h
	gcc bmp.c -o bmp.o -Wall -c

clean:
	rm bmp.o
	rm test.o
	rm iplib.o
	rm main
	rm test
	

