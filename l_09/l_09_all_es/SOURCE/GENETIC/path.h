#ifndef ___Path___
#define ___Path___

#include <armadillo>
#include <iomanip>


#include "random.h"
#include "globe.h"

class Path{

    private: 

        Globe *  _globe; //riferimento ad una mappa su cui lavorare
        
        int _N_allele; //numero alleli 
        
        
        double _len; //lunghezza del percorso

        arma::irowvec _chromosome;//cromosoma: contiene il percorso, ossia l'ordine in cui visitare le città nella mappa

       
    
    public: 

        //overloading operatore < per sorting di path

        bool operator<(const Path& path) const{return (_globe != nullptr) ? (_len < path.Get_len()) : throw std::runtime_error("Can't compare paths without a map");}
        



        //costruttori
        Path(); 
        Path(Globe * _globe); //data una mappa, crea un percorso del tipo 0,1,...,numero_città,0  

        Path(Globe * globe,arma::irowvec * chromosome); //crea un oggetto percorso, a partire da una mappa e dal cromosoma con gli indici
        
        Path(arma::irowvec * chromosome); //crea un oggetto percorso a partire dal cromosoma con gli indici, lascia a NULL il pointer alla mappa (per debugging)

        //Funzioni di stampa e accesso a data membri

        void Print() const {_chromosome.raw_print();}//stampa 
        void Print(std::ostream & fout)const {_chromosome.raw_print(fout);}//stampa su file
        double Get_len() const {return _len;}; //restituisce la lunghezza
        const arma::irowvec & Get_chromosome() const {return _chromosome;} //ritorna un riferimento al cromosoma 



        void Shuffle(Random *rnd); //"Mischia" l'intero vettore (per inizializzazione), e ne ricalcola la lunghezza


        //Mutazioni: tutte le mutazioni avvengono escludendo a priori il primo e l'ultimo elemento (rappresentanti la città di partenza e arrivo, che è fissata)

        void Swap(Random *rnd); //seleziona due alleli e li scambia
        void Shift(Random *rnd); //seleziona due indici i_1 e i_2 e una lunghezza m e trasla la sequenza di lunghezza m da i_1 a i_2 
        void Reverse(Random *rnd); //seleziona gli estremi di una sequenza e la inverte
        void Permutate(Random *rnd); //seleziona due indici i_1 e i_2 e una lunghezza m e scambia le sequenze di lunghezza m che hanno i_1 e i_2 come primo indice

        //Dopo ogni mutazione la lunghezza del percorso viene ricalcolata 


        static void Crossover(Path & parent1, Path & parent2,Random *rnd); //Crossover, dopo di esso la lunghezza dei due percorsi viene ricalcolata

        
};






#endif  


