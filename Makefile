all: main_iplib.o ip_lib.o bmp.o
	gcc main_iplib.o ip_lib.o bmp.o -o main -Wall --ansi --pedantic -ggdb -lm -g -O1 -g3 -O3 -fsanitize=address -fsanitize=undefined -std=gnu89 -Wextra 

main_iplib.o: main_iplib.c bmp.h
	gcc main_iplib.c -o main_iplib.o -Wall --ansi --pedantic -c -ggdb -lm -g -O1 -g3 -O3 -fsanitize=address -fsanitize=undefined -std=gnu89 -Wextra 

ip_lib.o: ip_lib.c ip_lib.h bmp.h
	gcc ip_lib.c -o ip_lib.o -Wall --ansi --pedantic -c -ggdb -lm -g -O1 -g3 -O3 -fsanitize=address -fsanitize=undefined -std=gnu89 -Wextra 
	
tfun: ip_lib.o bmp.o main_test.c
	gcc main_test.c -o main_test.o -Wall -lm -c 
	gcc main_test.o ip_lib.o bmp.o -o tfun -Wall --ansi --pedantic -ggdb -lm -g3 -O3 -fsanitize=address -fsanitize=undefined -std=gnu89 -Wextra 
	
test_main: test.o bmp.o
	gcc test.o bmp.o -o test -Wall -lm
	
test.o: test_bmp.c bmp.o
	gcc test_bmp.c -o test.o -Wall -c

bmp.o: bmp.c bmp.h
	gcc bmp.c -o bmp.o -Wall -c

clean:
	rm -f bmp.o
	rm -f test.o
	rm -f ip_lib.o
	rm -f main
	rm -f main_test.o
	rm -f test
	rm -f tfun
