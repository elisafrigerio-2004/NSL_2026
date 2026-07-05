RPATH= ../../../Random_generator/
CC = g++
CFLAGS = -O3 --std=c++14 -I$(RPATH) 
AFLAGS = -larmadillo


main.exe :  main.o  system.o particle.o random.o
	$(CC) system.o particle.o random.o main.o -o main.exe $(AFLAGS)



main.o : main.cpp system.h particle.h $(RPATH)random.h 
	$(CC) -c main.cpp -o main.o $(CFLAGS)


system.o : system.cpp system.h $(RPATH)random.h particle.h
	$(CC) -c system.cpp -o system.o $(CFLAGS)

particle.o : particle.cpp particle.h $(RPATH)random.h 
	$(CC) -c particle.cpp -o particle.o $(CFLAGS)



random.o : $(RPATH)random.cpp $(RPATH)random.h
	$(CC) -c $(RPATH)random.cpp -o random.o $(CFLAGS)

clean :
	rm *.o main.exe
	
clean_output:
	rm ../OUTPUT/*.*
	rm ../OUTPUT/CONFIG/*.*  