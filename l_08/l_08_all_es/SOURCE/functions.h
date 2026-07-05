#ifndef ___functions___
#define ___functions___

#include <armadillo>
#include <functional>

class Random; 



struct Result{
   double value;
   double error; 
};


//DATA BLOCKING 

Result Data_blocking(unsigned int  N_blocks, unsigned int N_steps, //numero di blocchi e di step
   arma::vec & x, //vettore di variabili stocastiche di cui calcolare il valor medio(devono essere N_blocks*N_steps)
   bool print_results=false, const  char * filename="results.dat"//opzionale, per stampa risultati
); 


#endif // ___functions___