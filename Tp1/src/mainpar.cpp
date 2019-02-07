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
    vect_of_intervalles_t intervalles;
    std::cout << "wha" << std::endl;

    while (getline(prime_nb_file, line))
    {
        std::cout << line << std::endl;
        interval_t buffer;
        mpz_inits(buffer.intervalle_bas, buffer.intervalle_haut, NULL);
        std::cout << "Avsscan" << std::endl;
        gmp_sscanf(line.c_str(), "%Zd %Zd", buffer.intervalle_bas, buffer.intervalle_haut);
        std::cout << "APPsscan" << std::endl;
        intervalles.push_back(buffer);
    }
    std::cout << "plop" << std::endl;

    //sort_and_prune(intervalles);
    // Lancement des threads
    // Donne 1/Nb_thread du tableau a chacun
    // Lance la fonction compute trhead pour chacun
    // Chaque thread print ses nombres donc osef
    // PB si le fichier d'entrée est trop gros !
    //compute_intervals(intervalles_bas,intervalles_haut)
}
