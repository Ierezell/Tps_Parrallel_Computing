#include "Types.hpp"

bool mpz_compare(mpz_t big_nb_1, mpz_t big_nb_2)
{
    if (mpz_cmp(big_nb_1, big_nb_2) < 0)
        return true;
    return false;
}

void sort_and_prune(vect_of_intervalles_t *intervalles)
{
    // trie les intervalles dans l'ordre croissant des bornes inférieures
    // pour chaque intervalle, si borne sup[i] < borne inf[i+1]
    // borne sup[i] = borne inf[i+1] - 1

    std::sort(intervalles->begin(), intervalles->end(), mpz_compare);
    mpz_t inter_bas;
    mpz_t inter_bas_minus_one;

    mpz_t next_inter_haut;
    for (int i = 0; i < intervalles->size() - 1; i++)
    {
        mpz_set(inter_bas, intervalles->at(i + 1).intervalle_bas);
        mpz_set(next_inter_haut, intervalles->at(i).intervalle_haut);
        if (next_inter_haut >= inter_bas)
        {
            mpz_sub_ui(inter_bas_minus_one, inter_bas, (unsigned long)1);
            mpz_set(inter_haut, inter_bas_minus_one); // borne sup[i] = borne inf[i+1] - 1
        }
    }
}

// void compute_intervals(vect_of_intervalles_t intervalles_bas, vect_of_intervalles_t intervalles_haut)
// {
//     mpz_t nb_to_check_prime, inc;
//     mpz_set_str(inc, "1", 10);
//     int is_prime;
//     for (int i = 0; i < intervalles_bas.size()-1; i++)
//     {
//         mpz_set(nb_to_check_prime, intervalles_bas.at(i));
//         while (mpz_cmp(nb_to_check_prime, intervalles_haut.at(i)) < 0)
//         {
//             is_prime = mpz_probab_prime_p(nb_to_check_prime, 50); //determine if nb is prime. probability of error < 4^(-50)
//             if (is_prime == 1 || is_prime == 2)                   //number is certainly prime or probably prime
//             {
//                 cout << nb_to_check_prime << endl;
//             }
//             mpz_add(nb_to_check_prime, nb_to_check_prime, inc); //nb = nb + 1
//         }
//     }
// }
