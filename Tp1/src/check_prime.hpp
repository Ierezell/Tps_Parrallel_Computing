
#include <gmpxx.h>
#include <stdint.h>

extern "C"
{
    int initialize_seed(const size_t bytes);
}
bool prob_prime(const mpz_class &n, const size_t rounds);
mpz_class pow_mod(mpz_class a, mpz_class x, const mpz_class &n);
mpz_class randint(const mpz_class &lowest, const mpz_class &highest);
static mpz_class find_prime(const size_t bits, const size_t rounds);
void delete_prng();
