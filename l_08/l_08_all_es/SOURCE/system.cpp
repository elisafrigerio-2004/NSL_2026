#include <cmath>

#include "random.h"
#include "system.h"

using namespace std; 
using namespace arma; 



System::System(double x,double sigma, double mu){
    
    _sigma = sigma; 
    _mu=mu;
    _x=x; 
}





double System::Metro_Sample(unsigned int N, vec & samples, Random *rnd,double delta,unsigned int N_E){ //crea un vettore di N samples secondo Metropolis e restituisce l'accettanza

    for(unsigned int i=0; i<N_E; i++){ //step di equilibrazione
        Metro_Move(rnd,delta); //esegue la mossa
    }

    samples.set_size(N);

    int accepted=0; 
    bool is_accepted=false;

    for(unsigned int i=0; i<N; i++){
        is_accepted=Metro_Move(rnd,delta); //esegue la mossa

        if(is_accepted){ accepted++; }//se è stata accettata aumenta il contatore
        samples(i)=_x; //salva la x corrente
    }


    return double(accepted)/N; 


}





bool System::Metro_Move(Random * rnd,double delta){
        
    //prova una mossa
    _x_new=_x + delta*rnd->Rannyu(-1,1); 
    //ne calcola l'accettazione
    double A= WF_mod_square(_x_new)/WF_mod_square(_x); 
 


    if(A > 1 || rnd->Rannyu() < A  ){ // se A >1 accetta con certezza, se A < 1 accetta con probabilità A 
        _x=_x_new; //accetta la mossa 
        return true;
    }

  

    return false; 

}





double System::Potential(double x){
    return pow(x,4) - 5/double(2) * x * x; 

}

double System::WF_mod_square(double x){ //modulo quadro funzione d'onda, non normalizzata

    double den= 0.5/(_sigma*_sigma); // 1/denominatore, comune ad entrambi

    double shift_right=-pow(x-_mu,2)*den; 
    double shift_left=-pow(x+_mu,2)*den; 

    return pow(exp(shift_left) + exp(shift_right),2);
}

double System::Hamiltonian_on_WF(double x){

    double c=1.0/(_sigma*_sigma);
    double arg=x*_mu*c; 
  
    double kinetic_part=- 0.5*c*((x*x+_mu*_mu)*c -1.0 - 2.0*arg*tanh(arg));
   
    return kinetic_part+ Potential(x);
}



void System::Potential(vec  & x,vec & res){
    res=pow(x,4) - 5/double(2) * square(x); 
    return; 
}

void System::WF_mod_square(vec  & x,vec & res){ //funzione d'onda, non normalizzata

    double den= 0.5/(_sigma*_sigma); // 1/denominatore, comune ad entrambi

    res = square(exp(-square(x-_mu)*den) + exp(-square(x+_mu)*den));

    return; 
}

void System::Hamiltonian_on_WF(vec & x,vec & res){

    double c=1.0/(_sigma*_sigma);
    vec arg=x*_mu*c; 

    res= - 0.5*c*((square(x)+_mu*_mu -_sigma*_sigma)*c  - 2.0*arg%tanh(arg)) //parte cinetica
    + pow(x,4) - 5/double(2) * square(x); //parte potenziale
    return;
}


