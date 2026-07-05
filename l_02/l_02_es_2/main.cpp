#include <iostream> 
#include <fstream>
#include <cmath>
#include "random.h"

using namespace std; 



/******************************************************************************************************************/
/******************************************************************************************************************/




void step_on_grid(Random *rnd,double *pos_grid );//esegue un passo del Random Walk 3D sulla griglia

void step_in_continuos(Random* rnd,double * pos_cont); //esegue un passo del Random Walk 3D nel continuo 

double square_module(double * position);// calcola il modulo quadro di un vettore posizione




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
    save_results << "value (grid),error (grid),value(cont), error(cont)"  << endl;

    /**********************************************************************************/


    //parametri

    int M=10000; //numero di randomwalk
    int N=100; //numero di blocchi
    int L=M/N; //numero di lanci per blocco
    int R=100; //numero di step per random walk
    /*********************************************************************************/

 


    //variabili di servizio

    //posizione del random walk (x,y,z)
    double pos_grid[3]; 
    double pos_cont[3]; 

    //somma del blocco (per ogni passo i =1,...,R)
    double  block_sum_grid[R];
    double  block_sum_cont [R];

    //somma cumulativa (per ogni passo i =1,...,R)
    double cum_sum_grid[R]={}; 
    double cum_sum_cont[R]={}; 

    //somma cumulativa dei quadrati (per stima varianza) (per ogni passo i =1,...,R)
    double cum_squares_sum_grid[R]={};
    double cum_squares_sum_cont[R]={}; 

    //errore 
    double error_grid;
    double error_cont; 

 

    /**********************************************************************************/


    for (int n=0; n < N; n++){ //itera sui blocchi

        for(int i=0;i<R;i++){ //iterando sui passi del RW, pone a zero la somma del blocco
            block_sum_grid[i]=block_sum_cont[i]=0;
        }
     
        for(int l=0; l < L; l++){ //itera sui RW generati per blocco
            //ogni RW parte dall'origine:
            pos_grid[0]=pos_grid[1]=pos_grid[2]=0; 
            pos_cont[0]=pos_cont[1]=pos_cont[2]=0;

            for(int i=0;i<R;i++){//iterando sui passi del RW

                step_on_grid(&rnd,pos_grid); //esegue uno step sulla griglia
                step_in_continuos(&rnd,pos_cont);//esegue uno step nel continuo
                
                //aggiunge alla somma del blocco relativa al passo i il nuovo valore
                block_sum_grid[i]+=square_module(pos_grid);
                block_sum_cont[i]+=square_module(pos_cont);
              
             
            }
        
        }

       
        for(int i=0;i<R;i++){ //iterando sui passi

            //calcola la radice quadrata della media del blocco (quantità da calcolare)
            block_sum_grid[i]=sqrt(block_sum_grid[i]/L); 
            block_sum_cont[i]=sqrt(block_sum_cont[i]/L); 
            
            // Aggiunge la quantità calcolata per il nuovo blocco alla media cumulativa
            cum_sum_grid[i]=(n*cum_sum_grid[i] +block_sum_grid[i])/(n+1);
            cum_sum_cont[i]=(n*cum_sum_cont[i] +block_sum_cont[i])/(n+1);

            // Aggiunge il quadrato della quantità calcolata per il nuovo blocco della alla media dei quadrati (per calcolo varianza)
            cum_squares_sum_grid[i]=(n*cum_squares_sum_grid[i]+block_sum_grid[i]*block_sum_grid[i])/(n+1);
            cum_squares_sum_cont[i]=(n*cum_squares_sum_cont[i]+block_sum_cont[i]*block_sum_cont[i])/(n+1); 
        }

    }
    for(int i=0; i<R; i++){ //iterando sui passi

        //calcola l'incertezza al blocco N 
        error_grid=sqrt((cum_squares_sum_grid[i]-cum_sum_grid[i]*cum_sum_grid[i])/N); 
        error_cont=sqrt((cum_squares_sum_cont[i]-cum_sum_cont[i]*cum_sum_cont[i])/N); 
        
    
        //stampa la media al blocco N relativa al passo i, e il rispettivo errore
        save_results << cum_sum_grid[i] << ","<< error_grid <<"," << cum_sum_cont[i] << "," << error_cont << endl;
    }
        


    save_results.close(); 

    return 0; 
}





/******************************************************************************************************************/
/******************************************************************************************************************/




void step_on_grid(Random *rnd,double *pos_grid ){

    int which_step=int(rnd->Rannyu(0,6)); //estrae uniformemente un intero tra 0 e 5 compresi

    switch(which_step){
        case 0: //fa un passo avanti sull'asse delle x
            pos_grid[0]+=1; 
            return; 
        case 1: //fa un passo indietro sull'asse delle x
            pos_grid[0]-=1; 
            return; 
        case 2: //fa un passo avanti sull'asse delle y
            pos_grid[1]+=1; 
            return; 
        case 3: //fa un passo indietro sull'asse delle y
            pos_grid[1]-=1; 
            return; 
        case 4: //fa un passo avanti sull'asse delle z
            pos_grid[2]+=1; 
            return; 
        case 5: //fa un passo indietro sull'asse delle z
            pos_grid[2]-=1; 
            return; 
    }

}



void step_in_continuos(Random* rnd,double * pos_cont){
    double theta=rnd->Rannyu(0,M_PI);//estrae theta con probabilita' uniforme
    double phi=rnd->Rannyu(0,2*M_PI);//estrae phi con probabilita' uniforme
    pos_cont[0]+=sin(theta)*cos(phi);//aggiorna la posizione sull'asse x
    pos_cont[1]+=sin(theta)*sin(phi);//aggiorna la posizione sull'asse y
    pos_cont[2]+=cos(theta);//aggiorna la posizione sull'asse y

    return;

}

double square_module(double * position){
    return position[0]*position[0]+position[1]*position[1]+position[2]*position[2]; 
}


