CC=g++
CFLAGS=-std=c++11 -Wall -Wextra -pedantic -g

# csim: main.o utils.o 
# 	$(CC) -o csim main.o utils.o 

# main.o: main.cpp utils.h 
# 	$(CC) $(CFLAGS) -c main.cpp

# utils.o: utils.cpp utils.h
# 	$(CC) $(CFLAGS) -c utils.cpp

csim: main.o utils.o block.o cacheset.o cache.o  globalcounter.o
	$(CC) -o csim main.o utils.o block.o cacheset.o cache.o globalcounter.o

main.o: main.cpp utils.h block.h cacheset.h cache.h
	$(CC) $(CFLAGS) -c main.cpp

utils.o: utils.cpp utils.h
	$(CC) $(CFLAGS) -c utils.cpp

block.o: block.cpp block.h
	$(CC) $(CFLAGS) -c block.cpp

globalcounter.o: globalcounter.cpp 	globalcounter.h
	$(CC) $(CFLAGS) -c globalcounter.cpp

cacheset.o: cacheset.cpp cacheset.h block.h globalcounter.h
	$(CC) $(CFLAGS) -c cacheset.cpp

cache.o: cache.cpp cache.h cacheset.h globalcounter.h
	$(CC) $(CFLAGS) -c cache.cpp

clean :
	rm -f *.o csim solution.zip

zip :
	rm -f solution.zip
	zip -9r solution.zip  Makefile *.h *.cpp README.txt exp_output.txt exp.py
