all:
	gcc *.c -o main

run:
	./main

valgrind:
	valgrind --leak-check=full --track-origins=yes ./main
