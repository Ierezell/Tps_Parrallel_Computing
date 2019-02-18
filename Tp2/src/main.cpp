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

    // pthread_t Ids_threads[nb_threads];
    // param_thread_t params_threads[nb_threads];
    // vect_of_intervalles_t buffer;
    // vect_of_intervalles_t::const_iterator first_elt_buffer;
    // vect_of_intervalles_t::const_iterator last_elt_buffer;
    // for (int i = 0; i < nb_threads; i++)
    // {
    //     first_elt_buffer = intervalles.begin() + i * (intervalles.size() / nb_threads);
    //     last_elt_buffer = intervalles.begin() + ((i + 1) * (intervalles.size() / nb_threads)) - 1;
    //     vect_of_intervalles_t buffer(first_elt_buffer, last_elt_buffer + 1);
    //     params_threads[i].intervalle = buffer;
    //     params_threads[i].inputNumeroThread = i;
    //     pthread_create(&Ids_threads[i], NULL, compute_intervalles, (void *)&(params_threads[i]));
    // }

    // //attendre la fin des threads
    // vector<Custom_mpz_t> finalList;
    // for (int i = 0; i < nb_threads; i++)
    // {
    //     pthread_join(Ids_threads[i], NULL);
    //     finalList.insert(finalList.end(),
    //                      std::make_move_iterator((params_threads[i].outputList).begin()),
    //                      std::make_move_iterator((params_threads[i].outputList).end()));
    // }

    //FIN DU PARALELLE

    float tac = chron.get();
    for (int i = 0; i < finalList.size(); i++)
    {
        cout << (finalList.at(i)).value << endl;
    }
    cerr << "Temps d'exécution : " << tac - tic << " secondes" << endl;
    return EXIT_SUCCESS;
}
