#include <iostream>
#include "system.h"

using namespace std;

int main (int argc, char *argv[]){

  //crea l'oggetto sistema e lo inizializza
  System SYS;
  SYS.initialize();

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