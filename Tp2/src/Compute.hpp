#ifndef COMPUTE_HPP
#define COMPUTE_HPP

#include "Types.hpp"

bool mpz_compare(mpz_t big_nb_1, mpz_t big_nb_2);

void swap_intervalle(vect_of_intervalles_t &intervalles);

void sort_and_prune(vect_of_intervalles_t &intervalles);


#endif //COMPUTE_HPP
