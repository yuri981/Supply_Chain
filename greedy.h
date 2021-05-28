#ifndef GREEDY_H
#define GREEDY_H

typedef std::vector<int> vettore;
typedef std::vector<vettore> matrice_int;

typedef std::vector<float> vettoref;
typedef std::vector<vettoref> matrice_float;

void greedy_algo(matrice_float *mat_greedy, matrice_float *mat_offerta, std::string nome_file);

#endif
