#ifndef TYPES_HPP
#define TYPES_HPP
#include <gmpxx.h>
#include <vector>
typedef struct intervals
{
    mpz_t intervalle_haut;
    mpz_t intervalle_bas;
} interval_t;

typedef std::vector<interval_t> vect_of_intervalles_t;
#endif
