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
    sort(intervalles.begin(), intervalles.end(), mpz_compare);

    //supprime les chevauchements d'intervalles
    for (int i = 0; i < intervalles.size() - 1; i++)
    {
        if ((intervalles.at(i + 1)).intervalle_bas <= (intervalles.at(i)).intervalle_haut)
        {
            if ((intervalles.at(i + 1)).intervalle_haut > (intervalles.at(i)).intervalle_haut)
            {
                //les intervalles se chevauchent mais ne sont pas inclus l'un dans l'autre
                (intervalles.at(i)).intervalle_haut = (intervalles.at(i + 1)).intervalle_bas - 1;
            }
            else
            {
                //l intervalle i+1 est inclus dans l'intervalle i
                intervalles.erase(intervalles.begin() + (i + 1));
                if (i > 0)
                    i--; //Il faut cette fois comparer l'intervalle i à l'intervalle i+2
            }
        }
        else
        {
            //les intervalles ne se chevauchent pas
            //do nothing
        }
    }
}

void *compute_intervalles(void *parametre)
{
    param_thread_t *input_thread = (param_thread_t *)parametre;
    Custom_mpz_t nb_to_check_prime;
    int is_prime;
    for (int i = 0; i < input_thread->intervalle.size(); i++)
    {
        nb_to_check_prime = input_thread->intervalle.at(i).intervalle_bas;
        while (nb_to_check_prime < input_thread->intervalle.at(i).intervalle_haut)
        {
            is_prime = mpz_probab_prime_p(nb_to_check_prime.value, 20); //determine if nb is prime. probability of error < 4^(-20)
            if (is_prime == 1 || is_prime == 2)                         //number is certainly prime or probably prime
            {
                (input_thread->outputList).push_back(nb_to_check_prime);
            }
            nb_to_check_prime = nb_to_check_prime + (unsigned int)1;
        }
    }
    pthread_exit(NULL);
}

void compute_intervalle(interval_t intervalle, vector<Custom_mpz_t> outputVector)
{
    Custom_mpz_t nb_to_check_prime;
    int is_prime;
    {
        nb_to_check_prime = intervalle.intervalle_bas;
        while (nb_to_check_prime < intervalle.intervalle_haut)
        {
            is_prime = mpz_probab_prime_p(nb_to_check_prime.value, 20); //determine if nb is prime. probability of error < 4^(-20)
            if (is_prime == 1 || is_prime == 2)                         //number is certainly prime or probably prime
            {
                outputVector.push_back(nb_to_check_prime);
            }
            nb_to_check_prime = nb_to_check_prime + (unsigned int)1;
        }
    }
}
