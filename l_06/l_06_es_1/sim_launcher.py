
import os 
import subprocess
import sys
import shutil
import numpy as np

INPUT_DIR="./INPUT/"
OUTPUT_DIR="./OUTPUT/"
SOURCE_DIR="./SOURCE/"



if len(sys.argv) != 4: 
    print("Usage: ",sys.argv[0], "  <Simulation type>  <Field h value> <Equilibration steps> ")
    print("<Simulation type>: M for Metropolis, G for Gibbs ")
    print("<Equilibration steps>:  integer, >=0")
    exit()



type=""
sim=0
if(sys.argv[1].upper()=="M" ): 
    print("Simulation type: Metropolis")
    type="METRO"
    sim=2
elif (sys.argv[1].upper()=="G" ): 
    print("Simulation type: Gibbs")
    type="GIBBS"
    sim=3
else : 
    print("Usage: ",sys.argv[0], "  <Simulation type>  <Field h value> <Equilibration steps> ")
    print("<Simulation type>: M for Metropolis, G for Gibbs ")
    print("<Equilibration steps>:  integer, >=0")
    exit()


h=0


try:
    h=float(sys.argv[2])

except ValueError: 
    print("Usage: ",sys.argv[0], "  <Simulation type>  <Field h value> <Equilibration steps> ")

    exit()
except Exception as e: 
    print("Error: ",e)
    print("Usage: ",sys.argv[0], "  <Simulation type>  <Field h value> <Equilibration steps> ")

    exit()


print("Field value h=",h)



N_eq=0

try:
    N_eq=int(sys.argv[3])

except ValueError:     
    print("Usage: ",sys.argv[0], "  <Simulation type>  <Field h value> <Equilibration steps> ")
    print("<Equilibration steps>:  integer, >=0")
    exit()
except Exception as e: 
    print("Error: ",e)
    print("Usage: ",sys.argv[0], "  <Simulation type>  <Field h value> <Equilibration steps> ")
    print("<Equilibration steps>:  integer, >=0")
    exit()


if N_eq < 0: 
    print("Usage: ",sys.argv[0], "  <Simulation type>  <Field h value> <Equilibration steps> ")
    print("<Equilibration steps>:  integer, >=0")
    exit()


print("Equilibration steps: ",N_eq)



results_folder=f"RESULTS/{type}_h={h}"
try: 
    os.makedirs(results_folder)
except FileExistsError: 
    print("Directory exists")
except Exception as e: 
    print("Error: ",e)
    exit()


T_values=np.arange(2.0,0.4,-0.1)

#si assicura che la configurazione xyz sia quella di ising 
shutil.copyfile(f"{INPUT_DIR}CONFIG/config.ising",f"{INPUT_DIR}CONFIG/config.xyz")
shutil.copyfile(f"{INPUT_DIR}CONFIG/config.ising",f"{OUTPUT_DIR}CONFIG/config.xyz")



restart=0




for T in T_values: #per ogni temperatura considerata
    
    print(f"Temperature: {T:.1f}")
    
    #riscrive il file di input coi parametri corretti
    
    with open(INPUT_DIR+"input.dat","w") as f:
        f.write(
            f"SIMULATION_TYPE         {sim} 1 {h}\n"
            f"RESTART                 {restart}\n"
            f"TEMP                    {T:.1f} \n"
            "NPART                   50 \n"
            "RHO                     0.8 \n"
            "R_CUT                   2.5 \n"
            "DELTA                   0.002 \n"
            "NBLOCKS                 20\n"
            "NSTEPS                  100000\n"
            "ENDINPUT")
                


    #crea una cartella per l'output della specifica temperatura
    output_folder=results_folder+ f"/T_{T:.1f}"
    try: 
        os.mkdir(output_folder)
    except FileExistsError: 
        print("Directory exists")
    except Exception as e: 
        print("Error: ",e)
        exit()

    subprocess.run(["./main.exe", str(N_eq)], cwd=SOURCE_DIR) #va nella cartella source e lancia il programma con il numero di step di equilibrazione indicato
    
    shutil.copytree(f"{OUTPUT_DIR}", f"{output_folder}",dirs_exist_ok=True) #salva gli output per non farli sovrascrivere


    
    shutil.copyfile(f'{SOURCE_DIR}seed.out',f'{INPUT_DIR}seed.in')#il generatore della prossima simulazione riparte da dov'è arrivato in quella appena conclusa

  
    restart=1

