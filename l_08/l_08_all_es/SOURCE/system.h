#ifndef ___System___
#define ___System___

#include <armadillo>
#include <string>
#include "functions.h"

class Random;



class System{



    private: 

        double _sigma; double _mu;// parametri
        double _x; //posizione

        double _x_new;//ausiliare, per mossa metropolis


        

    public: 

        System(double x,double sigma, double mu);


        // x
        double Get_x(){return _x;};
        void Set_x(double x){_x = x;};


        //Parametri: sigma, mu
        double Get_sigma(){return _sigma;};
        void Set_sigma(double sigma){_sigma = sigma;};

        double Get_mu(){return _mu;};
        void Set_mu(double mu){_mu = mu;};
        void Set_pams(double sigma,double mu){_sigma=sigma; _mu=mu;};
      


        double Metro_Sample(unsigned int N, arma::vec & samples, Random *rnd,double delta,unsigned int N_E=0); //crea un vettore di N samples secondo Metropolis, lo salva in samples, e restituisce l'accettanza. 
        // N_E è il numero di step "a vuoto" per burn in, di default zero
        bool Metro_Move(Random * rnd,double delta); //algoritmo di Metropolis: prova una mossa, la valuta, e restituisce True se la mossa è accettata e la esegue, False altrimenti (e non fa nulla)
        

        //applica le seguenti funzioni a x 

        double Potential(double x); //Energia potenziale
        double WF_mod_square(double x);  //modulo quadro funzione d'onda (non normalizzata)
        double Hamiltonian_on_WF(double x); //Restituisce H psi /psi , per calcolo <H>

        
        //applica le seguenti funzioni a un vettore di x e salva il risultato in res 
        void Potential(arma::vec & x,arma::vec & res);  //Energia potenziale
        void WF_mod_square(arma::vec & x,arma::vec & res); //modulo quadro funzione d'onda (non normalizzata)
        void Hamiltonian_on_WF(arma::vec & x,arma::vec & res); //Restituisce H psi /psi , per calcolo <H>




};





#endif // ___System___