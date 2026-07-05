#include <iostream> 
#include <fstream>
#include <cmath>
#include "random.h"

using namespace std; 




/******************************************************************************************************************/
/******************************************************************************************************************/




double calc_unif(Random *rnd); //ritorna f(x) per metodo Montecarlo con distribuzione uniforme 

double calc_import_sampl(Random* rnd); //ritorna f(x)/d(x) per metodo Montecarlo con importance sampling 



/******************************************************************************************************************/
/******************************************************************************************************************/




int main(){


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


    //gestione output
    ofstream save_results; 
    save_results.open("results.csv");
    if (!save_results.is_open()){
        cout << "Output file could not be opened: aborting." << endl; 
        return 0; 
    }
    save_results << "I (unif),error (unif),I(i.s.),error(i.s.)"  << endl;

    /*********************************************************************************/


     //parametri

    int M=100000; //numero di punti per il sampling 
    int N=100; //numero di blocchi
    int L=M/N; //numero di lanci per blocco


    /**********************************************************************************/


    //variabili di servizio

    //somma blocco 
    double block_sum_u; 
    double block_sum_is;

    //stima integrale
    double I_u=0; 
    double I_is=0; 

    //media dei quadrati, per stima varianza
    double squares_avg_u=0; 
    double squares_avg_is=0; 

    //deviazione standard
    double error_u; 
    double error_is; 

 

    /**********************************************************************************/


    for (int n=0; n < N; n++){ //itera sui blocchi
        block_sum_u=block_sum_is=0; //riporta a zero la somma del blocco
        for(int l=0; l < L; l++){ //itera sui lanci per blocco
            block_sum_u+=calc_unif(&rnd); //aggiunge f(x_i) alla somma per il calcolo di I (distr. uniforme)
            block_sum_is+=calc_import_sampl(&rnd); //aggiunge f(x_i) alla somma per il calcolo di I (importance sampling)

        }

        //media del blocco (stima di I)
        block_sum_u/=L; 
        block_sum_is/=L; 

        // Aggiunge la media del nuovo blocco alla media cumulativa
        I_u=(n*I_u +block_sum_u)/(n+1); 
        I_is=(n*I_is +block_sum_is)/(n+1); 

        // Aggiunge il quadrato della media del nuovo blocco della alla media cumulativa dei quadrati (per calcolo varianza)
        squares_avg_u=(n*squares_avg_u +block_sum_u*block_sum_u)/(n+1); 
        squares_avg_is=(n*squares_avg_is +block_sum_is*block_sum_is)/(n+1); 
        
        if (n==0){
            error_u=0;
            error_is=0;

        }else{
            //calcola l'incertezza cumulativa al blocco n
            error_u=sqrt((squares_avg_u-I_u*I_u)/n); 
            error_is=sqrt((squares_avg_is-I_is*I_is)/n); 
            
        }
        
        //moltiplica per la costante e stampa i risultati su file
        save_results <<I_u*M_PI/2<< ","  << error_u << "," << I_is*M_PI/2<< ","  << error_is << endl; //l'integeale è la media per la costante moltipicativa della funzione
            
       

    }


    save_results.close(); 


    return 0; 
}


/******************************************************************************************************************/
/******************************************************************************************************************/




double calc_unif(Random *rnd){
    double x=rnd->Rannyu(); //estrae x sampled da una distribuzione uniforme su [0,1)
    return cos(M_PI*x*0.5); //calcola f(x)
}

double calc_import_sampl(Random* rnd){
    double x=1-sqrt(1-rnd->Rannyu()); //genera x sampled da p(x)=2(1-x) su [0,1)
    return cos(M_PI*x*0.5)/(2*(1-x)); //calcola f(x)/p(x) e lo restituisce

}
