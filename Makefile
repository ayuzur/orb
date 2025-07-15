
obj=obj/term.o

flags=-I include/

bin/orb: src/main.c $(obj)
	gcc src/main.c obj/* $(flags) -o bin/orb 

obj/term.o: src/term.c include/term.h
	gcc -c src/term.c $(flags) -o obj/term.o
