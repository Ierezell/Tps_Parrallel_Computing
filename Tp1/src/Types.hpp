#ifndef TYPES_HPP
#define TYPES_HPP
#include <gmpxx.h>
#include <vector>
struct interval_t
{
    mpz_t intervalle_haut;
    mpz_t intervalle_bas;
};

typedef std::vector<interval_t> vect_of_intervalles_t;
#endif
