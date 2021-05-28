#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <vector>


using namespace std;

extern const string path;
extern const int N_COS; // numero di colonne della matrice dei costi dei mezzi
extern const int I_TTOT; // indice per il tempo totale di un mezzo esclusi c/s
extern const int I_CCARB; // indice per il costo del carburante di un mezzo
extern const int I_TORD; // indice per il tempo ordinario dell'autista
extern const int I_TSRA; // indice per il tempo straordinario dell'autista
extern const int I_TCS; // indice del tempo di c/s

typedef vector<int> vettore;
typedef vector<vettore> matrice_int;

typedef vector<float> vettoref;
typedef vector<vettoref> matrice_float;

string genera_output(matrice_float *mat_offerta, string nome_file, int n, matrice_float *mat_costi_mezzi){
    int i, t;
    float temp, temp_a, temp_b, temp_c; // variabili temporanee
    float alfa = 0.4;
    float beta = 0.6;
    float rt[n]; // vettore degli Rt
    float rt_totale = 0; // il totale dei valori di Rt moltiplicati per la domanda
    int q_disponibile; // la quantità disponibile dal produttore
    int spreco; // la quantità persa
    int q_disponibile2; // la quantità disponibile dal produttore per output
    int rimanenza; // la rimanenza per output
    float dom_prima[n]; // la richiesta per la prima dose
    float dom_seconda[n]; // la richiesta per la seconda dose
    float cons_prima[n]; // la quantità consegnata per la prima dose
    float cons_seconda[n]; // la quantità consegnata per la seconda dose
    float k_mezzo; // costo giornaliero del mezzo
    float k_aut_o; // costo ordinario orario autista
    float k_aut_s; // costo straordinario orario autista
    float k_carb_km; // coto del carburante al km
    int t_carico; // tempo di carico
    int t_scarico; // tempo di scarico
    int mezzi_tot; // numero di mezzi totali disponibili
    int mezzi_sett; // numero di mezzi utilizzati nella settimana
    int autisti_tot; // numero di autisti totali
    int autisti_sett; // numero di autisti utilizzati nella settimana
    float h_autisti_o; // ore lavorate dagli autisti ordinarie
    float h_autisti_s; // ore lavorate dagli autisti straordinarie
    int tcs_totale; // tempo di c/s totale della settimana
    float k_carb_tot; // costo del carburante di tutti i mezzi
    float k_carb_ora; // il costo del carburante l'ora
    string nuovo_file; // il file di output
    float alfa_fo; // peso alfa per la funzione obiettivo
    float A_efficacia;
    float A_efficacia_n;
    float K_costi;
    float K_costi_n;
    float fo;
    // apertura del file di input relativo all'istanza corrente
    ifstream f(nome_file);
    // estrae i dati dal file
    for (i=0; i<n; i++){
      f >> t;
      f >> rt[i];
    }
    f >> q_disponibile;
    f >> spreco;
    f >> rimanenza;
    f.close();
    q_disponibile2 = q_disponibile;
    // ricava la domanda degli Hub
    ifstream f2(path + "init_input/domanda.txt");
    f2 >> t; f2 >> t;
    for (i=0; i<n; i++){
      f2 >> t; f2 >> t;
      f2 >> dom_prima[i];
      f2 >> dom_seconda[i];
    }
    f2.close();
    // calcola la somma degli Rt moltiplicati per la domanda
    for (i=0; i<n; i++){
      rt_totale = rt_totale + (rt[i] * (dom_prima[i] + dom_seconda[i]));
    }
    // ricava le quantità consegnate
    for (i=0; i<n; i++){
      cons_seconda[i] = (*mat_offerta)[i][1];
      cons_prima[i] = (*mat_offerta)[i][0];
    }
    // calcolo dell'indice di efficacia
    temp = 0;
    temp_a = 0;
    temp_b = 0;
    temp_c = 0;
    for (i=0; i<n; i++){
      temp_a = cons_prima[i]/dom_prima[i];
      temp_a *= alfa;
      temp_b = cons_seconda[i]/dom_seconda[i];
      temp_b *= beta;
      temp_c = temp_a+temp_b;
      temp_c *= rt[i];
      temp += temp_c;
    }
    A_efficacia_n = temp;
    A_efficacia = temp/rt_totale;
    // ricava i costi
    ifstream f3(path + "init_input/costi_q.txt");
    f3 >> k_mezzo;
    f3 >> k_aut_o;
    f3 >> k_aut_s;
    f3 >> k_carb_km;
    f3 >> t;
    f3 >> t_carico;
    f3 >> t_scarico;
    f3 >> mezzi_tot;
    f3 >> autisti_tot;
    f3 >> alfa_fo;
    f3.close();
    t_carico = t_carico/60;
    t_scarico = t_scarico/60;
    // ricava il numero di mezzi utilizzati
    for (i=0; i<n*2; i++){
      if ((*mat_costi_mezzi)[i][I_TTOT] == 0){
        mezzi_sett = i;
        autisti_sett = i;
        break;
      }
    }
    // ricava il costo del carburante di tutti i mezzi
    k_carb_tot = 0;
    for (i=0; i<mezzi_sett; i++){
      k_carb_tot += (*mat_costi_mezzi)[i][I_CCARB];
    }
    // ricava le ore lavorate dagli autisti
    h_autisti_o = 0;
    h_autisti_s = 0;
    for (i=0; i<mezzi_sett; i++){
      h_autisti_o += (*mat_costi_mezzi)[i][I_TORD];
      h_autisti_s += (*mat_costi_mezzi)[i][I_TSRA];
    }
    // ricava il tempo totale di c/s per la settimana
    tcs_totale = 0;
    for (i=0; i<mezzi_sett; i++){
      tcs_totale += (*mat_costi_mezzi)[i][I_TCS];

    }
    h_autisti_o += tcs_totale;
    // ricava il costo del carburante all'ora
    k_carb_ora = k_carb_tot / (h_autisti_o+h_autisti_s);
    // ricava l'indice dei costi
    temp = 0;
    temp += (mezzi_sett* k_mezzo);
    for (i=0; i<mezzi_sett; i++){
      temp_a = (*mat_costi_mezzi)[i][I_TORD]*k_aut_o;
      temp_b = (*mat_costi_mezzi)[i][I_TSRA]*k_aut_s;
      temp = temp + temp_a + temp_b + (*mat_costi_mezzi)[i][I_CCARB];
    }
    K_costi_n = temp;
    temp = 0;
    temp += (k_mezzo*mezzi_tot);
    temp_a = (8*k_aut_o) + (2*k_aut_s);
    temp_b = (10 - tcs_totale) * k_carb_ora;
    temp_c = (temp_a+temp_b) * autisti_tot;
    temp += temp_c;
    K_costi = K_costi_n / temp;
    // calcola il valore della funzione obiettivo
    temp_a = 1/A_efficacia_n;
    temp_a *= alfa_fo;
    temp_b = 1-alfa_fo;
    temp_b *= K_costi_n;
    fo = temp_a + temp_b;
    // genera il file di output
    nuovo_file = "O" + nome_file.substr(43);
    ofstream f4(path + "/output/" + nuovo_file);
    f4 << fo << "\n";
    f4 << A_efficacia << " ";
    f4 << K_costi << "\n";
    f4 << A_efficacia_n << " ";
    f4 << K_costi_n << "\n";
    f4.close();
    // aggiorna il file totale degli output
    string infile = nome_file.substr(42);
    string oufile = nuovo_file;
    ofstream all1(path + "/output/O_all.csv", ios::app);
    string perc_grav, perc_qta, perc_rim;
    string temporary = infile.substr(2, 12);
    if (infile.length() >= 24){
        perc_grav = temporary;
        perc_grav.replace(2, 1, "%");
        perc_grav.replace(5, 1, "%");
        perc_grav.replace(8, 1, "%");
        perc_grav.replace(11, 1, "%");
        perc_qta = infile.substr(16, 1);
        if (perc_qta == "1" || perc_qta == "2"){
            perc_qta = perc_qta + "0%";
            perc_rim = infile.substr(19, 1);
            if (perc_rim == "1"){
                perc_rim = perc_rim + "0%";
            }
            else {
                perc_rim = perc_rim + "%";
            }
        }
        else {
            perc_qta = perc_qta + "%";
            perc_rim = infile.substr(18, 1);
            if (perc_rim == "1"){
                perc_rim = perc_rim + "0%";
            }
            else {
                perc_rim = perc_rim + "%";
            }
        }
    }
    else if (infile.length() < 24) {
        perc_grav = "reale";
        perc_qta = infile.substr(8, 1);
        if (perc_qta == "1" || perc_qta == "2"){
            perc_qta = perc_qta + "0%";
            perc_rim = infile.substr(11, 1);
            if (perc_rim == "1"){
                perc_rim = perc_rim + "0%";
            }
            else {
                perc_rim = perc_rim + "%";
            }
        }
        else {
            perc_qta = perc_qta + "%";
            perc_rim = infile.substr(10, 1);
            if (perc_rim == "1"){
                perc_rim = perc_rim + "0%";
            }
            else {
                perc_rim = perc_rim + "%";
            }
        }
    }
    all1 << infile << ";" << perc_grav << ";" << perc_qta << ";" << perc_rim << ";";
    all1 << q_disponibile2 << ";" << spreco << ";" << rimanenza << ";";
    all1 << oufile << ";" << mezzi_sett << ";" << fo << ";" << A_efficacia << ";" << K_costi << ";" << A_efficacia_n << ";" << K_costi_n << ";";
    all1.close();
    return path + "/output/" + nuovo_file;
}
