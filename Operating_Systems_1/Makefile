shell: main.o pathutil.o
	gcc main.o pathutil.o -o shell

main.o: main.c
	gcc -c main.c

pathutil.o: pathutil.h pathutil.c
	gcc -c pathutil.c
clean: 
	rm shell *.o
