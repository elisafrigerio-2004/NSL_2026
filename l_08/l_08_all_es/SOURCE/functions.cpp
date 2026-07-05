#include <cmath>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <exception>

#include "random.h"

#include "functions.h"


using namespace std; 






//DATA BLOCKING


Result Data_blocking(unsigned int  N_blocks, unsigned int N_steps,arma::vec & x,bool print_results, const  char * filename){

    ofstream print;

    if(print_results){

        print.open(filename); 

        if(print.is_open()){
            print << "Blk" << setw(12) << "Blk_avg" << setw(12) << "Cum_avg" << setw(12) << "Error" << endl; 

        }
    
        
    }


    if(N_blocks*N_steps!= x.n_elem){throw std::runtime_error("Array dimension does not match entered N_blocks*N_steps"); }


    double block_avg; 
    double cum_sum=0; 
    double cum_sum_sqrd=0; 

    double error; 



    for(unsigned int n=0; n< N_blocks; n++){
        
        
        block_avg=0; 

        for (unsigned int i=0; i < N_steps; i++){ block_avg+=x(n*N_steps +i);} //aggiunge il valore n*i esimo

        block_avg/=N_steps; //divide la somma per il numero di step per calcolare la media
        cum_sum+=block_avg;//la aggiunge alla somma cumulativa
        cum_sum_sqrd+=block_avg*block_avg; //aggiunge il quadrato alla somma cumulativa dei quadrati


        if(n==0){
            error=0; //non si può calcolare l'errore
        }else{
            error=sqrt(fabs(cum_sum_sqrd/(n+1) -pow(cum_sum/(n+1),2))/n);
        }


        if(print_results && print.is_open()){
            print << n << setw(12) << block_avg<< setw(12) << cum_sum/(n+1) << setw(12) << error <<"\n"; 

        }
    }

    
	Result result; 

	result.value=cum_sum/N_blocks; //assegna al risultato la media globale all'ultimo blocco

	result.error=error; //assegna al risultato l'errore  all'ultimo blocco


    if(print.is_open()){print.close();}

	return result;

}
