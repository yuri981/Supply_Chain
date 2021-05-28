#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

typedef vector<int> vettore;
typedef vector<vettore> matrice_int;

typedef vector<float> vettoref;
typedef vector<vettoref> matrice_float;

extern const string path;
// dichiarazione costanti per l'indicizzazione della matrice utilizzata nel greedy
extern const int N_MAT; // numero di colonne della matrice
const int I_HUB = 0;  // indice per gli id Hub
const int I_RT = 1;  // indice per gli Rt
const int I_DOM1 = 2;  // indice per la domanda per la prima dose
const int I_DOM2 = 3;  // indice per la domanda per la seconda dose
const int I_DTOT = 4;  // indice per la domanda totale
const int I_W1 = 5;  // indice per il peso relativo all'Rt
const int I_W2 = 6;  // indice per il peso realtivo alle domande totali
const int I_WM = 7;  // indice per la media di w1 e w2
const int I_MOLTWM = 8;  // indice per i prodotti tra le dosi mancanti e le medie


void greedy_algo(matrice_float *mat_greedy, matrice_float *mat_offerta, string nome_file){
    // dichiarazione delle variabili necessarie
    int i, j; // contatori
    float t; // variabile per valori da cestinare
    int n; // il numero di Hub
    int dom_totale; // il totale della domanda di tutti gli Hub
    int q_disponibile; // la quantità messa a disposizione dal produttore
    int mancanti; // la differenza tra domanda e quantità disponibile
    float somma_rt = 0; // la somma di tutti gli Rt
    // apertura del file della domanda per recuperare alcuni dati
    ifstream inp(path + "init_input/domanda.txt");
    inp >> n;
    // inizializza la matrice
    for (i=0; i<n; i++){
        for (j=0; j<N_MAT; j++)
            (*mat_greedy)[i][j] = 0;
    }
    // genero gli id Hub nella matrice
    for (i=0; i<n; i++)
        (*mat_greedy)[i][I_HUB] = i+1;
    inp >> dom_totale;
    // ricava le domande totali e relative alle due dosi per tutti gli Hub
    while (!inp.eof()){
        inp >> i;
        i--;
        inp >> (*mat_greedy)[i][I_DTOT];
        inp >> (*mat_greedy)[i][I_DOM1];
        inp >> (*mat_greedy)[i][I_DOM2];
    }
    inp.close();
    // apertura del file di input relativo all'istanza corrente
    ifstream file(nome_file);
    // ricava gli Rt dell'istanza corrente
    for (i=0; i<n; i++){
        file >> t;
        file >> (*mat_greedy)[i][I_RT];
    }
    // ricava la quantità disponibile
    file >> q_disponibile;
    file.close();
    // ricavo la somma degli Rt di tutti gli hub
    for (i=0; i<n; i++){
        somma_rt += (*mat_greedy)[i][I_RT];
    }
    // calcolo le dosi mancanti
    mancanti = dom_totale - q_disponibile;
    // ricavo i pesi w1 e w2, le medie e le medie moltiplicate per le dosi mancanti
    for (i=0; i<n; i++){
        (*mat_greedy)[i][I_W1] = (*mat_greedy)[i][I_RT]/somma_rt;
        (*mat_greedy)[i][I_W2] = (*mat_greedy)[i][I_DTOT]/dom_totale;
        (*mat_greedy)[i][I_WM] = ((*mat_greedy)[i][I_W1]+(*mat_greedy)[i][I_W2])/2;
        (*mat_greedy)[i][I_MOLTWM] = (*mat_greedy)[i][I_WM]*mancanti;
    }
    // inizializza la matrice delle offerte
    for (i=0; i<n; i++){
        (*mat_offerta)[i][0] = 0;
        (*mat_offerta)[i][1] = 0;
    }
    // assegna a priori una percentuale della domanda della seconda dose
    int dosi_ap[n];
    for (i=0; i<n; i++){
        dosi_ap[i] = (*mat_greedy)[i][I_DOM2]*0.2;
        (*mat_greedy)[i][I_DOM2] -= dosi_ap[i];
        q_disponibile -= dosi_ap[i];
    }
    // assegna le dosi di vaccino ad ogni Hub escludendo quelle mancanti
    for (i=0; i<n; i++){
        (*mat_offerta)[i][0] = (*mat_greedy)[i][I_DOM1];
        (*mat_offerta)[i][1] = (*mat_greedy)[i][I_DOM2];
    }

    // ordina la matrice con selection sort
    float temp[N_MAT];
    int minimo;
    // ciclo dell'algoritmo
    for (i=0; i<n-1; i++){
        minimo=i;
        // controlla se il valore successivo a quello preso in esame è ad esso minore
        for (j=i+1; j<n; j++){
            if ((*mat_greedy)[j][I_RT] < (*mat_greedy)[minimo][I_RT])
                minimo = j;
        }
        // scambio della serie di valori sulla matrice: crea un vettore temporaneo con la serie minore
        for (j=0; j<N_MAT; j++){
            temp[j] = (*mat_greedy)[minimo][j];
        }
        // copia la serie maggiore
        for (j=0; j<N_MAT; j++){
            (*mat_greedy)[minimo][j] = (*mat_greedy)[i][j];
        }
        // copia la serie minore dal vettore temporaneo nella matrice
        for (j=0; j<N_MAT; j++){
            (*mat_greedy)[i][j] = temp[j];
        }
    }

    // calcola l'offerta togliendo le dosi mancanti
    int differenza;
    int maggiore = 9; // indice per matrice ordinata che punta all'Hub con maggiore gravità
    int minore = 0; // indice per matrice ordinata che punta all'Hub con minore gravità
    int residuo1, residuo2, precedente, rimanenza_min, rimanenza_mag; // variabile utilizzata per contenere temporaneamente le dosi che da togliere
    int indice_maggiore, indice_minore; // indici da utilizzare per il calcolo
    rimanenza_min = 0;
    rimanenza_mag = 0;
    while (maggiore > minore){
        indice_maggiore = (*mat_greedy)[maggiore][I_HUB]-1;
        indice_minore = (*mat_greedy)[minore][I_HUB]-1;

        // togli le prime dosi per l'Hub con minore gravità
        differenza = (*mat_greedy)[maggiore][I_MOLTWM]+rimanenza_min;
        differenza /= 2;
        residuo1 = differenza;
        residuo2 = differenza;
        precedente = (*mat_offerta)[indice_minore][0] - residuo1;
        if (precedente >= 0){
            (*mat_offerta)[indice_minore][0] -= residuo1;
        } else {
            rimanenza_min += residuo1 - (*mat_offerta)[indice_minore][0];
            (*mat_offerta)[indice_minore][0] = 0;
        }

        // togli le seconde dosi per l'hub con minore gravità
        precedente = (*mat_offerta)[indice_minore][1] - residuo2;
        if (precedente >= 0){
            (*mat_offerta)[indice_minore][1] -= residuo2;
        } else {
            rimanenza_min += residuo2 - (*mat_offerta)[indice_minore][1];
            (*mat_offerta)[indice_minore][1] = 0;
        }

        // togli le prime dosi per l'Hub con maggiore gravità
        differenza = (*mat_greedy)[minore][I_MOLTWM]+rimanenza_mag;
        differenza /= 2;
        residuo1 = differenza;
        residuo2 = differenza;
        rimanenza_mag = 0;
        precedente = (*mat_offerta)[indice_maggiore][0] - residuo1;
        if (precedente >= 0){
            (*mat_offerta)[indice_maggiore][0] -= residuo1;
        } else {
            rimanenza_mag += residuo1 - (*mat_offerta)[indice_maggiore][0];
            (*mat_offerta)[indice_maggiore][0] = 0;
        }

        // togli le seconde dosi per l'hub con maggiore gravità
        precedente = (*mat_offerta)[indice_maggiore][1] - residuo2;
        if (precedente >= 0){
            (*mat_offerta)[indice_maggiore][1] -= residuo2;
        } else {
            rimanenza_mag += residuo2 - (*mat_offerta)[indice_maggiore][1];
            (*mat_offerta)[indice_maggiore][1] = 0;
        }

        // aggiorna gli indici
        minore++;
        maggiore--;
    }

    for (i=0; i<n; i++){
        (*mat_offerta)[i][1] += dosi_ap[i];
    }
}
