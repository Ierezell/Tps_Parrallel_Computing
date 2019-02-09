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

void *compute_intervalle_thread(void *input)
{
    (void *)input;
    cout << "Coucou, je suis le thread " << pthread_self() << endl;
    int value = 42;
    pthread_exit(&value);
}
int main(int argc, char *argv[])
{
    // Check for correct usage
    if (argc <= 2 || argc > 3)
    {
        cout << "Usage : " << argv[0] << "<fichier.txt>.\n";
        return EXIT_FAILURE;
    }

    int nb_threads = atoi(argv[1]);

    // Open the file
    ifstream prime_nb_file;
    prime_nb_file.open(argv[2]);
    if (prime_nb_file.is_open() == 0)
    {
        cout << "Impossible d'ouvrir le fichier.\n";
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
    cout << "swap" << endl;
    swap_intervalle(intervalles);
    cout << "sort" << endl;
    sort_and_prune(intervalles);
    cout << "inter" << endl;
    compute_intervalles(intervalles);
    cout << "plop" << endl;

    // Lancement des threads
    // Donne 1/Nb_thread du tableau a chacun
    // Lance la fonction compute trhead pour chacun
    // Chaque thread print ses nombres donc osef
    // PB si le fichier d'entrée est trop gros !
    long input = 10;
    pthread_t Ids_threads[nb_threads];
    cout << "Lancement des threads " << endl;
    for (int i = 0; i < intervalles.size(); i++)
    {
        vect_of_intervalles_t buffer(intervalles.begin() + i, intervalles.begin() + i + 1);
        for (int i = 0; i < buffer.size(); i++)
        {
            cout << "JJSJSJJSJJSJJSJJSJJSJJSJJSJ       " << buffer.at(i).intervalle_bas.value << endl;
            cout << "lkdfngmkjwdmkjshmkwjdhm<ksj       " << buffer.at(i).intervalle_haut.value << endl;
        }
        // cout << std::vector(intervalles.begin() + ((intervalles.size() / nb_threads) * i),
        //                     intervalles.begin() + ((intervalles.size() / nb_threads) * (i + 1)))
        //      << endl;
        //pthread_create(&Ids_threads[i], NULL, compute_intervalle_thread,
        //(void *)sub(&intervalles[(intervalle.size()/nb_threads)*i],
        // &intervalles[(intervalle.size()/nb_threads)*(i+1)]));
        pthread_create(&Ids_threads[i], NULL, compute_intervalle_thread, NULL);
    }
    //attendre la fin des threads
    cout << "Joignage" << endl;
    for (int i = 0; i < nb_threads; i++)
    {
        int *output;
        pthread_join(Ids_threads[i], (void **)&output);
        cout << "Un thread a renvoyé la valeur " << *output << endl;
    }
    cout << "Les threads ont fini de travailler" << endl;
}
