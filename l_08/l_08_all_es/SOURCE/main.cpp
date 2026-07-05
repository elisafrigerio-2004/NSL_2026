#include <iostream> 
#include <fstream>
#include <cmath>
#include <armadillo>
#include <iomanip>
#include <string>


#include "functions.h"
#include "random.h"
#include "system.h"

using namespace std;
using namespace arma; 

constexpr const char * INPUT_PATH= "../INPUT/";
constexpr const char * OUTPUT_PATH= "../OUTPUT/";

void load_parameters(const char * filename,    // File da cui caricare i parametri
    int& N_blocks,      // Numero di blocchi per la valutazione dell'integrale con Monte Carlo
    int& N_steps,       // Numero di passi per la valutazione dell'integrale con Monte Carlo
    int& N_e,           // Passi di "burn-in" 
    double& alpha,      // Coefficiente di progressione della temperatura T
    int& B,             // Numero di valori di T considerati
    int& R,             // Numero di passi Metropolis (esterno) per ciascun valore di T
    double& delta_out,  // Delta per il Metropolis esterno (ottimizzazione dei parametri)
    double& delta_in,   // Delta per il Metropolis interno (campionamento della funzione d'onda)
    double& x_0,        // x di partenza per il campionamento Metropolis della funzione d'onda
    double& sigma_0,    // Valore iniziale del parametro sigma
    double& mu_0,       // Valore iniziale del parametro mu
    double& T_0         // Valore iniziale di T
); 






int main (int argc, char *argv[]){


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




    /**********************************************************************************/

    //PARAMETRI 


    //MonteCarlo

    int N_blocks;      // Numero di blocchi per la valutazione dell'integrale con Monte Carlo
    int N_steps;       // Numero di passi per la valutazione dell'integrale con Monte Carlo
    int N_e;           // Passi di burn-in 
    double alpha;      // Coefficiente di progressione della temperatura (T)
    int B;             // Numero di valori di T considerati
    int R;             // Numero di passi Metropolis (esterno) per ciascun valore di T
    double delta_out;  // Delta per il Metropolis esterno (ottimizzazione dei parametri)
    double delta_in;   // Delta per il Metropolis interno (campionamento della funzione d'onda)
    double x_0;        // x di partenza per il campionamento Metropolis della funzione d'onda
    double sigma_0;    // Valore iniziale del parametro sigma
    double mu_0;       // Valore iniziale del parametro mu
    double T_0;        // Valore iniziale di T


    //carica i parametri
    string filename=string(INPUT_PATH) + "input.dat";
    load_parameters(filename.c_str(),N_blocks,N_steps,N_e,alpha,B,R,delta_out,delta_in,x_0,sigma_0,mu_0,T_0); 



    cout << "PARAMETERS: " << endl;
    cout << "N_blocks: " << N_blocks << "   N_steps: " << N_steps << "   N_e: " << N_e << endl; 
    cout << "alpha: " << alpha << "   B: " << B << "   R: "<< R << endl;
    cout << "delta_in: " << delta_in << "   delta_out: " << delta_out << endl; 
    cout << "STARTING VALUES: " << endl; 
    cout <<  "x_0: " << x_0 << "   sigma_0: " << sigma_0 << "   mu_0: " << mu_0 << "   T_0: " << T_0 << endl;


    double beta=1/T_0; //beta iniziale


    /**********************************************************************************/


    //STAMPA RISULTATI

    //risultati principali
    ofstream print_main;
    print_main.open(string(OUTPUT_PATH)+"Main_results.csv");

    print_main << left << "j" << ",  " << "beta" << ", " << "delta" << ",   " << "acc_outer" << ", " << "avg_acc_inn"<< ",  "  << "last_exp_H"<< ",   "  << "error" << ",    " << "last_sigma" << ", " <<  "last_mu " << endl; 

    //risultati intemedi (un file per ogni beta)
    ofstream print_side; 
 
    

    ofstream print_best; //per i migliori parametri, stampa in dettaglio il calcolo dell'integrale e la distribuzione delle x 
    /**********************************************************************************/


    //VARIABILI DI SERVIZIO

    double accept_inner; //accettanza del metropolis interno
    double sum_accept_inner; //per calcolo accettanza media del metropolis interno
    int accepted_outer;//numero mosse accettate metropolis esterno
    double accept_outer; //accettanza del metropolis esterno

    double A; 


    //per variabili temporanee metropolis
    vec x,H_on_WF,x_save; //x e f(x) per valore medio hamiltoniana
    x_save.set_size(N_steps*N_blocks);
    x.set_size(N_steps*N_blocks); 
    H_on_WF.set_size(N_steps*N_blocks);

    double new_sigma,new_mu,save_sigma,save_mu; //parametri
    Result exp_H,new_exp_H; //valore medio hamiltoniana
   

    /**********************************************************************************/


    //INIZIALIZZAZIONE

    
    System sys(x_0,sigma_0,mu_0); //Inizializza il sistema in x=0,sigma=sigma_0, mu=mu_0


   

    /**********************************************************************************/
    
    
    //variabili di servizio, per salvataggio temporaneo

  
    

    for(int j=0; j < B; j++){


        //aggiusta il delta di modo da tenere l'accettanza attorno allo 0.5
        if(j>0 && accept_outer < 0.4){delta_out/=3; }
        else if(j>0 && accept_outer > 0.6){delta_out*=2; }



        if(j == int(9.*B/10)){
            R*=2; //per l'ultimo 10% dei valori di beta, utilizza il doppio dei passi 
        }
    
        //azzera i contatori
        accepted_outer=0; 
        sum_accept_inner=0; 

  

        //Per ogni beta,stampa i risultati per ogni step di evoluzione parametri con metropolis
        filename=string(OUTPUT_PATH) + "DETAIL/" +to_string(j) + ".csv" ; //crea il file in cui salvare i risultati
        print_side.open(filename); 
        if(print_side.is_open()){
            print_side << "sigma" <<"," << "mu" <<"," << "exp_H" <<"," << "error" <<"," << "acceptance" << endl; 

        }


        for(int r=0; r < R; r++){

            
            
            //ricalcola l'integrale coi parametri correnti(per impedire la propagazione di valori "fortunati" che si trovano a sinistra del valor medio)


            sys.Metro_Sample(N_blocks*N_steps,x,&rnd,delta_in,N_e); //calcola un vettore di _N_blocks*_N_steps x distribuite secondo il modulo quadro di psi
            
            sys.Hamiltonian_on_WF(x,H_on_WF); //per ogni x calcola l'hamiltoniana applicata a psi(x) diviso per psi(x) e la salva in H_on_Wf 
            exp_H=Data_blocking(N_blocks,N_steps,H_on_WF); //col data blocking, calola la media di H_on_Wf (e quindi l'integrale)         
          


        
            //PROPOSTA DI MOSSA

            save_sigma=sys.Get_sigma(); save_mu=sys.Get_mu(); //salva i parametri attuali 

            do{ 
                //calcola dei nuovi parametri
                new_sigma=save_sigma +delta_out*rnd.Rannyu(-1,1);
                new_mu=save_mu +delta_out*rnd.Rannyu(-1,1);

            }while(new_sigma <0 || new_mu < 0) ;//i parametri possono essere solo positivi (simmetria):

            sys.Set_pams(new_sigma,new_mu);//modifica i parametri (proposta di mossa)

            
            
            //VALUTAZIONE DELL'ACCETTAZIONE 


            accept_inner=sys.Metro_Sample(N_blocks*N_steps,x,&rnd,delta_in,N_e); //calcola un vettore di _N_blocks*_N_steps x distribuite secondo il modulo quadro di psi
            sum_accept_inner+=accept_inner; //somma l'accettanza del metropolis interno per calcolarne la media 
            
            sys.Hamiltonian_on_WF(x,H_on_WF); //per ogni x calcola l'hamiltoniana applicata a psi(x) diviso per psi(x) e la salva in H_on_Wf 
            new_exp_H=Data_blocking(N_blocks,N_steps,H_on_WF); //col data blocking, calola la media di H_on_Wf (e quindi l'integrale)


        

            A= exp(- beta* (new_exp_H.value-exp_H.value)); //calcola l'accettanza relativa alla mossa exp(-beta (E_new - E_old))


            if(A > 1 || rnd.Rannyu() < A ){ // se A >1 accetta con certezza, se A < 1 accetta con probabilità A 
                accepted_outer++; //aumenta il numero di mosse accettate
                exp_H=new_exp_H; //salva il nuovo valore dell'energia
                
                x_save=x;//salva la configurazione di x 
            }else{
                sys.Set_pams(save_sigma,save_mu);//se la mossa non è stata accettata, rimette i parametri com'erano prima
        
            }

            //salva i risultati intermedi
            if(print_side.is_open()){
                print_side << sys.Get_sigma() <<","<< sys.Get_mu() << ","<< exp_H.value <<","<< exp_H.error <<","<< accept_inner <<"\n"; 

            }
        
        }

    

        accept_outer=double(accepted_outer)/R;
        if(print_main.is_open()){
            print_main << left   << j << ",   " << beta << ",    " << delta_out << ",    " << accept_outer <<  ",    " << sum_accept_inner/R <<  ",  " <<  exp_H.value << ",   " << exp_H.error <<   ",   " << sys.Get_sigma() <<  ",    " << sys.Get_mu() << endl ; 

        }
        beta/=alpha; //progressione geometrica beta

        print_side.close();

    }



    /**********************************************************************************/



    print_main.close();


    //per gli ultimi valori di sigma e mu, stampa nel dettaglio

            
    
    //calcola e stampa la distribuzione delle x secondo il modulo quadro della funzione d'onda
    sys.Metro_Sample(N_blocks*N_steps,x,&rnd,delta_in,N_e); //calcola un vettore di _N_blocks*_N_steps x distribuite secondo il modulo quadro di psi
    print_best.open(string(OUTPUT_PATH) + "x_dist.dat"); 
    x.print(print_best); 
    print_best.close(); 
    

    

    //calcola l'integrale ,stampando il dettaglio
 
    sys.Hamiltonian_on_WF(x,H_on_WF); 
    filename=string(OUTPUT_PATH) + "integral.dat";
    Data_blocking(N_blocks,N_steps,H_on_WF,true,filename.c_str()); 

 
  
    return 0; 
}


void load_parameters(const char * filename,int& N_blocks,int& N_steps, int& N_e, double& alpha,int& B,int& R,
    double& delta_out,double& delta_in,double& x_0,double& sigma_0,double& mu_0,double& T_0){


    //valori di default
    N_blocks = 100; N_steps = 2000; N_e = 200;
    alpha = 0.78; B = 50; R = 20;
    delta_out = 0.5; delta_in = 2.0;
    x_0 = 0.0;sigma_0 = 1.0; mu_0 = 1.0; T_0 = 2.0;
    
    
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

            

            if (property[0] == '#') { //salta i  commenti
                filein.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
    
            if (property == "N_blocks") {
                if (filein >> temp && temp > 0) { N_blocks = temp;} 
                else {
                    cout << "Error: Invalid N_blocks (must be integer and > 0). Keeping default: " << N_blocks << endl;
                    filein.clear(); // Pulsice lo stream in caso di errori
                }
            } 
            else if (property == "N_steps") {
                if (filein >> temp && temp > 0) {N_steps = temp;} 
                else {
                    cout << "Error: Invalid N_steps (must be integer and > 0). Keeping default: " << N_steps << endl;
                    filein.clear();
                }
            } 
            else if (property == "N_e") {
                if (filein >> temp && temp >= 0) {N_e = temp;}
                else {
                    cout << "Error: Invalid burn in steps N_e (must be integer and >= 0). Keeping default: " << N_e << endl;
                    filein.clear();
                }
            } 
            else if (property == "alpha") {
                if (filein >> temp_ && temp_ > 0.0 && temp_ <= 1.0) {alpha = temp_;} 
                else {
                    cout << "Error: Invalid alpha (must be between 0 and 1). Keeping default: " << alpha << endl;
                    filein.clear();
                }
            } 
            else if (property == "B") {
                if (filein >> temp && temp > 0) {B = temp;} 
                else {
                    cout << "Error: Invalid number of T values B (must be integer and > 0). Keeping default: " << B << endl;
                    filein.clear();
                }
            } 
            else if (property == "R") {
                if (filein >> temp && temp > 0) {R = temp;}
                else {
                    cout << "Error: Invalid steps for T value R (must be integer and > 0). Keeping default: " << R << endl;
                    filein.clear();
                }
            } 
            else if (property == "delta_out") {
                if (filein >> temp_ && temp_ > 0.0) { delta_out = temp_;} 
                else {
                    cout << "Error: Invalid delta_out (must be > 0). Keeping default: " << delta_out << endl;
                    filein.clear();
                }
            } 
            else if (property == "delta_in") {
                if (filein >> temp_ && temp_ > 0.0) {delta_in = temp_;} 
                else {
                    cout << "Error: Invalid delta_in (must be > 0). Keeping default: " << delta_in << endl;
                    filein.clear();
                }
            } 
            else if (property == "x_0") {
                if (filein >> temp_) {x_0 = temp_;} 
                else {
                    cout << "Error: Invalid x_0. Keeping default: " << x_0 << endl;
                    filein.clear();
                }
            } 
            else if (property == "sigma_0") {
                if (filein >> temp_ && temp_ > 0.0) {sigma_0 = temp_;} 
                else {
                    cout << "Error: Invalid sigma_0 (must be > 0). Keeping default: " << sigma_0 << endl;
                    filein.clear();
                }
            } 
            else if (property == "mu_0") {
                if (filein >> temp_) {mu_0 = temp_;} 
                else {
                    cout << "Error: Invalid mu_0. Keeping default: " << mu_0 << endl;
                    filein.clear();
                }
            } 
            else if (property == "T_0") {
                if (filein >> temp_ && temp_ > 0.0) {T_0 = temp_;} 
                else {
                    cout << "Error: Invalid T_0 (must be > 0). Keeping default: " << T_0 << endl;
                    filein.clear();
                }
            }
            else if(property != "END") {
                cout << "Invalid property: " << property << ". Skipping. "<< endl;
            }

            
            filein.ignore(numeric_limits<streamsize>::max(), '\n'); //salta i commenti 
        }while(property != "END" and !filein.eof()); 
        filein.close();
    }

}