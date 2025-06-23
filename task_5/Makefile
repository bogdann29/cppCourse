CC=g++
FLAGS=-std=c++20 -Wall -Wextra -pedantic
EXEC=main

main: main.o
	$(CC) $(FLAGS) main.o -o $(EXEC)

main.o:
	$(CC) $(FLAGS) -c main.cpp

clean:
	rm -f *.o $(EXEC)