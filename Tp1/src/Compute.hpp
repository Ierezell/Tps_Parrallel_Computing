#ifndef COMPUTE_HPP
#define COMPUTE_HPP

#include "Types.hpp"

bool mpz_compare(mpz_t big_nb_1, mpz_t big_nb_2);
void sort_and_prune(vect_of_intervalles_t *intervalles_bas, vect_of_intervalles_t *intervalles_haut);
void compute_intervals(vect_of_intervalles_t *addr_intervals);

#endif //COMPUTE_HPP
