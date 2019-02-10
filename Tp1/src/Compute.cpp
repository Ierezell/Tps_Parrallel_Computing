#include "Types.hpp"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <gmp.h>
#include <typeinfo>
#include <string>
#include <gmpxx.h>
using namespace std;

bool mpz_compare(interval_t &a, interval_t &b)
{
    if (a.intervalle_bas < b.intervalle_bas)
        return true;
    return false;
}

void swap_intervalle(vect_of_intervalles_t &intervalles)
{
    //gere le cas ou les intervalles haut et bas sont inversés
    Custom_mpz_t tmp;
    for (int i = 0; i < intervalles.size(); i++)
    {
        if ((intervalles.at(i)).intervalle_bas > (intervalles.at(i)).intervalle_haut)
        //l'intervalle est à l'envers
        {
            tmp = (intervalles.at(i)).intervalle_bas;
            (intervalles.at(i)).intervalle_bas = (intervalles.at(i)).intervalle_haut;
            (intervalles.at(i)).intervalle_haut = tmp;
        }
    }
}

void sort_and_prune(vect_of_intervalles_t &intervalles)
{

    // trie les intervalles dans l'ordre croissant des bornes inférieures
    cout << "liste pas triee" << endl;
    for (int i = 0; i < intervalles.size(); i++)
    {
        cout << "[";
        cout << (intervalles.at(i)).intervalle_bas.value << ",";
        cout << (intervalles.at(i)).intervalle_haut.value << "]" << endl;
    }

    sort(intervalles.begin(), intervalles.end(), mpz_compare);
    cout << "liste triee" << endl;
    for (int i = 0; i < intervalles.size(); i++)
    {
        cout << "[";
        cout << (intervalles.at(i)).intervalle_bas.value << ",";
        cout << (intervalles.at(i)).intervalle_haut.value << "]" << endl;
    }

    //supprime les chevauchements d'intervalles
    for (int i = 0; i < intervalles.size() - 1; i++)
    {
        cout << "avant traitement " << endl;
        cout << "i=" << i << " , intervalle i = [";
        cout << (intervalles.at(i)).intervalle_bas.value << ",";
        cout << (intervalles.at(i)).intervalle_haut.value << "]" << endl;

        cout << "intervalle i +1 = [";
        cout << (intervalles.at(i + 1)).intervalle_bas.value << ",";
        cout << (intervalles.at(i + 1)).intervalle_haut.value << "]" << endl;

        if ((intervalles.at(i + 1)).intervalle_bas <= (intervalles.at(i)).intervalle_haut)
        {
            if ((intervalles.at(i + 1)).intervalle_haut > (intervalles.at(i)).intervalle_haut)
            {
                //les intervalles se chevauchent mais ne sont pas inclus l'un dans l'autre
                cout << "les intervalles se chevauchent mais ne sont pas inclus l'un dans l'autre" << endl;
                (intervalles.at(i)).intervalle_haut = (intervalles.at(i + 1)).intervalle_bas - 1;
            }
            else
            {
                //l intervalle i+1 est inclus dans l'intervalle i
                cout << "l intervalle i+1 est inclus dans l'intervalle i" << endl;
                intervalles.erase(intervalles.begin() + (i + 1));
                if (i > 0)
                    i--; //Il faut cette fois comparer l'intervalle i à l'intervalle i+2
            }
        }
        else
        {
            //les intervalles ne se chevauchent pas
            cout << "les intervalles ne se chevauchent pas " << endl;
            //do nothing
        }
        cout << "apres traitement " << endl;
        cout << "i=" << i << " , intervalle i = [";
        cout << (intervalles.at(i)).intervalle_bas.value << ",";
        cout << (intervalles.at(i)).intervalle_haut.value << "]" << endl;
    }
    cout << "liste Finale" << endl;
    for (int i = 0; i < intervalles.size(); i++)
    {
        cout << "[";
        cout << (intervalles.at(i)).intervalle_bas.value << ",";
        cout << (intervalles.at(i)).intervalle_haut.value << "]" << endl;
    }
}

void compute_intervalles(vect_of_intervalles_t const &intervalles)
{
    Custom_mpz_t nb_to_check_prime;
    //mpz_init(nb_to_check_prime);
    int is_prime;
    cout << "Avant le for" << endl;
    for (int i = 0; i < intervalles.size(); i++)
    {
        cout << i << " eme intervalle de la boucle " << endl;
        nb_to_check_prime = (intervalles.at(i)).intervalle_bas;
        while (nb_to_check_prime < (intervalles.at(i)).intervalle_haut)
        {
            is_prime = mpz_probab_prime_p(nb_to_check_prime.value, 20); //determine if nb is prime. probability of error < 4^(-20)
            if (is_prime == 1 || is_prime == 2)                         //number is certainly prime or probably prime
            {
                cout << "Je suis premier !   " << nb_to_check_prime.value << endl;
            }
            nb_to_check_prime = nb_to_check_prime + (unsigned int)1;
        }
    }
}

void compute_intervalle(interval_t const &intervalle, struct param_thread * parametre)
{
    Custom_mpz_t nb_to_check_prime;
    //mpz_init(nb_to_check_prime);
    int is_prime;

    nb_to_check_prime = intervalle.intervalle_bas;
    while (nb_to_check_prime < intervalle.intervalle_haut)
    {
        is_prime = mpz_probab_prime_p(nb_to_check_prime.value, 20); //determine if nb is prime. probability of error < 4^(-20)
        if (is_prime == 1 || is_prime == 2)                         //number is certainly prime or probably prime
        {
            //cout << nb_to_check_prime.value << endl;
            (parametre->outputList).push_back(nb_to_check_prime);
        }
        nb_to_check_prime = nb_to_check_prime + (unsigned int)1;
    }

}
