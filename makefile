
autograde.o: autograde.c gradefile.h compile.h
	gcc -c autograde.c

PA1: autograde.o PA1.o
	gcc autograde.o PA1.o -o autograde

PA2: autograde.o PA2.o
	gcc autograde.o PA2.o -o autograde

PA3: autograde.o PA3.o
	gcc autograde.o PA3.o -o autograde

PA1.o: PA1.c gradefile.h compile.h
	gcc -c PA1.c

PA2.o: PA2.c gradefile.h compile.h
	gcc -c PA2.c

PA3.o: PA3.c gradefile.h compile.h
	gcc -c PA3.c




clean:
	rm *.o

install:
	cp autograde /usr/local/bin/autograde