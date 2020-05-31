all: main_iplib.o ip_lib.o bmp.o
	gcc main_iplib.o ip_lib.o bmp.o -o main_iplib -Wall --ansi --pedantic -ggdb -lm -g -O1 -g3 -O3 -fsanitize=address -fsanitize=undefined -std=gnu89 -Wextra 
	
main_iplib.o: main_iplib.c bmp.h
	gcc main_iplib.c -o main_iplib.o -c -lm -Wall --ansi --pedantic -g -O1 -g3 -O3 -std=gnu89 -Wextra -fsanitize=address -fsanitize=undefined
	
ip_lib.o: ip_lib.c ip_lib.h bmp.h
	gcc ip_lib.c -o ip_lib.o -c -lm -Wall --ansi --pedantic -g -O1 -g3 -O3 -std=gnu89 -Wextra -fsanitize=address -fsanitize=undefined
	
bmp.o: bmp.c bmp.h
	gcc bmp.c -o bmp.o -Wall -c
	
clean:
	rm -f bmp.o
	rm -f ip_lib.o
	rm -f main_iplib.o
	rm -f main_iplib
