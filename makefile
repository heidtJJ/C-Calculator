CC = gcc
FLAGS = -g -Wall

boo: calc.o tree.o
	$(CC) $(FLAGS) -o boo calc.o tree.o

calc.o: calc.c
	$(CC) $(FLAGS) -c calc.c

tree.o: tree.c
	$(CC) $(FLAGS) -c tree.c

clean:
	rm -f *.o boo $(EXEC)
		