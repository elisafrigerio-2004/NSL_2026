#include "globe.h"

#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <exception>





using namespace std; 
using namespace arma;

Globe::Globe(int N_cities){



    if(N_cities <=0){
        throw runtime_error("Not a valid number of cities"); 
    }
 

    _N_cities=N_cities; 

    _cities.reserve(N_cities); 

    _distances.set_size(N_cities,N_cities);


    in_use=false; 

    
}




Globe::Globe(const char * positions_file ){

    ifstream filein; 

   
   
    filein.open(positions_file); 

    if(!filein.is_open() || filein.peek() == EOF){ //se non trova il file, o è vuoto dà errore
        throw runtime_error("Positions file not found or empty. Aborting."); 
    }


    City city; 

    int counter =0; //conta quante città sono state caricate

    while(filein >> city.x  >> city.y){

        _cities.push_back(city); 
        counter++; 

    }



   _N_cities=counter; 

   if(static_cast<long unsigned int>(_N_cities) != size(_cities)){ //controllo di sicurezza
   throw runtime_error("Cities number does not match vector size. Aborting.");
   
   }

   
    _distances.zeros(_N_cities,_N_cities);

    in_use=false;


    
}








void Globe::Fill_Circumference(double R,Random *rnd){


    
    if(in_use){
        cout << "Warning: a path has already been built on this map. It can't be modified." << endl; 
        return; 
    }

    //variabili di servizio
    double theta;
    City city;   

    //Genera N_cities città con coordinate random su una circonferenza di raggio R
    for (int i=0; i < _N_cities; i++){

        theta=rnd->Rannyu()*2*M_PI; 
        city.x=R*cos(theta); 
        city.y=R*sin(theta); 
        _cities.push_back(city);
        
    }
    


    return;
    
}


void Globe::Fill_Square(double L,Random *rnd){


    
    if(in_use){
        cout << "Warning: a path has already been built on this map. It can't be modified." << endl; 
        return; 
    }

    //variabili di servizio
    City city;   

    //Genera N_cities città con coordinate random in un quadrato di lato L
    for (int i=0; i < _N_cities; i++){
        city.x=rnd->Rannyu()*L; 
        city.y=rnd->Rannyu()*L; 
        _cities.push_back(city);
        
    }
    


    return;
    
}


void Globe::Calculate_distances(){ //Per ogni coppia di città, calcola la distanza tra esse


    if(in_use){
        cout << "Warning: a path has already been built on this map. It can't be modified." << endl; 
        return; 
    }
    City city1,city2;
   
    //riempie la parte superiore della matrice


    for(uword i=0; i < _distances.n_rows -1 ; i++){
        for(uword j=i+1; j <_distances.n_cols; j++){
            city1=_cities.at(i); 
            city2=_cities.at(j);    
            
        
            _distances(i,j)=sqrt(pow(city1.x -city2.x,2) + pow(city1.y -city2.y,2) ); 

            

        }
    }
 
    _distances.diag().zeros();//gli elementi sulla diagonale sono nulli (distanza tra una città e sè stessa è zero)
       
    _distances+=_distances.t(); //riempie la parte inferiore con gli stessi elementi (la distanza tra due città non cambia invertendo l'ordine tra le due)


    return; 
}



double Globe::Get_path_len(const irowvec & path)const{

    double lenght=0; 

    if(any( path < 0 || path >= _N_cities)){ //controllo
        throw runtime_error("Path contains an invalid index");
    }


    //somma le distanze tra città nell'ordine in cui appaiono nel percorso
    for(int k=0; k< static_cast <int>( path.n_elem) -1; k++){
        lenght+=_distances(path(k),path(k+1)); 
    }


    return lenght;
}