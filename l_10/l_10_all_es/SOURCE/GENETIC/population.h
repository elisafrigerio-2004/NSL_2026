#ifndef ___Population___
#define ___Population___

#include <armadillo>
#include <vector>

#include "path.h"
#include "random.h"
#include "globe.h"

struct parameters{
    double sel_p=2; //esponente per selezione
    double Px=0.7; //Probabilità crossover
    double Pm=0.05; // Probabilità mutazione
    //Ripartizione mutazioni tra: 
    double Sw=0.25; //swap
    double Ss=0.25; //shift
    double Sp=0.25; //permutate
    double Sr=0.25; //reverse

};


bool inline check_mutation_shares(parameters & pams){
        if (pams.Sw <0 or pams.Ss < 0 or pams.Sp < 0 or pams.Sr <0 or pams.Sw >1 or pams.Ss >1 or pams.Sp >1 or pams.Sr >1){return false;}
        if(!(pams.Sw+pams.Sp+pams.Ss+ pams.Sr <= 1 and pams.Sw+pams.Sp+pams.Ss+ pams.Sr >= 1-0.00001 )){return false;}
        return true; 
}






class Population{

    private: 


        int _pop_size; //numero individui della popolazione
        int _N_allele; //numero di alleli di ciascun individuo

        Globe *  _globe; //riferimento ad una mappa su cui lavorare
        
        
        std::vector <Path> _individuals; //vettore di percorsi 


        double _best_half_avg_len; //lunghezza media della migliore metà della popolazione

        //parametri

      

        double _sel_p; //esponente per selezione 
        double _Px; //probabilità crossover 
        double _Pm; //probabilità mutazione

        double _Sw; //share di mutazione swap
        double _Ss; //share di mutazione shift
        double _Sr; //share di mutazione reverse
        double _Sp; //share di mutazione permutate





    public: 


        //costruttore (warning: setta parametri di default Px=0.7, P_m=0.05, sel_p=0.2 . Per cambiarli, usare Set_pams)
        Population(int pop_size, Globe *globe,Random *rnd);
        
        

        //funzioni di stampa e di accesso ai data membri

        double Get_size()const{return _pop_size;}
        double Get_best_half_avg_len(){return _best_half_avg_len;}


        Path  Get_individual(int i)const{return _individuals.at(i);}; 
        //scrittura individuo (se l'individuo non rispetta i criteri della popolazione, darà errore)
        void Set_individual(int i, Path individual){Check(individual); _individuals.at(i)=individual;} 

        void Print()const{for (auto it = _individuals.begin(); it != _individuals.end(); ++it) {it->Print();}};
        void Print(const  char * filename) const;

  
        

        //Setta i parametri 
        void Set_pams(parameters pams);
        void Print_pams();


        //Ordina la popolazione per lunghezza crescente del percorso
        void Order();
    

        //Controlla se un individuo rispetta i criteri per appartenere alla popolazione
        void Check(Path & individual); 


        //Produce la nuova generazione della popolazione, con mutazioni e crossover
        void New_generation(Random *rnd); 
        void Mutate(Path & individual,Random *rnd); 
        

     
};






#endif  


