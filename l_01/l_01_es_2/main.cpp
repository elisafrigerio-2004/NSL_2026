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
    int n=10000; //numero test
    int N[4]={1,2,10,100};  //grandezza campione

    /**********************************************************************************/


   
    //gestione output

    ofstream save_results1,save_results2,save_results3;

    save_results1.open("res_uni_dist.csv");
    save_results2.open("res_exp_dist.csv");
    save_results3.open("res_lor_dist.csv");

    if (!save_results1.is_open() || !save_results2.is_open() || !save_results3.is_open()){
        cout << "Output files could not be opened. Aborting." <<endl; 
        return 0; 
    }


    save_results1 << "N=1,N=2,N=10,N=100"    << endl;
    save_results2 << "N=1,N=2,N=10,N=100"    << endl;
    save_results3 << "N=1,N=2,N=10,N=100"    << endl;


   

    /**********************************************************************************/



    //variabili di servizio 

    double sum_uni,sum_exp,sum_lor;


    /**********************************************************************************/

    

    for (int i=0;i<n;i++){//per n volte 
        sum_uni=sum_exp=sum_lor=0; //setta le somme a zero
        int m=1; 
        for (m=1; m <= N[3]; m++){//estrae 100 elementi

            //estrae un numero dalla distribuzione * e lo aggiunge alla somma
            sum_uni+=rnd.Rannyu(); //*:uniforme
            sum_exp+=rnd.Exponential(1);//*:esponenziale
            sum_lor+=rnd.Lorentian(1,0);//*:lorentziana


            if(m==N[0] || m==N[1] || m==N[2] ){ //se m= 1,2,10 : media di 1,2,10 elementi
                save_results1 << sum_uni/m << "," ; //salva la media per la distribuzione uniforme
                save_results2 << sum_exp/m << "," ; //salva la media per la distribuzione esponenziale
                save_results3 << sum_lor/m << "," ; //salva la media per la distribuzione lorentziana


            }
            
        }
        //m=100 : media di 100 elementi
        save_results1 << sum_uni/m << endl; //salva la media per la distribuzione uniforme
        save_results2 << sum_exp/m << endl; //salva la media per la distribuzione esponenziale
        save_results3 << sum_lor/m << endl ; //salva la media per la distribuzione lorentziana
        
    }
    
    save_results1.close(); 
    save_results2.close(); 
    save_results3.close(); 



    return 0; 
}