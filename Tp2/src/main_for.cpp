#include <iostream>    // Entrées sorties de base C++
#include <fstream>     // File stream pour le fichier d'intervalles
#include <stdio.h>     // Entrées sorties de base C
#include <vector>      // Tableaux dynamiques en C++
#include <string>      // Class string en C++
#include <gmp.h>       // Bibliothèque de gros nombres C
#include <gmpxx.h>     // Bibliothèque de gros nombres C++
#include <omp.h>       // Bibliothèque OpenMp pour le calcul parallèle
#include "Compute.hpp" // Fonction pour le calculs de nombre premiers
#include "Chrono.hpp"  // Classe chronomètre pour le temps d'éxécution
#include "Types.hpp"   // Structures de données pratiques pour le traitement

int main(int argc, char const *argv[])
{
    // Check for correct usage
    if (argc <= 2 || argc > 3)
    {
        cerr << "Usage : " << argv[0] << "<Nombre de threads> <fichier.txt>.\n";
        return EXIT_FAILURE;
    }

    int nb_threads = atoi(argv[1]);
    omp_set_num_threads(nb_threads);

    // Open the file
    ifstream prime_nb_file;
    prime_nb_file.open(argv[2]);
    if (prime_nb_file.is_open() == 0)
    {
        cerr << "Impossible d'ouvrir le fichier.\n";
        return EXIT_FAILURE;
    }

    // Lis le fichier et sauvegarde les intervalles
    string line;
    vect_of_intervalles_t intervalles;
    while (getline(prime_nb_file, line))
    {
        interval_t buffer;
        gmp_sscanf(line.c_str(), "%Zd %Zd", buffer.intervalle_bas.value, buffer.intervalle_haut.value);
        intervalles.push_back(buffer);
    }
    Chrono chron = Chrono();
    float tic = chron.get();
    swap_intervalle(intervalles);
    sort_and_prune(intervalles);

    // DEBUT DU PARALELLE
    vect_of_intervalles_t::const_iterator first_elt_buffer;
    vect_of_intervalles_t::const_iterator last_elt_buffer;
    vector<Custom_mpz_t> finalList;
#pragma omp parallel
    {
#pragma omp for {

        for (int i = 0; i < intervalles.size(); i++)
        {
            vector<Custom_mpz_t> resultat = compute_interval(intervalles.at[i]);
#pragma omp critical {
            finalList.insert(finalList.end(),
                             std::make_move_iterator((resultat).begin()),
                             std::make_move_iterator((resultat).end()));
        }
    }
}
}

float tac = chron.get();
for (int i = 0; i < finalList.size(); i++)
{
    cout << (finalList.at(i)).value << endl;
}
cerr << "Temps d'exécution : " << tac - tic << " secondes" << endl;
return EXIT_SUCCESS;
}
