#include <iostream> 
#include <fstream>
#include <cmath>
#include "random.h"

using namespace std; 



/******************************************************************************************************************/
/******************************************************************************************************************/

double geometric_factor(double t,double v, double mu, double sigma, Random * rnd); //ritorna il fattore moltiplicativo F(t_i-t_(i-1)), per cui S(t_i)=F*S(t_(i-1))


double get_S_T(double S_0,double T,int R,double mu,double sigma, Random *rnd); //Trova S(T) discretizzando in R sottointervalli



double call(double S,double K); //ritorna il profitto per una call

double put(double S, double K);//ritorna il profitto per una put


double get_price(double (*type)(double,double), double S, double r, double K, double T); //trova il prezzo di un'opzione (call o put), "scontando" l'interesse



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
    save_results << "price (call;dir), error(call;dir), price (call;dir), error(call;dir) "  ;
    save_results << "price (call;dis), error(call;dis), price (call;dis), error(call;dis) "  << endl;


    /*********************************************************************************/


     //parametri

    int M=100000; //numero di punti per il sampling 
    int N=100; //numero di blocchi
    int L=M/N; //numero di lanci per blocco


    double S_0=100; //asset price al tempo t=0
    double T=1; //delivery date
    double K=100; //strike price
    double r=0.1; //risk-free interest
    double sigma=0.25; //volatilità
    int R=100; //numero sottointervalli per discretizzazione

    /**********************************************************************************/


    //variabili di servizio


    //
    double S;

    //somma blocco 
    double block_sum_call_dir; 
    double block_sum_put_dir;
    double block_sum_call_dis; 
    double block_sum_put_dis;

    //stima prezzo
    double price_call_dir=0; 
    double price_put_dir=0;
    double price_call_dis=0; 
    double price_put_dis=0; 


    //media dei quadrati, per stima varianza
    double squares_avg_call_dir=0;
    double squares_avg_put_dir=0; 
    double squares_avg_call_dis=0; 
    double squares_avg_put_dis=0; 

    //deviazione standard
    double error_call_dir; 
    double error_put_dir; 
    double error_call_dis; 
    double error_put_dis; 

    /**********************************************************************************/


    for (int n=0; n < N; n++){ //itera sui blocchi
        block_sum_call_dir=block_sum_put_dir=block_sum_call_dis=block_sum_put_dis=0; //riporta a zero la somma del blocco
        for(int l=0; l < L; l++){ //itera sui lanci per blocco
            
            S= S_0*geometric_factor(T,0,r,sigma,&rnd); // prezzo mercato- metodo diretto S(T)=S(0)F(T,0)
            
            block_sum_call_dir+=get_price(call,S,r,K,T); //prezzo opzione call
            block_sum_put_dir+=get_price(put,S,r,K,T); //prezzo opzione put

            S=get_S_T(S_0,T,R,r,sigma,&rnd); // prezzo mercato - con discretizzazione (R sottointervalli)
            block_sum_call_dis+=get_price(call,S,r,K,T); //prezzo opzione call
            block_sum_put_dis+=get_price(put,S,r,K,T); // prezzo opzione put


        }

        //media del blocco (stima di I)
        block_sum_call_dir/=L; 
        block_sum_put_dir/=L; 
        block_sum_call_dis/=L; 
        block_sum_put_dis/=L; 

        // Aggiunge la media del nuovo blocco alla media cumulativa
        price_call_dir=(n*price_call_dir +block_sum_call_dir)/(n+1); 
        price_put_dir=(n*price_put_dir +block_sum_put_dir)/(n+1); 
        price_call_dis=(n*price_call_dis +block_sum_call_dis)/(n+1); 
        price_put_dis=(n*price_put_dis +block_sum_put_dis)/(n+1); 
        
        // Aggiunge il quadrato della media del nuovo blocco della alla media cumulativa dei quadrati (per calcolo varianza)
        squares_avg_call_dir=(n*squares_avg_call_dir +block_sum_call_dir*block_sum_call_dir)/(n+1); 
        squares_avg_put_dir=(n*squares_avg_put_dir +block_sum_put_dir*block_sum_put_dir)/(n+1); 
        squares_avg_call_dis=(n*squares_avg_call_dis +block_sum_call_dis*block_sum_call_dis)/(n+1); 
        squares_avg_put_dis=(n*squares_avg_put_dis +block_sum_put_dis*block_sum_put_dis)/(n+1); 

        
        if (n==0){ 
            error_call_dir=0;
            error_put_dir=0;
            error_call_dis=0;
            error_put_dis=0;

        }else{
            //calcola l'incertezza cumulativa al blocco n
            error_call_dir=sqrt((squares_avg_call_dir-price_call_dir*price_call_dir)/n); 
            error_put_dir=sqrt((squares_avg_put_dir-price_put_dir*price_put_dir)/n); 
            error_call_dis=sqrt((squares_avg_call_dis-price_call_dis*price_call_dis)/n); 
            error_put_dis=sqrt((squares_avg_put_dis-price_put_dis*price_put_dis)/n);      
        }
        
        //stampa i risultati su file
        save_results << price_call_dir << "," << error_call_dir << "," << price_put_dir << "," << error_put_dir << ",";
        save_results << price_call_dis << "," << error_call_dis << "," << price_put_dis << "," << error_put_dis << endl;


    }


    save_results.close(); 


    return 0; 
}


/******************************************************************************************************************/
/******************************************************************************************************************/


double geometric_factor(double t,double v, double mu, double sigma, Random * rnd){ //ritorna il fattore moltiplicativo F(t_i-t_(i-1)), per cui S(t_i)=F*S(t_(i-1))
    double Dt=t-v; 
    double z=rnd->Gauss(0,1); 
    double factor=exp((mu-0.5*pow(sigma,2))*Dt+sigma*z*sqrt(Dt));

    return factor;
}



double get_S_T(double S_0,double T,int R,double mu,double sigma, Random *rnd){ //Trova S(T) discretizzando in R sottointervalli
        double S=S_0; 
        double Dt=T/R;
        double t=0;
        for (int i=0; i < R ; i++){
            S*=geometric_factor(t+Dt,t,mu,sigma,rnd);
            t+=Dt; 
        }
        return S; 

}


double call(double S,double K){ //ritorna il profitto per una call
    return max(0.,S-K);
}

double put(double S, double K){//ritorna il profitto per una put
    return max(0.,K-S); 
}


double get_price(double (*type)(double,double), double S, double r, double K, double T){ //trova il prezzo di un'opzione (call o put), "scontando" l'interesse
    double profit=type(S,K); 
    return profit*exp(-r*T); 
}




