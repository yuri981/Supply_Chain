#ifndef GENERATORE_OUTPUT_H
#define GENERATORE_OUTPUT_H

typedef std::vector<float> vettoref;
typedef std::vector<vettoref> matrice_float;

std::string genera_output(matrice_float *mat_offerta, std::string nome_file, int n, matrice_float *mat_costi_mezzi);

#endif

