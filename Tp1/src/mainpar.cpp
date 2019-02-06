#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <gmp.h>
#include <typeinfo>
#include <string>
#include <gmpxx.h>
#include "Types.hpp"
#include "Compute.hpp"
using namespace std;

int main(int argc, char *argv[])
{
    // Check for correct usage
    if (argc <= 2 || argc > 3)
    {
        cout << "Usage : " << argv[0] << "<fichier.txt>.\n";
        return EXIT_FAILURE;
    }
    // Open the file
    ifstream prime_nb_file;
    prime_nb_file.open(argv[2]);
    if (prime_nb_file.is_open() == 0)
    {
        cout << "Impossible d'ouvrir le fichier.\n";
        return EXIT_FAILURE;
    }
    // Lis le fichier et
    string line;
    vect_of_intervalles_t intervalles_bas;
    vect_of_intervalles_t intervalles_haut;

    while (getline(prime_nb_file, line))
    {
        mpz_t buffer_bas;
        mpz_t buffer_haut;
        gmp_sscanf(line.c_str(), "%Zd %Zd", buffer_bas, buffer_haut);
        intervalles_bas.push_back((mpz_class)buffer_bas);
        intervalles_haut.push_back((mpz_class)buffer_haut);
    }
    sort_and_prune(&intervalles_bas, &intervalles_haut);
    // Lancement des threads
    // Donne 1/Nb_thread du tableau a chacun
    // Lance la fonction compute trhead pour chacun
    // Chaque thread print ses nombres donc osef
    // PB si le fichier d'entrée est trop gros !
}
