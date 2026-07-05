RPATH=../../../Random_generator/
GPATH=./GENETIC/
CC = g++
ARMALIBS= -larmadillo -llapack -lblas
CFLAGS = -Wall -O3 --std=c++17 -I $(RPATH) -I $(GPATH)




main.exe : main.o random.o globe.o path.o population.o
	$(CC) random.o main.o globe.o path.o population.o  -o main.exe $(ARMALIBS)


main.o : main.cpp $(GPATH)population.h $(GPATH)path.h $(GPATH)globe.h $(RPATH)random.h
	$(CC) -c main.cpp -o main.o $(CFLAGS) 

globe.o: $(GPATH)globe.cpp $(GPATH)globe.h $(RPATH)random.h
	$(CC) -c $(GPATH)globe.cpp -o globe.o $(CFLAGS)

path.o: $(GPATH)path.cpp $(GPATH)path.h $(GPATH)globe.h $(RPATH)random.h
	$(CC) -c $(GPATH)path.cpp -o path.o $(CFLAGS)

population.o: $(GPATH)population.cpp $(GPATH)population.h $(GPATH)path.h $(GPATH)globe.h $(RPATH)random.h
	$(CC) -c $(GPATH)population.cpp -o population.o $(CFLAGS)




random.o : $(RPATH)random.cpp $(RPATH)random.h
	$(CC) -c $(RPATH)random.cpp -o random.o $(CFLAGS)


clean :
	rm *.o *.exe 
clean_output :
	rm ../OUTPUT/*.csv
	