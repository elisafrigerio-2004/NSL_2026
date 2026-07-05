RPATH=../../../Random_generator/
CC = g++
ARMALIBS= -larmadillo -llapack -lblas
CFLAGS = -Wall -O3 --std=c++17 -I $(RPATH) 




main.exe : main.o random.o system.o functions.o
	$(CC) random.o main.o system.o functions.o -o main.exe $(ARMALIBS)


main.o : main.cpp 
	$(CC) -c main.cpp -o main.o $(CFLAGS) 

system.o: system.cpp system.h 
	$(CC) -c system.cpp -o system.o $(CFLAGS)

functions.o: functions.cpp functions.h
	$(CC) -c functions.cpp -o functions.o $(CFLAGS)



random.o : $(RPATH)random.cpp $(RPATH)random.h
	$(CC) -c $(RPATH)random.cpp -o random.o $(CFLAGS)



clean :
	rm *.o main.exe 

clean_output : 
	rm ../OUTPUT/*.csv 
	rm ../OUTPUT/*.dat 
	rm ../OUTPUT/DETAIL/*.csv
