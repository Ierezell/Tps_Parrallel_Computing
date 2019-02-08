#include "Types.hpp"
#include <gmpxx.h>
#include <vector>

Custom_mpz_t::Custom_mpz_t(void) { mpz_inits(value, NULL); }
Custom_mpz_t::Custom_mpz_t(mpz_t const &op)
{
    mpz_inits(value, NULL);
    mpz_set(value, op);
}
Custom_mpz_t::Custom_mpz_t(unsigned int op)
{
    mpz_inits(value, NULL);
    mpz_set_ui(value, op);
}

Custom_mpz_t Custom_mpz_t::operator-(Custom_mpz_t const &op1)
{
    Custom_mpz_t res;
    mpz_sub(res.value, value, op1.value);
    return res;
}
Custom_mpz_t Custom_mpz_t::operator-(unsigned int op2)
{
    Custom_mpz_t res;
    mpz_sub_ui(res.value, value, op2);
    return res;
}
Custom_mpz_t &Custom_mpz_t::operator=(Custom_mpz_t const &other)
{
    if ((void *)this == (void *)&other)
        return *this;
    mpz_set(value, other.value);
    return *this;
}
Custom_mpz_t::Custom_mpz_t(Custom_mpz_t const &other)
{
    mpz_set(value, other.value);
}
Custom_mpz_t::~Custom_mpz_t(void)
{
    mpz_clear(value);
}
bool Custom_mpz_t::operator==(Custom_mpz_t const &op2)
{
    if (mpz_cmp(value, op2.value) == 0)
        return true;
    return false;
}
bool Custom_mpz_t::operator<(const Custom_mpz_t &op2)
{
    if (mpz_cmp(value, op2.value) < 0)
        return true;
    return false;
}
bool Custom_mpz_t::operator<=(const Custom_mpz_t &op2)
{
    if (mpz_cmp(value, op2.value) <= 0)
        return true;
    return false;
}
