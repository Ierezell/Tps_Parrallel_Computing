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

pthread_mutex_t gLock = PTHREAD_MUTEX_INITIALIZER;
int gNumIntervalle = 0;
vect_of_intervalles_t gIntervalles; //defini comme variable globale pour etre disponible par tous les threads; protégé par un mutex

void *compute_intervalle_thread(void *arg)
{
    struct param_thread_t *parametre = (struct param_thread_t *)arg; //recuperation des arguments transmis au thread

    interval_t intervalleThread;
    int numIntervalleThread = 0;

    //Recupere un nouvel intervalle
    //Utilise un mutux pour s'assurer qu'un intervalle ne puisse pas être récupéré par 2 threads.
    while (numIntervalleThread < gIntervalles.size())
    {
        pthread_mutex_lock(&gLock);
        numIntervalleThread = gNumIntervalle;
        gNumIntervalle++;
        pthread_mutex_unlock(&gLock);

        if (numIntervalleThread < gIntervalles.size())
        {
            intervalleThread = gIntervalles.at(numIntervalleThread);
            cerr << "Le thread n° " << parametre->inputNumeroThread << " a recupere l'intervalle n° " << numIntervalleThread << endl;
            //calcule les nombres premiers dans intervalleThread
            compute_intervalles(intervalleThread, parametre);
        }
        else
        {
            pthread_exit(NULL);
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    // Vérifie le nombre d'arguments
    if (argc <= 2 || argc > 3)
    {
        cerr << "Usage : " << argv[0] << "<fichier.txt>.\n";
        return EXIT_FAILURE;
    }

    int nb_threads = atoi(argv[1]);

    // Ouvre le fichier
    ifstream prime_nb_file;
    prime_nb_file.open(argv[2]);
    if (prime_nb_file.is_open() == 0)
    {
        cerr << "Impossible d'ouvrir le fichier.\n";
        return EXIT_FAILURE;
    }

    // Lis le fichier et sauvegarde les intervalles
    string line;
    while (getline(prime_nb_file, line))
    {
        interval_t buffer;
        gmp_sscanf(line.c_str(), "%Zd %Zd", buffer.intervalle_bas.value, buffer.intervalle_haut.value);
        gIntervalles.push_back(buffer);
    }

    //debut du traitement des intervalles; début du chronometre
    Chrono chron = Chrono();
    float tic = chron.get();

    swap_intervalle(gIntervalles);
    sort_and_prune(gIntervalles);

    // Lancement des threads

    pthread_t Ids_threads[nb_threads];
    struct param_thread_t params_threads[nb_threads];

    for (int i = 0; i < nb_threads; i++)
    {
        params_threads[i].inputNumeroThread = i; //initialisation des inputs de la structure transmise au thread
        pthread_create(&Ids_threads[i], NULL, compute_intervalle_thread, (void *)&(params_threads[i]));
    }

    //attend la fin des threads et concatene les vecteurs renvoyés dans finalList
    vector<Custom_mpz_t> finalList;
    for (int i = 0; i < nb_threads; i++)
    {
        struct param_thread_t *output;
        pthread_join(Ids_threads[i], NULL);
        finalList.insert(finalList.end(),
                         std::make_move_iterator((params_threads[i].outputList).begin()),
                         std::make_move_iterator((params_threads[i].outputList).end()));
    }
    //trie finalList dans l'ordre croissant
    sort(finalList.begin(), finalList.end());

    //traitement des intervalles terminé; fin du chronometre
    float tac = chron.get();

    //affichage des resultats dans stdout
    for (int i = 0; i < finalList.size(); i++)
    {
        cout << (finalList.at(i)).value << endl;
    }

    //affichage du temps d'execution dans stderr
    cerr << "temps d'execution : " << tac - tic << " secondes" << endl;

    return EXIT_SUCCESS;
}
