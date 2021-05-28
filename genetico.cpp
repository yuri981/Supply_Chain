#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <float.h>
#include <vector>


using namespace std;

typedef vector<int> vettore;
typedef vector<vettore> matrice_int;

typedef vector<float> vettoref;
typedef vector<vettoref> matrice_float;

const int PERM = 100;
const int PERM_MIGLIORE = 15;
const int H_NAZIONALE = 11;
extern const string path;
extern const int N_COS; // numero di colonne della matrice dei costi dei mezzi
extern const int I_TTOT; // indice per il tempo totale di un mezzo esclusi c/s
extern const int I_CCARB; // indice per il costo del carburante di un mezzo
extern const int I_TORD; // indice per il tempo ordinario dell'autista
extern const int I_TSRA; // indice per il tempo straordinario dell'autista
extern const int I_TCS; // indice del tempo di c/s


void genetico(matrice_float *mat_distanze, matrice_float *mat_offerta, string nome_file, int n, matrice_float *mat_costi_mezzi){
    // dichiarazione delle variabili
    int i, j, k, c, t, indice, contr, inizio, sign; // contatori
    float ttot, ccarb, tord, tsra, tcs; // variabili temporanee per la matrice dei costi dei mezzi
    int perm_corrente[n]; // vettore per la permutazione in corso di lavorazione
    float k_mezzo_d; // costo del veicolo giornaliero
    float k_autista_oh; // costo ordinario dell'autista orario
    float k_autista_sh; // costo straordinario dell'autista orario
    float k_carburante; // costo del carburante euro/km
    int cap_mezzo; // capacità del mezzo
    float t_carico; // tempo di carico
    float t_scarico; // tempo di scarico
    float costo_corrente; // il costo della permutazione corrente
    int mat_mezzi[n*2][n+2]; // matrice che contiene i mezzi da utilizzare con la permutazione corrente
    float mat_costi[n+1][n+1]; // matrice dei costi sulle distanze
    float mat_tempi[n+1][n+1]; // matrice dei tempi di percorrenza
    float v_medie[n+1][n+1]; // matrice delle velocità medie
    matrice_int mat_permutazioni; // matrice delle permutazioni
    mat_permutazioni.resize(PERM);
    for (i=0; i<PERM; i++) {
        mat_permutazioni[i].resize(n);
    }
    float temp, tmp; // variabili temporanee
    int hub1, hub2; // variabili per il calcolo delle velocità medie
    int i_mat, i_perm, i_off; // contatori per il calcolo dei mezzi
    int temp_mezzo; // variabile temporanea per il calcolo dei mezzi
    int temp_int; // variabile temporanea intera
    bool ended, is_ok; // variabili boleane
    int offerta_corrente[n]; // vettore che contiene l'offerta ordinata per id hub
    int offerta_ordinata[n]; // vettore che contiene l'offerta nell'ordine della permutazione
    int ind1, ind2; // indici
    int h1, h2; // varibili temporanee per gli hub  nel calcolo del costo della permutazione
    float tempo, costo; // variabili necessarie nel calcolo del costo della permutazione
    int mezzo_corrente[n+2]; // il mezzo in lavorazione nel calcolo del costo della permutazione
    int sol_migliori[2][n]; // matrice che contiene le due soluzioni migliori
    float migliore1 = FLT_MAX; // prima soluzione migliore
    float migliore2 = FLT_MAX; // seconda soluzione migliore
    int sol_temp[n]; // vettore temporaneo per calcolo delle soluzioni migliori
    int permutazione[n]; // vettore della permutazione corrente
    ofstream sol3(path + "temp_out/sol_migliori.txt"); sol3.close();

    // apre il file dei costi e assegna le variabili
    ifstream in(path + "init_input/costi_q.txt");
    in >> k_mezzo_d;
    in >> k_autista_oh;
    in >> k_autista_sh;
    in >> k_carburante;
    in >> cap_mezzo;
    in >> t_carico;
    in >> t_scarico;
    in.close();
    // converte il tempo in centesimi
    t_carico = t_carico/60;
    t_scarico = t_scarico/60;

    // calcola e genera la matrice dei costi
    for (k=0; k<n+1; k++){
        for (j=0; j<n+1; j++){
            temp = (*mat_distanze)[k][j];
            mat_costi[k][j] = temp*k_carburante;
        }
    }

    // calcola l'offerta complessiva per ogni hub
    for (i=0; i<n; i++){
        offerta_corrente[i] = (*mat_offerta)[i][0] + (*mat_offerta)[i][1];
    }

    // inizializza la matrice con i tempi di percorrenza tra gli Hub
    for (i=0; i<n+1; i++){
        for (j=0; j<n+1; j++){
            mat_tempi[i][j] = 0;
        }
    }
    // inizializza la matrice delle velocità medie
    for (i=0; i<n+1; i++){
        for (j=0; j<n+1; j++){
            v_medie[i][j] = 0;
        }
    }

    // apre il file con le velocità medie
    ifstream vel(path + "init_input/grafo.txt");
    vel >> t; vel >> t;
    // ricava le velocità medie
    while (!vel.eof()){
        vel >> hub1;
        vel >> hub2;
        vel >> v_medie[hub1-1][hub2-1];
        vel >> tmp;
    }
    vel.close();

    // estrae i dati necessari per la matrice dei tempi
    for (i=0; i<n+1; i++){
        for (j=0; j<n+1; j++){
            if (v_medie[i][j] != 0){
                mat_tempi[i][j] = (*mat_distanze)[i][j]/v_medie[i][j];
            }
        }
    }
    // inizializza la matrice dei costi dei mezzi
    for (i=0; i<n*2; i++){
        for (j=0; j<N_COS; j++){
            (*mat_costi_mezzi)[i][j] = 0;
        }
    }


    // genera le prime 100 permutazioni
    // inizializza il processo di generazione dei mumeri casuali
    default_random_engine engine;
    engine.seed(chrono::system_clock::now().time_since_epoch().count());
    uniform_int_distribution<int> distr(0, 9);
    uniform_int_distribution<int> distr2(0, 9);
    // inizializza la matrice delle permutazioni
    for (i=0; i<PERM; i++){
        for (j=0; j<n; j++){
            mat_permutazioni[i][j] = 0;
        }
    }
    // riempie la permutazione ordinata
    for (i=0; i<n; i++){
        permutazione[i] = i+1;
    }
    is_ok = false; // variabile booleana per il controllo delle permutazioni
    // comincia il calcolo delle permutazioni
    for (i=0; i<PERM; i++){
        // ciclo while pre la creazione e il controllo della permutazione corrente
        while (!is_ok){
            // crea la permutazione corrente
            for (j=0; j<n; j++){
                k = distr(engine);
                temp_int = permutazione[k];
                permutazione[k] = permutazione[j];
                permutazione[j] = temp_int;
            }
            // controlla se la permutazione corrente e' diversa da tutte le altre
            // fa la differenza tra la permutazione corrente e le altre
            c = 0;
            for (j=0; j<i; j++){
                if (i == 0) break;
                contr=0;
                for (k=0; k<n; k++){
                    if (permutazione[k] - mat_permutazioni[j][k] == 0){
                        contr++;
                    }
                }
                // se una permutazione è uguale ad un'altra
                if (contr == n) {
                    c = 1;
                }
            }
            if (c != 1){
                is_ok = true;
                for (j=0; j<n; j++){
                    mat_permutazioni[i][j] = permutazione[j];
                }
            }
        } // fine ciclo while
        is_ok = false;
    } // fine ciclo for delle prime 100 permutazioni



    // ciclo for per lo sviluppo dell'algoritmo
    for (indice=0; indice<PERM; indice++){
    // ciclo for per la ennesima centinaia di permutazioni
        for (i=0; i<PERM; i++){
        // ricava la permutazione corrente
            for (j=0; j<n; j++){
                perm_corrente[j] = mat_permutazioni[i][j];
            }

        // inizializza matrice dei mezzi
            for (k=0; k<n*2; k++){
                for (j=0; j<n+2; j++){
                    mat_mezzi[k][j] = 0;
                }
            }
        // calcola i mezzi necessari
            for (k=0; k<n; k++){
                t = perm_corrente[k];
                offerta_ordinata[k] = offerta_corrente[t-1];
            }
        // ciclo while per il calcolo dei mezzi
            i_mat = 0;
            i_perm = 0;
            i_off = 0;
            ended = false;
            while (!ended){
                // il primo hub del percorso del mezzo è sempre quello nazionale
                mat_mezzi[i_mat][0] = H_NAZIONALE;
                temp_mezzo = 0;
                c = 1;
                // calcola quanti hub possono entrare in un mezzo
                while (temp_mezzo <= cap_mezzo){
                    temp_mezzo++;
                    offerta_ordinata[i_off]--;
                    if (offerta_ordinata[i_off] <= 0){
                        i_off++;
                        if (i_off >= n){
                            break;
                        }
                        c++;
                    }
                }
                // assegna gli hub trovati al mezzo
                if (c == 1){
                    mat_mezzi[i_mat][1] = perm_corrente[i_perm];
                }
                else {
                    for (k=1; k<=c; k++){
                        mat_mezzi[i_mat][k] = perm_corrente[i_perm];
                        if (k != c)
                            i_perm++;
                    }
                }
                // l'ultimo hub del percorso del mezzo è sempre quello nazionale
                mat_mezzi[i_mat][c+1] = H_NAZIONALE;
                // se le dosi da trasportare sono terminate termina il ciclo
                if (i_off > n-1){
                    ended = true;
                } else {
                    i_mat++;
                }
            }

            // calcola il costo della permutazione
            k=0; costo = 0; ended = false; ttot = 0; ccarb = 0; tord = 0; tsra = 0; tcs = 0;
            // ciclo while per il calcolo del costo
            while (!ended){
                for (j=0; j<n+2; j++){
                    mezzo_corrente[j] = 0;
                }
                // estrae un mezzo
                for (j=0; j<n+2; j++){
                t = mat_mezzi[k][j];
                if (t == 0){
                    break;
                }
                mezzo_corrente[j] = t;
                }
                // calcola i costi orari dell'autista e del carburante
                ind1 = 0; ind2 = 1; tempo = 0;
                while (mezzo_corrente[ind2] != 0){
                    h1 = mezzo_corrente[ind1];
                    h2 = mezzo_corrente[ind2];
                    tempo += mat_tempi[h1-1][h2-1];
                    costo = costo + mat_costi[h1-1][h2-1];
                    ccarb = ccarb + mat_costi[h1-1][h2-1];
                    ind1++;
                    ind2++;
                }
                // aggiunge il tempo totale del mezzo esclusi c/s sulla matrice dei costi dei mezzi
                ttot = tempo;
                // aggiunge il tempo di carico e scarico
                tempo = tempo + t_carico + (t_scarico*(ind2-2));
                tcs = t_carico + (t_scarico*(ind2-2));
                // calcola il costo del tempo per l'autista
                if (tempo > 8){
                    costo = costo + ((tempo-8) * k_autista_sh);
                    tsra = tempo-8;
                    tempo = 8;
                    costo = costo + (tempo * k_autista_oh);
                    tord = tempo;
                } else {
                costo = costo + (tempo * k_autista_oh);
                tord = tord + tempo;
                }
                // aggiunge il costo del mezzo
                costo += k_mezzo_d;
                // aggiorna contatori
                (*mat_costi_mezzi)[k][I_TTOT] = ttot;
                (*mat_costi_mezzi)[k][I_CCARB] = ccarb;
                (*mat_costi_mezzi)[k][I_TORD] = tord;
                (*mat_costi_mezzi)[k][I_TSRA] = tsra;
                (*mat_costi_mezzi)[k][I_TCS] = tcs;
                ttot = 0; ccarb = 0; tord = 0; tsra = 0; tcs = 0;
                k++;
                // controlla se i mezzi sono finiti
                if (mat_mezzi[k][0] == 0){
                    ended = true;
                }
            }
            costo_corrente = costo;

            // calcolo delle soluzioni migliori
            if (costo_corrente < migliore1){
                tmp = migliore1;
                migliore1 = costo_corrente;
                migliore2 = tmp;
                for (j=0; j<n; j++){
                    sol_temp[j] = sol_migliori[0][j];
                }
                for (j=0; j<n; j++){
                    sol_migliori[0][j] = perm_corrente[j];
                }
                for (j=0; j<n; j++){
                    sol_migliori[1][j] = sol_temp[j];
                }
            } else if (costo_corrente < migliore2 && costo_corrente > migliore1){
                migliore2 = costo_corrente;
                for (j=0; j<n; j++){
                    sol_migliori[1][j] = perm_corrente[j];
                }
            }
        } // fine ciclo for delle 100 permutazioni




        // comincio a generare le prossime 100 permutazioni
        // inizializza la matrice delle permutazioni
        for (j=0; j<PERM; j++){
            for (k=0; k<n; k++){
        mat_permutazioni[j][k] = 0;
            }
        }
        // inserisce le due permutazioni migliori del ciclo precedente
        for (j=0; j<n; j++){
            mat_permutazioni[0][j] = sol_migliori[0][j];
        }
        for (j=0; j<n; j++){
            mat_permutazioni[1][j] = sol_migliori[1][j];
        }


        // genero le prime 15 permutazioni
        for (j=0; j<n; j++){
            sol_temp[j] = sol_migliori[0][j];
        }
        // crea una permutazione corrente
        for (i=0; i<n; i++){
            permutazione[i] = sol_temp[i];
        }
        is_ok = false; // variabile booleana per il controllo delle permutazioni
        inizio = 2;
        for (i=inizio; i<PERM_MIGLIORE+inizio; i++){
            // ciclo while pre la creazione e il controllo della permutazione corrente
            while (!is_ok){
                // crea la permutazione corrente
                k = distr(engine);
                c = distr2(engine);
                temp_int = permutazione[k];
                permutazione[k] = permutazione[c];
                permutazione[c] = temp_int;
                // controlla se la permutazione corrente e' diversa da tutte le altre
                // fa la differenza tra la permutazione corrente e le altre
                c = 0;
                for (j=0; j<i; j++){
                    if (i == 0) break;
                    contr = 0;
                    for (k=0; k<n; k++){
                        if (permutazione[k] - mat_permutazioni[j][k] == 0){
                            contr++;
                        }
                    }
                    // se una permutazione è uguale ad un'altra
                    if (contr == n) {
                        c = 1;
                    }
                }
                if (c != 1){
                    is_ok = true;
                    for (j=0; j<n; j++){
                        mat_permutazioni[i][j] = permutazione[j];
                    }
                }
            } // fine ciclo while
            is_ok = false;
            // inizializza il vettore permutazioni
            for (j=0; j<n; j++){
                permutazione[j] = sol_temp[j];
            }
        } // fine ciclo for delle prime 15 permutazioni



        // genero le seconde 15 permutazioni
        for (j=0; j<n; j++){
            sol_temp[j] = sol_migliori[1][j];
        }
        // crea una permutazione corrente
        for (i=0; i<n; i++){
            permutazione[i] = sol_temp[i];
        }
        is_ok = false; // variabile booleana per il controllo delle permutazioni
        inizio = PERM_MIGLIORE+2;
        for (i=inizio; i<PERM_MIGLIORE+inizio; i++){
            // ciclo while pre la creazione e il controllo della permutazione corrente
            while (!is_ok){
                // crea la permutazione corrente
                k = distr(engine);
                c = distr2(engine);
                temp_int = permutazione[k];
                permutazione[k] = permutazione[c];
                permutazione[c] = temp_int;
                // controlla se la permutazione corrente e' diversa da tutte le altre
                // fa la differenza tra la permutazione corrente e le altre
                c = 0;
                for (j=0; j<i; j++){
                    if (i == 0) break;
                    contr = 0;
                    for (k=0; k<n; k++){
                        if (permutazione[k] - mat_permutazioni[j][k] == 0){
                            contr++;
                        }
                    }
                    // se una permutazione è uguale ad un'altra
                    if (contr == n) {
                        c = 1;
                    }
                }
                if (c != 1){
                    is_ok = true;
                    for (j=0; j<n; j++){
                        mat_permutazioni[i][j] = permutazione[j];
                    }
                }
            } // fine ciclo while
            is_ok = false;
            // inizializza il vettore permutazioni
            for (j=0; j<n; j++){
                permutazione[j] = sol_temp[j];
            }
        } // fine ciclo for delle seconde 15 permutazioni



        // genero le restanti 68 permutazioni
        contr = PERM_MIGLIORE*2; // numero delle permutazioni già presenti
        contr += 2;
        // riempie la permutazione ordinata
        for (i=0; i<n; i++){
            permutazione[i] = i+1;
        }
        is_ok = false; // variabile booleana per il controllo delle permutazioni
        // comincia il calcolo delle permutazioni
        for (i=contr; i<PERM; i++){
            // ciclo while pre la creazione e il controllo della permutazione corrente
            while (!is_ok){
                // crea la permutazione corrente
                for (j=0; j<n; j++){
                    k = distr(engine);
                    temp_int = permutazione[k];
                    permutazione[k] = permutazione[j];
                    permutazione[j] = temp_int;
                }
                // controlla se la permutazione corrente e' diversa da tutte le altre
                // fa la differenza tra la permutazione corrente e le altre
                c = 0;
                for (j=0; j<i; j++){
                    if (i == 0) break;
                    sign = 0;
                    for (k=0; k<n; k++){
                        if (permutazione[k] - mat_permutazioni[j][k] == 0){
                            sign++;
                        }
                    }
                    // se una permutazione è uguale ad un'altra
                    if (sign == n) {
                        c = 1;
                    }
                }
                if (c != 1){
                    is_ok = true;
                    for (j=0; j<n; j++){
                        mat_permutazioni[i][j] = permutazione[j];
                    }
                }
            } // fine ciclo while
            is_ok = false;
        } // fine ciclo for delle 68 permutazioni


    } // fine ciclo totale

  // genera soluzione finale
  ofstream sol(path + "temp_out/sol_migliori.txt", ios::app);
  sol << nome_file.substr(42) << "\n";
  for (i=0; i<n; i++){
    sol << offerta_corrente[i] << " ";
  }
  sol << "\n";
  for (i=0; i<n; i++){
    sol << sol_migliori[0][i] << " ";
  }
  sol << "\n" << migliore1 << "\n";
  for (i=0; i<n; i++){
    sol << sol_migliori[1][i] << " ";
  }
  sol << "\n" << migliore2 << "\n";
  sol.close();
}
