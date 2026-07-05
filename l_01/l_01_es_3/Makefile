RPATH=../../Random_generator/
CC = g++
CFLAGS = -Wall -O3 --std=c++11 -I $(RPATH)


main.exe : main.o random.o
	$(CC) random.o main.o -o main.exe

main.o : main.cpp
	$(CC) -c main.cpp -o main.o $(CFLAGS)
	


random.o : $(RPATH)random.cpp $(RPATH)random.h
	$(CC) -c $(RPATH)random.cpp -o random.o $(CFLAGS)



clean :
	rm *.o main.exe 

clean_output: 
	rm *.csv
