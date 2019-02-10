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
vect_of_intervalles_t intervalles; //defined as global to be accessed by any threads

void *compute_intervalle_thread(void *arg)
{
    struct param_thread *parametre = (struct param_thread *)arg; //recuperation des arguments transmis au thread
    cout << "Coucou, je suis le thread " << pthread_self();
    cout << " et j ai recu la valeur " << parametre->inputValue << endl;
    parametre->outputValue = 42; //modification de la valeur de la structure: peut etre utilisé pour la sortie du thread

    interval_t intervalleThread;
    int numIntervalleThread = 0;

    //Get a new interval.
    //Use a mutex to guarantee that two threads cannot get the same interval.
    while (numIntervalleThread < intervalles.size())
    {
        pthread_mutex_lock(&gLock);
        numIntervalleThread = gNumIntervalle;
        gNumIntervalle++;
        pthread_mutex_unlock(&gLock);

        if (numIntervalleThread < intervalles.size())
        {
            intervalleThread = intervalles.at(numIntervalleThread);
            cout << "Le thread " << pthread_self() << " a recupere l'intervalle n° " << numIntervalleThread << endl;
            //compute prime numbers in intervalleThread
            compute_intervalle(intervalleThread, parametre);
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
    while (getline(prime_nb_file, line))
    {
        interval_t buffer;
        gmp_sscanf(line.c_str(), "%Zd %Zd", buffer.intervalle_bas.value, buffer.intervalle_haut.value);
        intervalles.push_back(buffer);
    }

    //debut du traitement des intervalles; début du chronometre
    Chrono chron = Chrono();
    float tic = chron.get();

    cout << "swap" << endl;
    swap_intervalle(intervalles);
    cout << "sort" << endl;
    sort_and_prune(intervalles);
    cout << "inter" << endl;
    // compute_intervalles(intervalles); //version sans threads
    // cout << "plop" << endl;

    // Lancement des threads
    // Donne 1/Nb_thread du tableau a chacun
    // Lance la fonction compute trhead pour chacun
    // Chaque thread print ses nombres donc osef
    // PB si le fichier d'entrée est trop gros !
    pthread_t Ids_threads[nb_threads];
    struct param_thread params_threads[nb_threads];

    cout << "Lancement des threads " << endl;     //debug
    for (int i = 0; i < intervalles.size(); i++)  //debug (toute la boucle for)
    {
        vect_of_intervalles_t buffer(intervalles.begin() + i, intervalles.begin() + i + 1);
        for (int i = 0; i < buffer.size(); i++)
        {
            cout << "intervalle bas       " << buffer.at(i).intervalle_bas.value << endl;
            cout << "intervalle haut       " << buffer.at(i).intervalle_haut.value << endl;
        }
        // cout << std::vector(intervalles.begin() + ((intervalles.size() / nb_threads) * i),
        //                     intervalles.begin() + ((intervalles.size() / nb_threads) * (i + 1)))
        //      << endl;
        //pthread_create(&Ids_threads[i], NULL, compute_intervalle_thread,
        //(void *)sub(&intervalles[(intervalle.size()/nb_threads)*i],
        // &intervalles[(intervalle.size()/nb_threads)*(i+1)]));
    }
    for (int i = 0; i < nb_threads; i++)
    {
        params_threads[i].inputValue = i + 1; //initialisation de la structure transmise au thread //debug
        pthread_create(&Ids_threads[i], NULL, compute_intervalle_thread, (void *)&(params_threads[i]));
    }

    //attend la fin des threads et concatene les vecteurs renvoyés dans finalList
    vector<Custom_mpz_t> finalList;
    for (int i = 0; i < nb_threads; i++)
    {
        struct param_thread *output;
        pthread_join(Ids_threads[i], NULL);
        cout << "Un thread a renvoyé la valeur " << (params_threads[i]).outputValue << endl;  //debug
        finalList.insert(finalList.end(),
                         std::make_move_iterator((params_threads[i].outputList).begin()),
                         std::make_move_iterator((params_threads[i].outputList).end())
                         );
    }
    sort(finalList.begin(), finalList.end());

    //traitement des intervalles terminé; fin du chronometre
    float tac = chron.get();
    cerr << "temps d'execution : " << tac - tic << " secondes" << endl;

    for(int i=0;i<finalList.size();i++)
    {
        cout << (finalList.at(i)).value <<endl;
    }

    return EXIT_SUCCESS;
}
