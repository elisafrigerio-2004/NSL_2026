#include "population.h"

#include <algorithm>




using namespace std; 
using namespace arma; 

Population::Population(int pop_size, Globe *globe,Random *rnd){

    if(globe->Get_N_cities() < 3){
        throw runtime_error("Not enough cities for a circular path.");
    }

    _pop_size=pop_size; 
    globe->Use_Globe(); //segna la mappa come in uso, così non viene modificata
    _globe=globe;//Mappa delle città
    


    _N_allele=_globe->Get_N_cities() +1; 

    _individuals.reserve(_pop_size); 

    Path individual(_globe); 

    for(int i=0; i < _pop_size; i++){
        individual.Shuffle(rnd); //Mischia il cromosoma del percorso corrente in modo randomico
        _individuals.push_back(individual); //Aggiunge il percorso alla popolazione
        
    }

    //Viene inizializzato coi parametri di default
    parameters pams; 
   
    _sel_p=pams.sel_p; 
    _Pm=pams.Pm; 
    _Px=pams.Px;
    _Sw=pams.Sw;
    _Ss=pams.Ss;
    _Sr=pams.Sr;
    _Sp=pams.Sp;

    

}






void Population::Set_pams(parameters pams){


    if(pams.Pm>=0 and pams.Pm<=1){ _Pm=pams.Pm;}
    else{cout << pams.Pm << " is an invaild entry for Pm. Using default: "<< _Pm;}

    if(pams.Px>=0 and pams.Px<=1){ _Px=pams.Px;}
    else{cout << pams.Px << " is an invaild entry for Px. Using default: " <<_Px;}
 
    if(pams.sel_p>0 ){ _sel_p=pams.sel_p;}
    else{cout << pams.sel_p << " is an invaild entry for sel_p. Using default: " << _sel_p;}
 

       if(check_mutation_shares(pams)){ //se le share di mutazione sono valide e sommano a 1, le imposta come parametri
        _Sw=pams.Sw;
        _Ss=pams.Ss;
        _Sr=pams.Sr;
        _Sp=pams.Sp;
    }else{
        cout << "Invalid entries for mutation shares. Using default: S_W=S_R=S_P=S_S=0.25" <<endl; 
    }
    

 
}


void Population::Print_pams(){
    cout << "GENETIC ALGORITHM PARAMETERS: ";
    cout << "Mutation probability: " << _Pm << "   Crossover probability: " << _Px << "   selection exponent p: " << _sel_p << endl; 
    cout << "MUTATION SHARES: " ; 
    cout << "Swap: " << _Sw << "   Shift: " << _Ss << "   Reverse: " << _Sr << "   Permutate: " << _Sp << endl;


}





void Population::Order(){
    sort(_individuals.begin(),_individuals.end()); //riordina la popolazione in funzione della lunghezza del percorso

    double sum=0; 
    int counter=0; 
    
    //calcola la lunghezza media della metà peggiore della popolazione
    for(long unsigned int i=0; i< _individuals.size()/2; i++){ 
        sum+=_individuals.at(i).Get_len(); 
        counter++; 
    }

    _best_half_avg_len=sum/counter; 
}


void Population::Check(Path & individual){
    auto chromosome=individual.Get_chromosome(); 

    if(chromosome.n_elem!=static_cast <uword>(_N_allele)){ //Il percorso deve essere lungo quanto fissato dalla popolazione
        throw runtime_error("Path lenght does not match population");
    }

    if((chromosome)(0) != 0 || (chromosome)(chromosome.n_elem -1)!=0) {//Deve iniziare e finire con 0
        throw runtime_error("Path does not start or end with the first city");
    }
    irowvec unique_elements=unique(chromosome); 
    if(unique_elements.n_elem != chromosome.n_elem -1 ){//Deve contenere ogni città (e quindi ogni numero una volta), tranne lo 0 che appare due volte
        throw runtime_error("Unexpected repeated cities in path");
    }

    
}


void Population::New_generation(Random *rnd){
  

    int index1,index2; 
    double r;
            
    vector <Path> new_gen; //nuova generazione di percorsi

    Path parent1,parent2; //temporaneo, per salvataggio genitori

    for (int i=0; i < _pop_size; i+=2 ){

        r=rnd->Rannyu();
        index1=static_cast<int>(_pop_size*pow(r,_sel_p)); //seleziona il genitore 1

        do{
            r=rnd->Rannyu();
            index2=static_cast<int>(_pop_size*pow(r,_sel_p)); //seleziona il genitore 2
        }while(index1==index2); 

        //li salva temporaneamente
        parent1=Path(_individuals.at(index1)); 
        parent2=Path(_individuals.at(index2)); 

        if(rnd->Rannyu() < _Px){ Path::Crossover(parent1,parent2,rnd);}; //con probabilità Px fa un crossover tra i due
        if(rnd->Rannyu() < _Pm){ Mutate(parent1,rnd);};//con probabilità Pm muta il genitore1 
        if(rnd->Rannyu() < _Pm){ Mutate(parent2,rnd);}; //con probabilità Pm muta il genitore2
        //La nuova lunghezza del percorso viene automaticamente calcolata dentro la funzione di crossover o mutazione
    

        //controlla se i nuovi percorsi soddisfano i requisiti (altrimenti viene lanciata un'eccezione e il programma termina immediatamente)
        Check(parent1); 
        Check(parent2);
        //aggiunge i due nuovi percorsi alla nuova generazione
        new_gen.push_back(parent1); 
        new_gen.push_back(parent2); 
   


    }

    _individuals=new_gen;//salva la nuova generazione
    
}



void Population::Mutate(Path & individual,Random *rnd){
    
    double selector=rnd->Rannyu(0,1); //sceglie casualmente con probabilità uniforme una delle tre mutazioni possibili

    if(selector<_Sw){
        individual.Swap(rnd);//seleziona due alleli con probabilità uniforme e li scambia
    }
    else if(selector<(_Sw+_Sr)){
        individual.Reverse(rnd); //seleziona con probabilità uniforme gli estremi di una sequenza e la inverte
    }
    else if(selector<(_Sw+_Sr+_Sp)){
        individual.Permutate(rnd); //seleziona una lunghezza m, e il primo indice di due sequenze con quella lunghezza, poi le scambia
    }
    else{
        individual.Shift(rnd); // seleziona una lunghezza m, un passo n e un indice i e shifta di n la sequenza lunga m che ha i come indice del primo elemento
    }

    //La nuova lunghezza del percorso viene calcolata dentro le specifiche funzioni

}







void Population::Print(const  char * filename)const{
    ofstream fout; 

    fout.open(filename); 

    if(!fout.is_open()){
        throw runtime_error("File not found"); 
    }

    fout << left << "Lenght" << "    | Path" << endl;  
    for (auto it = _individuals.begin(); it != _individuals.end(); ++it) {it->Print(fout);}

    fout.close(); 


};


