
obj=obj/term.o obj/draw.o obj/utils.o obj/sleep.o
eflags=-Wpedantic -Wall -Wextra
flags=-I include/ $(eflags) -g3

bin/orb: src/main.c $(obj)
	gcc src/main.c obj/* $(flags) -lncursesw -lm -o bin/orb 

obj/term.o: src/term.c include/term.h
	gcc -c src/term.c $(flags) -o obj/term.o

obj/draw.o: src/draw.c include/draw.h
	gcc -c src/draw.c $(flags) -o obj/draw.o

obj/utils.o: src/utils.c include/utils.h
	gcc -c src/utils.c $(flags) -o obj/utils.o

obj/sleep.o: src/sleep.c include/sleep.h
	gcc -c src/sleep.c $(flags) -o obj/sleep.o

run:
	./bin/orb

clean:
	rm obj/*
	rm bin/*
