#include <iostream>
#include <fstream>
#include <float.h>
#include <vector>

using namespace std;

typedef vector<float> vettoref;
typedef vector<vettoref> matrice_float;
extern const string path;

void dijkstra(matrice_float *mat_distanze, int n) {
  // Dichiarazione delle variabili utilizzate
  int i, j, z, c, inizio, fine, attuale, primo;
  int ind_righe = 0; // indici per la matrice delle distanze
  int ind_colonne = 0;
  float min, t;
  // Apertura dei file di ingresso dati
  ifstream in(path + "init_input/grafo.txt");
  // inizializza matrice delle distanze
  for (i=0; i<n; i++){
    for (j=0; j<n; j++){
        (*mat_distanze)[i][j] = 0;
    }
  }
  in >> t;
  float g[n][n]; // Metrice delle adiacenze
  float d[n]; // Vettore delle distanze
  int p[n]; // Vettore delle provenienze
  int v[n]; // Vettore dei visitati
  in >> primo;
  primo--;
  // Inizializza la matrice delle adiacenze con tutti valori infiniti
  for (i=0; i<n; i++)
    for (j=0; j<n; j++)
      g[i][j] = FLT_MAX;
  // Costruisce la matrice delle adiacenze da file
  while (!in.eof()) {
    in>>i;
    i--;
    in>>j;
    j--;
    in>>t;
    in>>g[i][j];
  }
  in.close();
  // Inizio del calcolo dei percorsi da tutti i nodi a tutti i nodi
  for (z=primo; z<n; z++){
    for (c=0; c<n; c++){
      if (z != c){
        inizio = z;
        fine = c;
    // Vettore delle distanze, che contiene per ciascun nodo la distanza minima dal nodo iniziale
          for (i=0; i<n; i++)
            d[i] = FLT_MAX;
    // Vettore delle provenienze, che contiene per ciascun nodo quello che lo precede nel percorso
          for (i=0; i<n; i++)
            p[i] = -1;
    // Vettore dei visitati, che tiene traccia dei nodi visitati
          for (i=0; i<n; i++)
            v[i] = 0;
          d[inizio] = 0;
          attuale = inizio;
          min = 0;
          while (attuale!=fine && min!=FLT_MAX) {
            min = FLT_MAX;
    // Cerca il nodo con distanza minima che non sia ancora stato visitato
            for (i=0; i<n; i++)
              if (v[i] == 0 && d[i] < min){
                min = d[i];
                attuale = i;
              }
            v[attuale] = 1;
    // Aggiorna i vettori D e P
            for (i=0; i<n; i++)
              if ( g[attuale][i] != FLT_MAX && d[i] > d[attuale] + g[attuale][i]){
                d[i] = d[attuale] + g[attuale][i];
                p[i] = attuale;
              }
            }
    // Crea la matrice delle distanze
            if (v[fine] == 0){
              (*mat_distanze)[ind_righe][ind_colonne] = -1;
              ind_colonne++;
            }
            else {
              (*mat_distanze)[ind_righe][ind_colonne] = d[fine];
              ind_colonne++;
            }
          } else {
              (*mat_distanze)[ind_righe][ind_colonne] = 0;
              ind_colonne++;
          }
        }
        ind_righe++;
        ind_colonne = 0;
      }
}
