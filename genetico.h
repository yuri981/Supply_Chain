#ifndef GENETICO_H
#define GENETICO_H

typedef std::vector<int> vettore;
typedef std::vector<vettore> matrice_int;

typedef std::vector<float> vettoref;
typedef std::vector<vettoref> matrice_float;

void genetico(matrice_float *mat_distanze, matrice_float *mat_offerta, std::string nome_file, int n, matrice_float *mat_costi_mezzi);

#endif
