CC=g++
FLAGS=-std=c++20 -Wall -Wextra -pedantic
EXEC=main
DEPS=$(wildcard *.h)

main: main.o
	$(CC) $(FLAGS) main.o -o $(EXEC)

main.o: main.cpp $(DEPS)
	$(CC) $(FLAGS) -c main.cpp

clean:
	rm -f *.o $(EXEC)