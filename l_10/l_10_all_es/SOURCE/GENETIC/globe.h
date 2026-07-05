#ifndef ___Globe___
#define ___Globe___

#include <vector>
#include <armadillo>
#include <iostream>
#include <iomanip>
#include <string>

#include "random.h"


struct City{

    //coordinate
    double x; 
    double y; 

    //overloading operatore di stampa << 
    friend std::ostream& operator<<(std::ostream& sout, const City& city) { 
        sout  <<  city.x << std::setw(12) << city.y;
        return sout;
    }
}; 



class Globe{

    private: 

        int _N_cities; //Numero di città

        std::vector <City> _cities; //Vettore contenente le città
        
        
        arma::mat _distances; //Matrice il cui ingresso i,j è la distanza tra le città in i-esima e j-esima posizione in _cities


        bool in_use; //se sulla mappa sono stati costruiti dei percorsi, ne impedisce la modifica


    public: 


        //costruttori
        Globe(int N_cities); 

        Globe(const char *positions_file); //inizializzazione da file 

        //accesso e stampa data membri
        int Get_N_cities()const{return _N_cities;}
        City Get_city(int i)const {return _cities.at(i);}

        void Print_distances()const{_distances.print();};
        void Print_distances(std::ostream & fout)const{_distances.print(fout);};

        void Print_Cities()const{for(const auto & city: _cities){std::cout  << city << std::endl; };};
        void Print_Cities(std::ostream & fout)const {for(const auto & city: _cities){fout  << city << std::endl; };};

        void Use_Globe(){in_use=true;}; 

        //Riempimento mappa
        void Fill_Circumference(double R, Random * rnd); 
        void Fill_Square(double L,Random *rnd);


        //Distanze
        void Calculate_distances();  //Per ogni coppia di città, calcola la distanza tra esse
        double Get_distance(int i, int j)const {return _distances(i,j);}; // restituisce la distanza tra le città in posizioni i e j
        double Get_path_len(const arma::irowvec & path) const ; //calcola la lunghezza di un percorso costruito sulla mappa
};








#endif  


