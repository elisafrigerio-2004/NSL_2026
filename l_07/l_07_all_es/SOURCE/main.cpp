#include <iostream>
#include <string>   
#include <stdexcept>  

#include "system.h"

using namespace std;

int main (int argc, char *argv[]){


  if(argc!=2){
    cout << "Usage: " << argv[0] <<  " <equilibration steps>" << endl; 
    return 1;
  }

  int N_e;


  try{
    N_e=stoi(argv[1]);
  }
  catch (const std::invalid_argument& e) {
    cout << "Warning: " << argv[1] << " is not a valid number of equilibration steps. Using default (0)." <<endl; 
    N_e=0;
  } 
  if(N_e<0){
    cout << "Warning: " << N_e << " is not a valid number of equilibration steps. Using default (0)." <<endl; 
    N_e=0;
  }

  cout << "Using "<< N_e  << " equilibration steps."<< endl; 

  //crea l'oggetto sistema e lo inizializza
  System SYS;
  SYS.initialize();

  if(N_e > 0){
    for(int j=0; j < N_e; j++){ //fase di equilibrazione
      SYS.step(); //evolve il sistema di un passo
    }
  }


  //leggendo il file Properties, prepara il sistema per il calcolo delle proprietà indicate.
  SYS.initialize_properties();

  SYS.block_reset(0); //resetta le variabili di servizio del blocco
  
  for(int i=0; i < SYS.get_nbl(); i++){ //itera sui blocchi

    for(int j=0; j < SYS.get_nsteps(); j++){ //itera sugli step in un blocco
      SYS.step(); //evolve il sistema di un passo
      SYS.measure();//effettua le misure

    }
    SYS.averages(i+1); //calcola le medie cumulative al blocco i+1 e le stampa su file 
    SYS.block_reset(i+1); //resetta le variabili di servizio del blocco
  }
  SYS.finalize();

  return 0;
}