#include <iostream> 
#include <string>
#include <fstream>
#include <limits>
#include "mpi.h"


#include "random.h"
#include "globe.h"
#include "path.h"
#include "population.h"

constexpr const char * INPUT_PATH= "../INPUT/";
constexpr const char * OUTPUT_PATH= "../OUTPUT/";


using namespace std; 
using namespace arma;


void load_parameters(const char * filename, bool & migrations_allowed,int &N_migr, int & pop_size, int & N_gen, parameters & pams);

int main(int argc, char *argv[]){

    arma_rng::set_seed(1); //fissa il seed per armadillo


    //SETUP MPI

    int size, rank;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /*******************************************************************************************/


    //SETUP GENERATORE
    Random rnd;
    int seed[4];
    int p1, p2;
    ifstream Primes(string(INPUT_PATH) +"Primes");


    if (Primes.is_open()){
        for(int i=0; i <= rank; i++){
            Primes >> p1 >> p2 ;
        }
    } else cerr << "PROBLEM: Unable to open Primes" << endl;
    Primes.close();
    ifstream input(string(INPUT_PATH) +"seed.in");
    string property;
    if (input.is_open()){
        while ( !input.eof() ){
            input >> property;
            if( property == "RANDOMSEED" ){
            input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
            rnd.SetRandom(seed,p1,p2);
            }
        }
        input.close();
    } else cerr << "PROBLEM: Unable to open seed.in" << endl;




    /*******************************************************************************************/


    //PARAMETRI

    
    bool migrations_allowed=1; //Sono permesse migrazioni?
    int N_migr=100; //Numero di generazioni tra una migrazione e l'altra
    int pop_size=100; //Numero individui per ogni generazione
    int N_gen=1000; // Numero generazioni
    parameters pams; //parametri algoritmo genetico


    if (rank==0){

        string filename=string(INPUT_PATH)+"input.dat";
        load_parameters(filename.c_str(), migrations_allowed,N_migr,pop_size,N_gen, pams); //carica i parametri

        //li stampa

            cout << "MIGRATION SETTINGS: migrations " ;
        cout << (( migrations_allowed)? "are allowed. Generations between migrations:  " + to_string(N_migr)  : "are not allowed")  << endl;


        cout << "POPULATION SETTINGS: " ;
        cout <<  "Population size: " << pop_size << "   Number of generations: " << N_gen << endl; 
    






    }


    
    //e li manda a tutti gli altri


    MPI_Bcast(&pop_size,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&N_gen,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&migrations_allowed,1,MPI_C_BOOL,0,MPI_COMM_WORLD);
    MPI_Bcast(&N_migr,1,MPI_INT,0,MPI_COMM_WORLD);

    MPI_Bcast(&pams,7, //numero di parametri dell'algoritmo genetico
        MPI_DOUBLE,0,MPI_COMM_WORLD);

   
    /*******************************************************************************************/

     //Output

    ofstream fout; 

    string outfile; 

    if(migrations_allowed){

        outfile= string(OUTPUT_PATH) + "W_MIGR/rank_" +to_string(rank) + "_pop.csv"; 

    }else{
        outfile= string(OUTPUT_PATH) +  "NO_MIGR/rank_" +to_string(rank) + "_pop.csv"; 

    }
    

    fout.open(outfile);

    fout <<setw(6) << "Gen" << setw(12) << "Avg_len" << setw(12) << "Best_len" << "     "<< "Best_path" << endl; 

    /*******************************************************************************************/


  
    //Mappa e popolazione

    
    string position_file= string(INPUT_PATH) + "cap_prov_ita.dat";



    Globe globe(position_file.c_str());

    int N_cities=globe.Get_N_cities(); 


    int chromosome_size=N_cities+1; 
    globe.Calculate_distances();


    Population pop(pop_size,&globe,&rnd); 

    pop.Set_pams(pams); 

    if(rank==0){
        pop.Print_pams();
    }

    /*******************************************************************************************/

    //Comunicazioni
    
    irowvec send_best_chromosome;
    irowvec receive_best_chromosome (chromosome_size, arma::fill::none); 

    irowvec communication_order=linspace <irowvec>(0,size-1,size); 

    int sender,receiver; 

    MPI_Request send_req, rec_req;
    MPI_Status send_stat,rec_stat;
  


    /*******************************************************************************************/

    for(int i=0; i < N_gen; i++){

        pop.Order(); //Ordina la generazione corrente

        if(i%N_migr==0){//migrazione
          
            
            //salva il miglior individuo per spedirlo
            Path best_path(pop.Get_individual(0));
            send_best_chromosome=pop.Get_individual(0).Get_chromosome(); 

        
         

            if(size>=2 and migrations_allowed){//implementa le migrazioni
              
                if(rank==0){
                    communication_order=arma::shuffle(communication_order); //utilizza un ordine diverso ad ogni giro   

                }
                MPI_Bcast(communication_order.memptr(),communication_order.n_elem,MPI_LONG_LONG,0,MPI_COMM_WORLD);


                
                for(int j=0; j<size; j++){
                    if(rank==communication_order(j)){ 
                        sender=communication_order((j-1 + size)%size); //seleziona il mittente per cui rank è ricevitore
                        receiver=communication_order((j+1)%size); //seleziona il recivitore per cui rank è mittente
                        break; 
                    }
                }
                

                //scambia coi vicini il miglior individuo
               

                MPI_Isend(send_best_chromosome.memptr(), chromosome_size, MPI_LONG_LONG, receiver , 0, MPI_COMM_WORLD, &send_req);
                MPI_Irecv( receive_best_chromosome.memptr(),chromosome_size, MPI_LONG_LONG, sender, 0, MPI_COMM_WORLD, &rec_req);

                MPI_Wait(&send_req, &send_stat);
                MPI_Wait(&rec_req, &rec_stat);

                best_path=Path(&globe,&receive_best_chromosome);
                pop.Set_individual(0,best_path); //sostituisce il suo miglior individuo con quello ricevuto
          

            }

         
        }   

        
        
        //stampa i risultati: generazione, lunghezza media della metà migliore della popolazione, lunghezza del miglior percorso
        fout << setw(6)  <<  i << setw(12) << pop.Get_best_half_avg_len() << setw(12) << pop.Get_individual(0).Get_len()<<"     ";
        //miglior percorso
        pop.Get_individual(0).Get_chromosome().raw_print(fout); 
       

        pop.New_generation(&rnd); //Crea la nuova generazione

   

    }



    fout.close(); 
    MPI_Finalize(); 


    


    return 0;


    

}


void load_parameters(const char * filename, bool & migrations_allowed,int &N_migr, int & pop_size, int & N_gen,parameters & pams){

    
    ifstream filein;
    filein.open(filename); 

    if (!filein.is_open()) {                                 
        cout << "Input file not found: using default parameters." << endl;
    } else {
        string property;
        int temp; 
        double temp_; 
        do {
            filein >> property;

            if (property[0] == '#') { //Salta i commenti 
                filein.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            else if (property == "Migrations_allowed") {
                if (filein >> temp && (temp == 0 || temp == 1)) { migrations_allowed = temp; } 
                else {
                    cout << "Error: Invalid value for boolean variable migrations_allowed. Keeping default: " << migrations_allowed << endl;
                    filein.clear();
                }
            } 
            else if (property == "N_migr") {
                if (filein >> temp && temp > 0) { N_migr = temp; } 
                else {
                    cout << "Error: Invalid number of generations between migrations. Keeping default: " << N_migr << endl;
                    filein.clear(); // Pulsice lo stream in caso di errori
                }
            } 
            else if (property == "Pop_size") {
                if (filein >> temp && temp > 0) { pop_size = temp; } 
                else {
                    cout << "Error: Invalid population size . Keeping default: " << pop_size << endl;
                    filein.clear();
                }
            } 
            else if (property == "N_gen") {
                if (filein >> temp && temp > 0) { N_gen = temp; }
                else {
                    cout << "Error: Invalid number of generations. Keeping default: " << N_gen << endl;
                    filein.clear();
                }
            } 
            else if (property == "P_m" ) {
                if (filein >> temp_ && temp_ >= 0.0 && temp_ <= 1.0) { pams.Pm = temp_; } 
                else {
                    cout << "Error: Invalid P_m (must be between 0 and 1). Keeping default: " << pams.Pm << endl;
                    filein.clear();
                }
            } 
            else if (property == "P_x") {
                if (filein >> temp_ && temp_ >= 0.0 && temp_ <= 1.0) { pams.Px = temp_; }
                else {
                    cout << "Error: Invalid P_x (must be between 0 and 1). Keeping default: " << pams.Px << endl;
                    filein.clear();
                }
            } 
            else if (property == "sel_p") {
                if (filein >> temp_ && temp_ > 0.0) { pams.sel_p = temp_; } 
                else {
                    cout << "Error: Invalid sel_p (must be > 0). Keeping default: " << pams.sel_p << endl;
                    filein.clear();
                }
            } 
            else if (property == "S_W") {
                if (filein >> temp_ && temp_ >= 0.0 && temp_ <= 1.0) { pams.Sw = temp_; } 
                else {
                    cout << "Error: Invalid S_W (must be between 0 and 1). Keeping default: " << pams.Sw << endl;
                    filein.clear();
                }
            } 
            else if (property == "S_S") { 
                if (filein >> temp_ && temp_ >= 0.0 && temp_ <= 1.0) { pams.Ss = temp_; } 
                else {
                    cout << "Error: Invalid S_S (must be between 0 and 1). Keeping default: " << pams.Ss << endl;
                    filein.clear();
                }
            } 
            else if (property == "S_R") {
                if (filein >> temp_ && temp_ >= 0.0 && temp_ <= 1.0) { pams.Sr = temp_; } 
                else {
                    cout << "Error: Invalid S_R (must be between 0 and 1). Keeping default: " << pams.Sr << endl;
                    filein.clear();
                }
            } 
            else if (property == "S_P") {
                if (filein >> temp_ && temp_ >= 0.0 && temp_ <= 1.0) { pams.Sp = temp_; } 
                else {
                    cout << "Error: Invalid S_P (must be between 0 and 1). Keeping default: " << pams.Sp << endl;
                    filein.clear();
                }
            } 
            else if(property != "END") {
                cout << "Invalid property: " << property << ". Skipping. " << endl;
            }

            filein.ignore(numeric_limits<streamsize>::max(), '\n'); //Salta i commenti
        } while(property != "END" && !filein.eof()); 
        
        if(!check_mutation_shares(pams)){//controlla se le share di mutazione sommano a uno,altrimenti imposta dei valori di default
            cout << "Invalid mutation shares (they do not sum up to 1). Keeping default: S_w=S_p=S_r=S_s=0.25" << endl; 
            pams.Sw=pams.Sp=pams.Sr=pams.Ss=0.25; 
        } 
        filein.close();
    }
}