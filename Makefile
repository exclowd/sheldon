


build/input.o: src/input.c include/input.h include/utils.h
	gcc -c src/input.c -o build/input.o

build/parse.o: src/parse.c
	gcc -c src/parse.c -o build/parse.o