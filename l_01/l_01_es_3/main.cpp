#include <iostream> 
#include <fstream>
#include <cmath>
#include <string>
#include <iomanip>
#include "random.h"

using namespace std;



/******************************************************************************************************************/
/******************************************************************************************************************/


double random_cos(Random * rnd); // ritorna il coseno di un angolo con probabilità uniforme
bool lancio(Random * rnd,double d, double l); //simulazione di un lancio del bastoncino: ritorna True se attravera una linea, False altrimenti



/******************************************************************************************************************/
/******************************************************************************************************************/



 
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


    //parametri
    

    double d=3; //distanza linee 
    double l=2.9 ;//lunghezza ago
    int N=100; //blocchi
    int M=10000; // lanci per blocco


    
    /**********************************************************************************/

    //gestione output
    ofstream save_results; 
    save_results.open("results.csv");
    if (!save_results.is_open()){
        cout << "Output file could not be opened: aborting." << endl; 
        return 0; 
    }
    save_results << "cum_avg,error"  << endl;


    /**********************************************************************************/
    

    //variabili di servizio
    int counter; 
    double pi_estimate; 
    double cum_avg=0; 
    double sqrd_cum_avg=0; 
    double error; 

    /**********************************************************************************/


    
    for(int n=0;n <N; n++){//per ogni blocco di lanci
        counter=0;
        for(int r=0; r < M; r++){//per ogni lancio nel blocco
            if(lancio(&rnd,d,l) ) {//lancio l'ago, se cade su una linea...
                counter++; //... incremento il contatore
            }

        }
        pi_estimate=2*l/d*double(M)/double(counter); //stima di pi per l'n-esimo blocco
        cum_avg= (n*cum_avg + pi_estimate)/(n+1); //calcola la media cumulativa delle stime dei blocchi fino al blocco n

        sqrd_cum_avg=(n*sqrd_cum_avg + pi_estimate*pi_estimate)/(n+1); //calcola la media dei quadrati delle stime dei blocchi
        if (n==0){
            error=0;
        }else{
            error=sqrt((sqrd_cum_avg-cum_avg*cum_avg)/n); //calcola l'incertezza cumulativa al blocco n
        }
        

        save_results <<cum_avg<< ","  << error << endl;
      
    }


    save_results.close(); 


    return 0; 
}


/******************************************************************************************************************/
/******************************************************************************************************************/



double random_cos(Random * rnd){ // ritorna il coseno di un angolo con probabilità uniforme 
    double x= rnd-> Rannyu(-1,1);
    double y= rnd-> Rannyu(-1,1);
    
    if (x*x+y*y>= 1){// se il punto cade fuori dal cerchio, lo rigetta, ed avvia un nuovo calcolo
        return random_cos(rnd); 
    }
    return x/sqrt(x*x+y*y); //ritorna il coseno dell'angolo individuato dalla semiretta che congiunge il punto all'origine
}

bool lancio(Random * rnd,double d, double l){ 

    double x1=rnd->Rannyu(0,d);//ascissa del primo estremo dell'ago 
    double x2=x1+l*random_cos(rnd); // ascissa del secondo estremo dell'ago (l per il coseno di un angolo estratto da una distribuzione di probabilità uniforme)
    return x2 < 0 || x2 >d; //ritorna True se x2 cade fuori dall'intervallo [0,d] (attraversa la linea)
}

 