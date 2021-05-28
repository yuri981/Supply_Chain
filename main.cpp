#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <chrono>

#include "dijkstra.h"
#include "generatore_istanze.h"
#include "greedy.h"
#include "genetico.h"
#include "generatore_output.h"

using namespace std;

typedef vector<int> vettore;
typedef vector<vettore> matrice_int;

typedef vector<float> vettoref;
typedef vector<vettoref> matrice_float;

extern const string path = "C:/Users/yurim/Dropbox/Morganti/pro/";
extern const int N_MAT = 9;
extern const int N_COS = 5;
extern const int I_TTOT = 0; // indice per il tempo totale di un mezzo esclusi c/s
extern const int I_CCARB = 1; // indice per il costo del carburante di un mezzo
extern const int I_TORD = 2; // indice per il tempo ordinario dell'autista
extern const int I_TSRA = 3; // indice per il tempo straordinario dell'autista
extern const int I_TCS = 4; // indice del tempo di c/s
double elapsedTime;


int main() {
    // apre il file della domanda per ricavare il numero degli Hub
    ifstream n_hub(path + "init_input/domanda.txt");
    int i, j, n;
    n_hub >> n; // numero degli Hub
    n_hub.close();

    // genera la matrice delle distanze
    matrice_float mat_distanze;
    mat_distanze.resize(n+1);
    for (i=0; i<n+1; i++) {
        mat_distanze[i].resize(n+1);
	}
	// esegue il dijkstra per creare la matrice delle distanze
    dijkstra(&mat_distanze, n+1);

    // genera i file di input
    genera_input();

    // genera la matrice per il greedy
    matrice_float mat_greedy;
    mat_greedy.resize(n);
    for (i=0; i<n; i++) {
        mat_greedy[i].resize(N_MAT);
	}
    // genera matrice per l'offerta
    matrice_float mat_offerta;
    mat_offerta.resize(n);
    for (i=0; i<n; i++){
        mat_offerta[i].resize(2);
    }
    // genera la matrice dei costi dei mezzi
    matrice_float mat_costi_mezzi;
    mat_costi_mezzi.resize(n*2);
    for (i=0; i<n*2; i++){
        mat_costi_mezzi[i].resize(N_COS);
    }

    // apre il file che contiene i path di tutti i file di input
    ifstream file(path + "temp_out/nomi_files.txt");
    // genera il file per le soluzioni migliori di ogni input
    ofstream sol_fin(path + "temp_out/sol_migliori.txt");
    sol_fin.close();
    string str;
    string file_out = "";

    // per ogni path applica gli algoritmi
    j=1;
    while (getline(file, str)){
        auto tstart = chrono::high_resolution_clock::now();
        cout << j << "\n";
        greedy_algo(&mat_greedy, &mat_offerta, str);
        genetico(&mat_distanze, &mat_offerta, str, n, &mat_costi_mezzi);
        file_out = genera_output(&mat_offerta, str, n, &mat_costi_mezzi);
        auto tend = chrono::high_resolution_clock::now();
        elapsedTime = chrono::duration<double>(tend-tstart).count();
        ofstream ff(file_out, ios::app);
        ff << elapsedTime << " s";
        ff.close();
        ofstream all2(path + "/output/O_all.csv", ios::app);
        all2 << elapsedTime << ";" << "\n";
        j++;
    }
    file.close();

  return 0;
}
