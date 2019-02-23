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
