#ifndef TYPES_HPP
#define TYPES_HPP
#include <gmpxx.h>
#include <vector>
class custom_mpz_t
{

  public:
    mpz_t value;
    custom_mpz_t()
    {
        mpz_inits(value, NULL);
    }
    custom_mpz_t(unsigned int op)
    {
        mpz_set_ui(value, op);
    }
    custom_mpz_t(const mpz_t op)
    {
        mpz_set(value, op);
    }
    custom_mpz_t(const custom_mpz_t &other)
    {
        mpz_set(value, other.value);
    }
    custom_mpz_t &operator=(custom_mpz_t const &other)
    {
        if ((void *)this == (void *)&other)
            return *this;
        mpz_set(value, other.value);
        return *this;
    }

    custom_mpz_t operator-(custom_mpz_t op1)
    {
        custom_mpz_t res;
        mpz_sub(res.value, value, op1.value);
        return res;
    }
    custom_mpz_t operator-(unsigned int op2)
    {
        custom_mpz_t res;
        mpz_sub_ui(res.value, value, op2);
        return res;
    }
};
bool operator<(const custom_mpz_t &op1, const custom_mpz_t &op2) const
{
    if (mpz_cmp(value, other.value) < 0)
        return true;
    return false;
}
bool operator<=(const custom_mpz_t &op1, const custom_mpz_t &op2) const
{
    if (mpz_cmp(value, other.value) <= 0)
        return true;
    return false;
}

struct interval_t
{
    custom_mpz_t intervalle_haut;
    custom_mpz_t intervalle_bas;
};

typedef std::vector<interval_t> vect_of_intervalles_t;

#endif
