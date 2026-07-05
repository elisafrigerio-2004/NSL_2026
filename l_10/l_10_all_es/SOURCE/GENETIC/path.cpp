#include "path.h"





using namespace arma;
using namespace std;



Path::Path(){
    _globe=NULL; 
    _len=-1; 
    _N_allele=-1; //Path non inizializzato
}



Path::Path(Globe * globe){

    _globe=globe; //Mappa delle città
    globe->Use_Globe(); //segna che la mappa è in uso, per impedirne la modifica

    _N_allele=_globe->Get_N_cities() +1; //Il percorso passa da ogni città una e una sola volta, e finisce nella città di partenza

    _chromosome=linspace <irowvec>(0,_N_allele-1,_N_allele); 
    _chromosome(_N_allele -1)=0; 

}


Path::Path(Globe * globe,irowvec * chromosome){

    globe->Use_Globe(); //segna che la mappa è in uso, per impedirne la modifica
    _globe=globe; //Mappa delle città
    
    _chromosome=*chromosome;

    _len=_globe->Get_path_len(_chromosome); 
}


Path::Path(irowvec * chromosome){

    _globe=NULL; 
    _len=-1; //impossibile calcolare la lunghezza senza una mappa
    _chromosome=*chromosome;

    
}





void Path::Shuffle(Random * rnd){

    int index; 
    auto _mutable= _chromosome(span(1,_chromosome.n_elem -2)); //crea una view di armadillo escludento il primo e l'ultimo elemento dell'array (punto di partenza e di arrivo)

    for(unsigned int k=0; k < _mutable.n_elem; k++){
        index=static_cast <int> (rnd->Rannyu(0,_mutable.n_elem) );
        _mutable.swap_cols(k,index); //scambia i due alleli
    
    }




    if(_globe==nullptr){
        cout << "Warning: there is no associated map to this path. Unable to calculate path lenght." <<endl; 
        return; 
    }


    _len=_globe->Get_path_len(_chromosome); //ricalcola la lunghezza


    return;
    
}





void Path::Swap(Random * rnd){

    int f=_chromosome.n_elem -2; //indice dell'ultimo elemento modificabile e numero elementi modificabili (il conteggio parte da 1)


    if(f < 2){ //non abbastanza elementi per una mutazione
        cout << "Warning: not enough elements for swap mutation."<<endl; 
        return; 
    }


    auto _mutable= _chromosome(span(1,f)); //crea una view di armadillo escludento il primo e l'ultimo elemento dell'array (città di partenza e di arrivo,fissa)


    //genera due indici casuali
    int index1=static_cast <int> (rnd->Rannyu(0,_mutable.n_elem));
    int index2=static_cast <int> (rnd->Rannyu(0,_mutable.n_elem));

    _mutable.swap_cols(index1,index2); //scambia i due alleli

    if(_globe==nullptr){
        cout << "Warning: there is no associated map to this path. Unable to calculate path lenght." <<endl; 
        return; 
    }
    _len=_globe->Get_path_len(_chromosome); //ricalcola la lunghezza

    return; 

    
}




void Path::Shift(Random * rnd){


    int f=_chromosome.n_elem -2; //indice dell'ultimo elemento modificabile e numero elementi modificabili (il conteggio parte da 1)

    if (f<2){ //non abbastanza elementi per una mutazione
        cout << "Warning: not enough elements for shift mutation."<<endl; 
        return; 
    }else if (f==2){
        Swap(rnd); //shift corrisponde a swap
        return;
    }


    int index1=static_cast <int> (rnd->Rannyu(1,f+1)); //indice del primo allele della sequenza da shiftare
    int index2=index1;//indice del primo allele della sequenza da shiftare dopo lo shift
 
    while(index2==index1){
        index2=static_cast <int> (rnd->Rannyu(1,f+1));
    }
 
    if(index2 <index1){//li scambia
        int temp=index2; 
        index2=index1; 
        index1=temp; 
    }


    int max_len= min(index2-index1,f-index2+1); //lunghezza massima della sequenza, per evitare sovrapposizioni o di uscire dall'array


    //seleziona la lunghezza delle sequenze da shiftare
    int m=static_cast <int> (rnd->Rannyu(1,max_len+1)); 


    _chromosome(span(index1,index2 +m -1))=shift(_chromosome(span(index1,index2 +m -1)),index2-index1); //shift 

 
    if(_globe==nullptr){
        cout << "Warning: there is no associated map to this path. Unable to calculate path lenght." <<endl; 
        return; 
    }
    _len=_globe->Get_path_len(_chromosome); //ricalcola la lunghezza

    return; 


}






void Path::Reverse(Random * rnd){


    int f=_chromosome.n_elem -2; 


    if(f < 2 ){
        cout << "Warning: not enough elements for reverse mutation."<<endl; 
        return;
    }else if (f==2){
        Swap(rnd); //reverse corrisponde a swap
        return;
    }

    //seleziona gli indici di inizio e fine della sequenza da invertire (escludendo il primo e l'ultimo)
    int begin=static_cast <int> (rnd->Rannyu(1,f+1 )); 
    
    int end=begin; 

    while(end==begin){

        end=static_cast <int> (rnd->Rannyu(1,f +1)); 

    }

    if(end < begin){ // li scambia
        int temp=end; 
        end=begin; 
        begin=temp; 
    }
    
    

    _chromosome(span(begin,end))= reverse(_chromosome(span(begin,end)));//inverte la sequenza 

    if(_globe==nullptr){
        cout << "Warning: there is no associated map to this path. Unable to calculate path lenght." <<endl; 
        return; 
    }

    _len=_globe->Get_path_len(_chromosome); //ricalcola la lunghezza

    return; 

}



void Path::Permutate(Random * rnd){ 
  
    int f=_chromosome.n_elem -2; //indice dell'ultimo elemento modificabile

    if (f<2){ //non abbastanza elementi per effettuare una permutazione
        cout << "Warning: not enough elements for permutation."<<endl; 
        return; 
    }else if (f==2){
        Swap(rnd); //permutate corrisponde a swap
        return;
    }


    int index1=static_cast <int> (rnd->Rannyu(1,f+1)); //seleziona l'indice del primo allele della prima sequenza 

    int index2=index1; 
    while(index2 == index1){
        index2=static_cast <int> (rnd->Rannyu(1,f+1)); //seleziona l'indice del primo allele  della seconda sequenza
    }
      if(index2 <index1){//li scambia
        int temp=index2; 
        index2=index1; 
        index1=temp; 
    }



    int max_len= min(index2-index1,f-index2+1); //lunghezza massima della sequenza, per evitare sovrapposizioni o di uscire dall'array

    //seleziona la lunghezza delle sequenze da permutare 
    int m=static_cast <int> (rnd->Rannyu(1,max_len+1)); 


    //effettua lo scambio

    irowvec temp = _chromosome(span(index1,index1+m-1));
    _chromosome(span(index1,index1+m-1))=_chromosome(span(index2,index2+m-1));
    _chromosome(span(index2,index2+m-1))=temp; 

    if(_globe==nullptr){
        cout << "Warning: there is no associated map to this path. Unable to calculate path lenght." <<endl; 
        return; 
    }

    _len=_globe->Get_path_len(_chromosome); //ricalcola la lunghezza

    return; 
    
}


void Path::Crossover(Path & parent1, Path & parent2,Random *rnd){

    //crea dei puntatori ai cromosomi dei percorsi (per modificarli)
    auto * chromosome1= &parent1._chromosome;
    auto * chromosome2= &parent2._chromosome;

    int cut_point=static_cast<int>(rnd->Rannyu(1,chromosome1->n_elem)); //estrae a caso un punto di taglio (escluso il primo e l'ultimo carattere)


    span swap_span=span(cut_point,chromosome1->n_elem-1); //span del vettore da tagliare: dal punto di taglio fino alla fine

    //i valori contenuti nello span che viene tagliato sono ora mancanti dal vettore
    irowvec missing_values1=(*chromosome1)(swap_span); 
    irowvec missing_values2=(*chromosome2)(swap_span); 


    //vettori di appoggio, per il "rimpiazzo" dello span da scambiare
    irowvec replacement1(missing_values1.n_elem); 
    irowvec replacement2(missing_values2.n_elem); 


    int i=0;
    for(const auto value: (*chromosome2)(span(1,chromosome2->n_elem -1))){ //tra i valori del vettore 2 escluso il primo
    
        if(any(missing_values1==value)){ //cerca quelli che mancano al vettore 1
            replacement1(i)=value; //li aggiunge nell'ordine in cui appaiono nel vettore 2
            i++; 
        }
    }

    i=0; 
    for(const auto value: (*chromosome1) (span(1,chromosome1->n_elem -1))){//tra i valori del vettore 1 escluso il primo
        if(any(missing_values2==value)){//cerca quelli che mancano al vettore 2
            replacement2(i)=value; //li aggiunge nell'ordine in cui appaiono nel vettore 1
            i++;
        }
    }


    //effettua il "rimpiazzo"
    (*chromosome1)(swap_span)=replacement1;
    (*chromosome2)(swap_span)=replacement2;


    //Ricalcola la lunghezza (nota: parent1._globe  e parent2._globe dovrebbero puntare allo stesso oggetto)
    parent1._len= parent1._globe->Get_path_len(*chromosome1); 
    parent2._len=parent2._globe->Get_path_len(*chromosome2); 

    return; 


}

