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

using namespace std;

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
    omp_set_nested(1);

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
    vector<Custom_mpz_t> finalList;

    // Init variables pour le parallele
    Custom_mpz_t debut;
    Custom_mpz_t fin;
    mpz_t offset;
    mpz_t nb_to_check_prime;
    unsigned long size_interval;
    int is_prime;
    // DEBUT DU PARALELLE
    #pragma omp parallel num_threads(8) private(nb_to_check_prime, debut, fin, offset, size_interval, is_prime)
    #pragma omp for 
    for (int i = 0; i < intervalles.size(); i++)
    {
        debut = intervalles.at(i).intervalle_bas;
        fin = intervalles.at(i).intervalle_haut;
        mpz_init(offset);
        mpz_sub(offset, fin.value, debut.value);
        if (mpz_fits_ulong_p(offset))
            size_interval = mpz_get_ui(offset);
        else
            cout << "La taille de l'intervalle ne peux pas rentrer dans un unsigned long" << endl;
    #pragma omp parallel num_threads(8) private(is_prime, nb_to_check_prime)
    #pragma omp for 
        for (int i = 0; i < size_interval; i += 1)
        {
            mpz_init(nb_to_check_prime);
            mpz_add(nb_to_check_prime, nb_to_check_prime, debut.value);
            mpz_add_ui(nb_to_check_prime, nb_to_check_prime, i);
            is_prime = mpz_probab_prime_p(nb_to_check_prime, 20); //determine if nb is prime. probability of error < 4^(-20)
            if (is_prime == 1 || is_prime == 2)
            {
#pragma omp critical
                finalList.push_back(nb_to_check_prime); //number is certainly prime or probably prime
            }
        }
        
    }
    sort(finalList.begin(), finalList.end());
    float tac = chron.get();
    for (int i = 0; i < finalList.size(); i++)
    {
        cout << (finalList.at(i)).value << endl;
    }
    cerr << "Temps d'exécution : " << tac - tic << " secondes" << endl;
    return EXIT_SUCCESS;
}
