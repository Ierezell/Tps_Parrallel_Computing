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

    Chrono chron = Chrono();
    float tic = chron.get();
    // Lis le fichier et sauvegarde les intervalles
    string line;
    vect_of_intervalles_t intervalles;
    while (getline(prime_nb_file, line))
    {
        interval_t buffer;
        gmp_sscanf(line.c_str(), "%Zd %Zd", buffer.intervalle_bas.value, buffer.intervalle_haut.value);
        intervalles.push_back(buffer);
    }
    swap_intervalle(intervalles);
    sort_and_prune(intervalles);

    pthread_t Ids_threads[nb_threads];
    param_thread_t params_threads[nb_threads];
    vect_of_intervalles_t buffer;
    vect_of_intervalles_t::const_iterator first_elt_buffer;
    vect_of_intervalles_t::const_iterator last_elt_buffer;

    cout << "Lancement des threads " << endl;

    for (int i = 0; i < nb_threads; i++)
    {
        first_elt_buffer = intervalles.begin() + i * (intervalles.size() / nb_threads);
        last_elt_buffer = intervalles.begin() + ((i + 1) * (intervalles.size() / nb_threads)) - 1;
        vect_of_intervalles_t buffer(first_elt_buffer, last_elt_buffer + 1);
        // cout << "buffer " << endl;
        // for (int i = 0; i < buffer.size(); i++)
        // {
        //     cout << "intervalle bas " << buffer.at(i).intervalle_bas.value << endl;
        //     cout << "intervalle haut" << buffer.at(i).intervalle_haut.value << endl;
        // }
        params_threads[i].intervalle = buffer;
        params_threads[i].inputNumeroThread = i;
        cout << "Lancement du thread " << i << endl;
        pthread_create(&Ids_threads[i], NULL, comp_int, (void *)&(params_threads));
    }

    //attendre la fin des threads
    vector<Custom_mpz_t> finalList;
    for (int i = 0; i < nb_threads; i++)
    {
        pthread_join(Ids_threads[i], NULL);
        // cout << "Un thread a renvoyé la valeur " << endl;
        // for (int i = 0; i < output->size(); i++)
        // {
        //     cout << "intervalle bas " << output->at(i).intervalle_bas.value << endl;
        //     cout << "intervalle haut" << output->at(i).intervalle_haut.value << endl;
        // }
        finalList.insert(finalList.end(),
                         std::make_move_iterator((params_threads[i].outputList).begin()),
                         std::make_move_iterator((params_threads[i].outputList).end()));
    }
    float tac = chron.get();
    for (int i = 0; i < finalList.size(); i++)
    {
        cout << (finalList.at(i)).value << endl;
    }
    cerr << "temps d'execution : " << tac - tic << " secondes" << endl;
    return EXIT_SUCCESS;
}
