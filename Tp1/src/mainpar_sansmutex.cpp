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

void *compute_intervalle_thread(void *arg)
{
    param_thread_t *parametre = (param_thread_t *)arg; //recuperation des arguments transmis au thread
    cout << "Coucou, je suis le thread " << pthread_self();
    cout << " et j ai recu" << endl;
    for (int i = 0; i < parametre->intervalle.size(); i++)
    {
        compute_intervalle(intervalleThread, parametre);
        cout << "intervalle bas " << parametre->intervalle.at(i).intervalle_bas.value << endl;
        cout << "intervalle haut" << parametre->intervalle.at(i).intervalle_haut.value << endl;
    }

    pthread_exit();
}

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
    cout << "swap" << endl;
    swap_intervalle(intervalles);
    cout << "sort" << endl;
    sort_and_prune(intervalles);
    cout << "inter" << endl;
    // compute_intervalles(intervalles);
    pthread_t Ids_threads[nb_threads];
    param_thread_t params_threads[nb_threads];
    vect_of_intervalles_t buffer;
    vect_of_intervalles_t::const_iterator first;
    vect_of_intervalles_t::const_iterator last;
    cout << "Lancement des threads " << endl;
    for (int i = 0; i < nb_threads; i++)
    {
        first = intervalles.begin() + i * (intervalles.size() / nb_threads);
        cout << "first " << endl;
        last = intervalles.begin() + ((i + 1) * (intervalles.size() / nb_threads)) - 1;
        cout << "last " << endl;
        vect_of_intervalles_t buffer(first, last + 1);
        cout << "buffer " << endl;
        for (int i = 0; i < buffer.size(); i++)
        {
            cout << "intervalle bas " << buffer.at(i).intervalle_bas.value << endl;
            cout << "intervalle haut" << buffer.at(i).intervalle_haut.value << endl;
        }
        params_threads[i].intervalle = buffer;
        cout << "lancement thread " << i << endl;
        pthread_create(&Ids_threads[i], NULL, compute_intervalle_thread, (void *)&(params_threads));
    }

    //attendre la fin des threads
    for (int i = 0; i < nb_threads; i++)
    {
        vect_of_intervalles_t *output;
        pthread_join(Ids_threads[i], (void **)&output);
        cout << "Un thread a renvoyé la valeur " << endl;
        for (int i = 0; i < output->size(); i++)
        {
            cout << "intervalle bas " << output->at(i).intervalle_bas.value << endl;
            cout << "intervalle haut" << output->at(i).intervalle_haut.value << endl;
        }
    }
    float tac = chron.get();
    cerr << "temps d'execution : " << tac - tic << " secondes" << endl;
    return EXIT_SUCCESS;
}
