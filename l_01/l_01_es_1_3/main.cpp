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

    int M=100; //sotto-intervalli
    int n=10000; //quantità di numeri random estratta per test
    int R=10000;  //numero test (maggiore di quanto richiesto (R=100) per poter impostare un confronto con la distribuzione teorica)


    /**********************************************************************************/


    //Gestione output risultati

    ofstream save_results; 
    save_results.open("results.csv");

    if (!save_results.is_open()){
        cout << "Output file could not be opened: aborting." << endl; 
        return 0; 
    }

    save_results << "chi2"    << endl; 

    /**********************************************************************************/

    //variabili di servizio

    double squares_sum; 
    int index;
    int counter[M]; //contatore per i sottointervalli
   

    /**********************************************************************************/

    
   
    for (int r=0;r<R;r++){ //per R volte
        squares_sum=0; 
        //setta il contatore a zero
        for (int m=0; m <M; m++){
            counter[m]=0; 
        }

        for(int i=0;i<n;i++){ //per n estrazioni
            index=int(rnd.Rannyu()*M); //controlla in quale intervallo cade il numero estratto...
            counter[index]++; //... e incrementa il rispettivo contatore
        }
        
        for(int m=0;m<M;m++){ //iterando su tutti gli intervalli
            squares_sum+=pow(counter[m]-n/M,2); //somma il quadrato dello scarto dal valore atteso
        }
        save_results << squares_sum/(n/M) << endl; //salva il valore di chi quadro per il test r-esimo

        
    }
    
    save_results.close(); 

    return 0; 
}