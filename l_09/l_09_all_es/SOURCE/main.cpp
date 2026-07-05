#include <iostream> 
#include <string>
#include <fstream>


#include "random.h"
#include "globe.h"
#include "path.h"
#include "population.h"

constexpr const char * INPUT_PATH= "../INPUT/";

constexpr const char * OUTPUT_PATH= "../OUTPUT/";






using namespace std; 
using namespace arma;




void load_parameters(const char * filename, int& N_cities, int& Pop_size, int& N_gen, bool& Cities_on_circle,parameters & pams); 


int main(int argc, char *argv[]){

    //SETUP GENERATORE
    Random rnd;
    int seed[4];
    int p1, p2;
    ifstream Primes(string(INPUT_PATH) +"Primes");
    if (Primes.is_open()){
        Primes >> p1 >> p2 ;
       

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


 
    //parametri 


    int N_cities=1; //numero città 
    bool Cities_on_circle=1; // le città sono sulla circonferenza? 
    int pop_size=100; //Numero individui per ogni generazione
    int N_gen=1000; // Numero generazioni
    parameters pams; //parametri algoritmo genetico


    string filename=string(INPUT_PATH) + "input.dat";

    load_parameters(filename.c_str(), N_cities, pop_size, N_gen, Cities_on_circle, pams); 

    cout << "MAP SETTINGS: " ;
    cout << N_cities << " cities ";
    cout << (( Cities_on_circle)? "on circumference " : "in square")  << endl;
    cout << "POPULATION SETTINGS: " ;
    cout <<  "Population size: " << pop_size << "   Number of generations: " << N_gen << endl; 
 




    /*******************************************************************************************/


    //Output

    ofstream fout; 
    string cities_file,paths_file;
    

    /*******************************************************************************************/

    
    //Mappa e popolazione


    Globe globe(N_cities); 


    if(Cities_on_circle){
        globe.Fill_Circumference(1.0,&rnd);
        cities_file=string(OUTPUT_PATH)+"cities_on_circle.csv"; 
        paths_file=string(OUTPUT_PATH)+"circle_paths.csv"; 


    }else{
        globe.Fill_Square(1.0,&rnd); 
        cities_file=string(OUTPUT_PATH)+"cities_in_square.csv"; 
        paths_file=string(OUTPUT_PATH)+"square_paths.csv"; 

    }
    
    globe.Calculate_distances();

    fout.open(cities_file); 
    globe.Print_Cities(fout);
    fout.close();

    fout.open(paths_file);

    fout <<setw(6) << "Gen" << setw(12) << "Avg_len" << setw(12) << "Best_len" << "     "<< "Best_path" << endl; 



    /*******************************************************************************************/


    //Popolazione

    Population pop(pop_size,&globe,&rnd); 

    pop.Set_pams(pams); 

    pop.Print_pams();


    /*******************************************************************************************/



    
    for(int i=0; i < N_gen; i++){

        pop.Order(); //Ordina la generazione corrente

        //stampa i risultati: generazione, lunghezza media della metà migliore della popolazione, lunghezza del miglior percorso
        fout << setw(6)  <<  i << setw(12) << pop.Get_best_half_avg_len() << setw(12) << pop.Get_individual(0).Get_len()<<"     ";
        //stampa il miglior percorso
        pop.Get_individual(0).Get_chromosome().raw_print(fout); 
       
        pop.New_generation(&rnd); //Crea la nuova generazione

    }    

}


void load_parameters(const char * filename, int& N_cities, int& Pop_size, int& N_gen, bool& Cities_on_circle,parameters & pams){

    
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
    
            if (property == "N_cities") {
                if (filein >> temp && temp > 0) { N_cities = temp; } 
                else {
                    cout << "Error: Invalid number of cities. Keeping default: " << N_cities << endl;
                    filein.clear(); // Pulsice lo stream in caso di errori
                }
            } 
            else if (property == "Pop_size") {
                if (filein >> temp && temp > 0) { Pop_size = temp; } 
                else {
                    cout << "Error: Invalid population size . Keeping default: " << Pop_size << endl;
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
            else if (property == "Cities_on_circle") {
                if (filein >> temp && (temp == 0 || temp == 1)) { Cities_on_circle = temp; } 
                else {
                    cout << "Error: Invalid value for boolean variable cities_on_circle. Keeping default: " << Cities_on_circle << endl;
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
        
        if(!check_mutation_shares(pams)){//controlla se le share di mutazione sommano a uno
            cout << "Invalid mutation shares (they do not sum up to 1). Keeping default: S_w=S_p=S_r=S_s=0.25" << endl; 
            pams.Sw=pams.Sp=pams.Sr=pams.Ss=0.25;
        } 
        filein.close();
    }
}