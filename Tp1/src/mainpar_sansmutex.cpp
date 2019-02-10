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
#include "Chrono.hpp"
using namespace std;

void *compute_intervalle_thread(void *arg)
{
    vect_of_intervalles_t *parametre = (vect_of_intervalles_t *)arg; //recuperation des arguments transmis au thread
    cout << "Coucou, je suis le thread " << pthread_self();
    cout << " et j ai recu" << endl;
    for (int i = 0; i < parametre->size(); i++)
    {
        cout << "intervalle bas " << parametre->at(i).intervalle_bas.value << endl;
        cout << "intervalle haut" << parametre->at(i).intervalle_haut.value << endl;
    }
    parametre->at(0).intervalle_haut = 42; //modification de la valeur de la structure: peut etre utilisé pour la sortie du thread
    parametre->at(0).intervalle_bas = 56;
    pthread_exit((void *)&parametre);
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
    vect_of_intervalles_t buffer;
    vect_of_intervalles_t::const_iterator first;
    vect_of_intervalles_t::const_iterator last;
    cout << "Lancement des threads " << endl;
    for (int i = 0; i < nb_threads; i++)
    {
        first = intervalles.begin() + i * 2;
        cout << "first " << endl;
        last = intervalles.begin() + ((i + 1) * 2) - 1;
        cout << "last " << endl;
        vect_of_intervalles_t buffer(first, last);
        cout << "buffer " << endl;
        for (int i = 0; i < buffer.size(); i++)
        {
            cout << "intervalle bas " << buffer.at(i).intervalle_bas.value << endl;
            cout << "intervalle haut" << buffer.at(i).intervalle_haut.value << endl;
        }
        pthread_create(&Ids_threads[i], NULL, compute_intervalle_thread, (void *)&(buffer));
    }

    //attendre la fin des threads
    for (int i = 0; i < nb_threads; i++)
    {
        vect_of_intervalles_t *output;
        pthread_join(Ids_threads[i], (void **)&output);
        cout << "Un thread a renvoyé la valeur " << endl;
        for (int i = 0; i < output->size(); i++)
        {
            cout << "intervalle bas " << (output->at(i).intervalle_bas).value << endl;
            cout << "intervalle haut" << (output->at(i).intervalle_haut).value << endl;
        }
    }
    float tac = chron.get();
    cerr << "temps d'execution : " << tac - tic << " secondes" << endl;
    return EXIT_SUCCESS;
}
