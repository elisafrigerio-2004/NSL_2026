### Laboratorio di Simulazione Numerica


Questa repository contiene le mie soluzioni agli esercizi proposti dal corso di _Laboratorio di Simulazione Numerica_ dell'Università degli Studi di Milano. 

Per ogni lezione, la cartella `l_<numero lezione>` contiene: 
- Un _jupyter notebook_ `LSN_Exercises_<numero lezione>.ipynb` con la consegna degli esercizi.
- Un _jupyter notebook_ `Lezione_<numero lezione>.ipynb` con la risoluzione: analisi dati, grafici e dovuti commenti.
- Per ogni esercizio, una cartella `l_<numero lezione>_es_<numero esercizio>` con i file di dati e il codice che li ha generati. Se tutti gli esercizi di una lezione sono stati eseguiti con lo stesso codice, si trovano in un'unica cartella `l_<numero lezione>_all_es`. 

Dalla lezione 4 in poi, la cartella di ciascun esercizio contiene tre (o quattro) sottocartelle. 
- `SOURCE`: contiene i file .cpp, .h e il Makefile. 
- `INPUT`: contiene i file di input necessari al funzionamento del programma.
- `OUTPUT`: contiene l'output del programma.
- `RESULTS`:  per gli esercizi che richiedono più esecuzioni dello stesso programma, gli output relativi alla singola esecuzione vengono salvati in questa cartella, sotto un nome opportuno. 

I Makefile sono provvisti di un comando `make clean_output` che consente di rimuovere gli output del programma senza destabilizzare la struttura delle cartelle. Non agisce sulla cartella `RESULTS`.


Nella cartella `Random_generator` invece, si trova il codice del generatore di numeri pseudocasuali necessario al funzionamento degli esercizi. 