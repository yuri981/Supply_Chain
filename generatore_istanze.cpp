#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iomanip>


using namespace std;

extern const string path;
// il numero delle percentuali utilizzate per la q.tà disponibile e il residuo
const int N_PERC = 3;
// il numero delle percentuali utilizzate per calcolare gli Rt
const int N_PERC_RT = 4;
// il numero degli indici
const int IND = 6;
// vettore delle percentuali della q.tà disponibile
float perc_q_disponibile[N_PERC] = {0.05, 0.1, 0.2};
// vettore delle percentuali del residuo
float perc_residuo[N_PERC] = {0.02, 0.05, 0.1};
// matrice degli Rt
float mat_rt[N_PERC_RT][N_PERC_RT] = {
  {0.25, 0.25, 0.25, 0.25},
  {0.5, 0.16, 0.16, 0.16},
  {0.16, 0.16, 0.16, 0.5},
  {0.16, 0.5, 0.16, 0.16}
};


// funzione che calcola lo spreco
float calcola_spreco(){
  float media = 1;
  float dev_stand = 0.894427;
  float num;

  default_random_engine engine;
  engine.seed(chrono::system_clock::now().time_since_epoch().count());
  normal_distribution <float> distribution{media, dev_stand};
  do {
    num = distribution(engine);
  }
  while (num <=0);

  return num/100;
}


void genera_rt_casuali(int rt_casuali[], int ordine_hub[], int indici[], int n){
  int i, temp;
  default_random_engine engine;
  engine.seed(chrono::system_clock::now().time_since_epoch().count());
  for (i=0; i<n; i++){
    if (i >= indici[0] && i <= indici[1]){
      uniform_real_distribution<> distr(75, 99);
      temp = distr(engine);
      rt_casuali[ordine_hub[i]-1] = temp;
    }
    else if (i >= indici[2] && i <= indici[3]){
      uniform_real_distribution<> distr(100, 124);
      temp = distr(engine);
      rt_casuali[ordine_hub[i]-1] = temp;
    }
    else if (i >= indici[4] && i <= indici[5]){
      uniform_real_distribution<> distr(125, 149);
      temp = distr(engine);
      rt_casuali[ordine_hub[i]-1] = temp;
    }
    else {
      uniform_real_distribution<> distr(150, 175);
      temp = distr(engine);
      rt_casuali[ordine_hub[i]-1] = temp;
    }
  }
}

// funzione che genera i file di input
void genera_file(float perc_rt[], float perc_qta_disponibile, float perc_residuo, int n, double dom_totale, float sprecoT, int rt_casuali[], int j, int ordine_hub[]){
  int i;
  // genera il nome del file
  float a = perc_rt[0]*100;
  float b = perc_rt[1]*100;
  float c = perc_rt[2]*100;
  float d = perc_rt[3]*100;
  string nome_file = path + "input/I_" + to_string(int(a)) + "_" + to_string(int(b)) + "_" + to_string(int(c))
                     + "_" + to_string(int(d)) + "_" + to_string(j+1) + "_" + to_string(int(perc_qta_disponibile*100))
                     + "_" + to_string(int(perc_residuo*100)) + "_" + ".txt";
  // genera la quantita disponibile
  float qta_disponibile = dom_totale - (dom_totale*perc_qta_disponibile);
  // genera lo spreco
  float spreco = qta_disponibile * sprecoT;
  // genera il residuo
  float residuo = qta_disponibile * perc_residuo;
  // apre e scrive il file che contiene i nomi di tutte le istanze
  ofstream f_nomi(path + "temp_out/nomi_files.txt", ios::app);
  f_nomi << nome_file << "\n";
  f_nomi.close();
  // apre e scrive il file dell'istanza
  ofstream out(nome_file);
  /*
  // per controllo
  out << "ordine degli Hub:\n";
  for (int y=0; y<n; y++){
    out << ordine_hub[y] << " ";
    }
  out << "\n";
  out << int(a) << "% " << int(b) << "% " << int(c) << "% " << int(d) << "%" << "\n";
  out << "q.tà disponibile " << int(perc_qta_disponibile*100) << "%\n";
  out << "residuo " << int(perc_residuo*100) << "%\n";
  // fine controllo
  */
  for (i=0; i<n; i++){
    out << i+1 << "\t" << (float)rt_casuali[i]/100 << "\n";
  }
  out << int(qta_disponibile) /*<< "  (" << dom_totale << "-" << "(" << dom_totale << "*" << perc_qta_disponibile << ")" << ")"*/ << "\n";
  out << int(spreco) /*<< "  (" << dom_totale << "*" << sprecoT << ")" */<< "\n";
  out << int(residuo) /*<< "  (" << qta_disponibile << "*" << perc_residuo << ")" */<< "\n";
  out.close();
}


//funzione che genera i file di input relativi all'Rt reale
void genera_file_rt_reale(float perc_qta_disponibile, float perc_residuo, int n, double dom_totale, float sprecoT){
  int i, t;
  // genera il nome del file
  string nome_file = path + "input/I_reale_" + to_string(int(perc_qta_disponibile*100)) + "_" + to_string(int(perc_residuo*100)) + ".txt";
  // genera gli rt
  float rt[n];
  ifstream rt_file(path + "init_input/rt_altro.txt");
  while (!rt_file.eof()){
    rt_file >> i;
    i--;
    rt_file >> rt[i];
  }
  rt_file.close();
  // genera la quantita disponibile
  float qta_disponibile = dom_totale - (dom_totale*perc_qta_disponibile);
  // genera lo spreco
  float spreco = qta_disponibile * sprecoT;
  // genera il residuo
  float residuo = qta_disponibile * perc_residuo;
  // apre e scrive il file che contiene i nomi di tutte le istanze
  ofstream f_nomi(path + "temp_out/nomi_files.txt", ios::app);
  f_nomi << nome_file << "\n";
  f_nomi.close();
  // apre e scrive il file dell'istanza
  ofstream out(nome_file);
  for (i=0; i<n; i++){
    out << i+1 << "\t" << rt[i] << "\n";
  }
  out << int(qta_disponibile) << "\n";
  out << int(spreco) << "\n";
  out << int(residuo) << "\n";
  out.close();
}


// funzione che genera gli indici necessari alla generazione del file di input
void genera_indici(int indici[], int i){
  int j;
  int mat_indici[N_PERC_RT][IND] ={
    {0, 1, 2, 3, 4, 6},
    {0, 4, 5, 6, 7, 8},
    {0, 0, 1, 2, 3, 4},
    {0, 0, 1, 5, 6, 7}
  };
  for (j=0; j<IND; j++){
    indici[j] = mat_indici[i][j];
  }
}


// inizializza il vettore degli hub
void inizializza_hub(int n, int ordine_hub[]){
    int i;
    for (i=0; i<n; i++){
        ordine_hub[i] = i+1;
    }
}

// funzione che ordina hub in modo casuale
void mescola_hub(int n, int ordine_hub[]){
    int i, j, temp;
    default_random_engine engine;
    engine.seed(chrono::system_clock::now().time_since_epoch().count());
    uniform_real_distribution<> distr(0, 9);
    for (i=0; i<n; i++){
        j = distr(engine);
        temp = ordine_hub[j];
        ordine_hub[j] = ordine_hub[i];
        ordine_hub[i] = temp;
    }
}


void genera_input(){
  // Dichiarazione variabili e apertura file della domanda
  int i, j, t, n, k;
  ifstream inp(path + "init_input/domanda.txt");
  double dom_totale;
  inp >> n; // numero di Hub
  inp >> dom_totale; // domanda totale di tutti gli Hub
  inp.close();
  float perc_rt[N_PERC_RT]; // vettore che contine le percentuali per calcolare gli Rt
  int ordine_hub[n]; // vettore per l'ordine degli Hub in fase di generazione dei file
  float spreco; // percentuale dello spreco per ogni istanza
  int rt_casuali[n]; // vettore con rt casuali per ogni istanza
  // apre il file che contiene i nomi di tutti i file generati
  ofstream out(path + "temp_out/nomi_files.txt");
  out.close();
  // calcola le istanze con Rt reale
  for (j=0; j<N_PERC; j++){
    for (t=0; t<N_PERC; t++){
      spreco = calcola_spreco();
      genera_file_rt_reale(perc_q_disponibile[j], perc_residuo[t], n, dom_totale, spreco);
    }
  }
  // calcola le altre istanze
  int indici[IND];
  for (i=0; i<N_PERC_RT; i++){
    genera_indici(indici, i);
    for (j=0; j<N_PERC_RT; j++){
      perc_rt[j] = mat_rt[i][j];
    }
    for (j=0; j<N_PERC; j++){
      inizializza_hub(n, ordine_hub);
      if (j != 0)
        mescola_hub(n, ordine_hub);
      genera_rt_casuali(rt_casuali, ordine_hub, indici, n);
      for (t=0; t<N_PERC; t++){
        for (k=0; k<N_PERC; k++){
          spreco = calcola_spreco();
          genera_file(perc_rt, perc_q_disponibile[t], perc_residuo[k], n, dom_totale, spreco, rt_casuali, j, ordine_hub);
        }
      }
    }
  }
}
