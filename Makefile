.default: all

all: pc dp

clean:
	rm -f pc dp *.o

pc: producerConsumer.o
	g++ -Wall -Werror -pedantic -std=c++17 -g -O -o $@ -pthread $^

dp: diningPhilosophers.o
	g++ -Wall -Werror -pedantic -std=c++17 -g -O -o $@ -pthread $^


%.o: %.cpp
	g++ -Wall -Werror -pedantic -std=c++17 -g -O -c $^