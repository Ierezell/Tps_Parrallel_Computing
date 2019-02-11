#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <gmp.h>
#include <typeinfo>
#include <string>
#include <gmpxx.h>
#include "Types.hpp"
#include <typeinfo>
#include "Compute.hpp"
#include "Chrono.hpp"
using namespace std;

int main(int argc, char *argv[])
{
    // Check for correct usage
    if (argc <= 2 || argc > 3)
    {
        cerr << "Usage : " << argv[0] << "<fichier.txt>.\n";
        return EXIT_FAILURE;
    }

    int nb_threads = atoi(argv[1]);

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

    pthread_t Ids_threads[nb_threads];
    param_thread_t params_threads[nb_threads];
    vect_of_intervalles_t buffer;
    vect_of_intervalles_t::const_iterator first_elt_buffer;
    vect_of_intervalles_t::const_iterator last_elt_buffer;
    for (int i = 0; i < nb_threads; i++)
    {
        first_elt_buffer = intervalles.begin() + i * (intervalles.size() / nb_threads);
        last_elt_buffer = intervalles.begin() + ((i + 1) * (intervalles.size() / nb_threads)) - 1;
        vect_of_intervalles_t buffer(first_elt_buffer, last_elt_buffer + 1);
        params_threads[i].intervalle = buffer;
        params_threads[i].inputNumeroThread = i;
        pthread_create(&Ids_threads[i], NULL, compute_intervalles, (void *)&(params_threads[i]));
    }

    //attendre la fin des threads
    vector<Custom_mpz_t> finalList;
    for (int i = 0; i < nb_threads; i++)
    {
        pthread_join(Ids_threads[i], NULL);
        finalList.insert(finalList.end(),
                         std::make_move_iterator((params_threads[i].outputList).begin()),
                         std::make_move_iterator((params_threads[i].outputList).end()));
    }
    float tac = chron.get();
    // for (int i = 0; i < finalList.size(); i++)
    // {
    //     cout << (finalList.at(i)).value << endl;
    // }
    cerr << "Temps d'exécution : " << tac - tic << " secondes" << endl;
    return EXIT_SUCCESS;
}
