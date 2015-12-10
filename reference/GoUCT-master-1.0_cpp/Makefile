CFLAGS=-Wall -Wextra

go: main

main: Board.o Node.o main.o
	$(CXX) $(CFLAGS) -o go Board.o Node.o main.o

main.o: main.cpp
	$(CXX) -c $(CFLAGS) main.cpp

Board.o: Board.cpp
	$(CXX) -c $(CFLAGS) Board.cpp

Node.o: Node.cpp
	$(CXX) -c $(CFLAGS) Node.cpp

.PHONY: clean
clean:
	rm -f go *.o *~
