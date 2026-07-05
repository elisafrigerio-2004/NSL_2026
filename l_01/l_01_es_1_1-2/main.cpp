#include <iostream> 
#include <fstream>
#include <cmath>
#include "random.h"

using namespace std;

int main (int argc, char *argv[]){

    //setup generatore
    Random rnd;
    int seed[4];
    int p1, p2;
    ifstream Primes("Primes");
    if (Primes.is_open()){
        Primes >> p1 >> p2 ;
    } else cerr << "PROBLEM: Unable to open Primes" << endl;
    Primes.close();

    ifstream input("seed.in");
    string property;
    if (input.is_open()){
        while ( !input.eof() ){
            input >> property;
            if( property == "RANDOMSEED" ){
            input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
            rnd.SetRandom(seed,p1,p2);
            }
        }
        input.close();
    } else cerr << "PROBLEM: Unable to open seed.in" << endl;

    /**********************************************************************************/

    //Parametri 

    int M=100000; //steps
    int N=100; //numero blocchi
    int L=M/N;//grandezza blocco 

    /**********************************************************************************/


    //Gestione output risultati

    ofstream save_results; 
    save_results.open("results.csv");

    if (!save_results.is_open()){
        cout << "Output file could not be opened: aborting." << endl; 
        return 0; 
    }

    save_results << "cum_avg_1,error_1,cum_avg_2,error_2"    << endl;


    /**********************************************************************************/

    //variabili di servizio

    double rnd_num;

    //esercizio 1.1
    double block_sum_1; 
    double cum_avg_1=0; 
    double sqrd_cum_avg_1=0;
    double error_1=0;


    //esercizio 1.2
    double block_sum_2; 
    double cum_avg_2=0; 
    double sqrd_cum_avg_2=0;
    double error_2=0;
    
    /**********************************************************************************/

  
    for (int n=0;n<N;n++){//per ogni blocco
        block_sum_1=0; 
        block_sum_2=0; 
        for(int i=0;i<L;i++){ //per numero step /numero blocchi volte

            rnd_num=rnd.Rannyu(); //estrazione numero

            block_sum_1+= rnd_num; //es 1.1 : calcolo dell'integrale di r 
            block_sum_2+= pow(rnd_num-0.5,2); // es 1.2 : calcolo dell'integrale di (r-1/2)^2

        }
        block_sum_1/=L; //Calcola la media del blocco
        block_sum_2/=L; //Calcola la media del blocco


    
        cum_avg_1= (n*cum_avg_1 + block_sum_1)/(n+1); //calcola la media cumulativa delle medie dei blocchi fino al blocco n
        cum_avg_2= (n*cum_avg_2 + block_sum_2)/(n+1); //calcola la media cumulativa delle medie dei blocchi fino al blocco n
        

        sqrd_cum_avg_1=(n*sqrd_cum_avg_1 + block_sum_1*block_sum_1)/(n+1); //calcola la media dei quadrati delle medie dei blocchi
        sqrd_cum_avg_2=(n*sqrd_cum_avg_2 + block_sum_2*block_sum_2)/(n+1); //calcola la media dei quadrati delle medie dei blocchi

        if (n==0){ 
            error_1=0;
            error_2=0;
        }else{
            error_1=sqrt((sqrd_cum_avg_1-cum_avg_1*cum_avg_1)/n); //calcola l'incertezza cumulativa al blocco n
            error_2=sqrt((sqrd_cum_avg_2-cum_avg_2*cum_avg_2)/n); //calcola l'incertezza cumulativa al blocco n

        }
        
        save_results << cum_avg_1<< "," << error_1 <<", " << cum_avg_2<< "," << error_2 << endl; //stampa risultati su file
      
    }

    save_results.close(); 
    return 0; 
}