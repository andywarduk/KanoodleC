
all: Kanoodle

Kanoodle: Shapes.o Board.o ShapeFit.o SolutionMgr.o main.o 
	gcc -Wall -Wextra $^ -o $@

%.o: %.c
	gcc -c $^ -Wall -Wextra -fPIC -fno-inline -g -O4 -o $@

clean:
	rm -f *.o *~ core.*
