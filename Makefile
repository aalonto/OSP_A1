.default: all

.default: all

all: a1

clean:
	rm -f producerConsumer *.o

a1: producerConsumer.o
	g++ -Wall -Werror -pedantic -std=c++17 -g -O -o $@ -pthread $^

%.o: %.cpp
	g++ -Wall -Werror -pedantic -std=c++17 -g -O -c $^