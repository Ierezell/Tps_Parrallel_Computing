#ifndef TYPES_HPP
#define TYPES_HPP
#include <gmpxx.h>
#include <vector>

class Custom_mpz_t
{
public:
  mpz_t value;
  Custom_mpz_t(void);
  Custom_mpz_t(Custom_mpz_t const &other);
  Custom_mpz_t(mpz_t const &op);
  Custom_mpz_t(unsigned int op);

  Custom_mpz_t &operator=(Custom_mpz_t const &other);
  Custom_mpz_t operator-(Custom_mpz_t const &op1);
  Custom_mpz_t operator-(unsigned int op2);
  Custom_mpz_t operator+(Custom_mpz_t const &op1);
  Custom_mpz_t operator+(unsigned int op2);

  bool operator==(Custom_mpz_t const &op2);
  bool operator>(const Custom_mpz_t &op2);
  bool operator>=(const Custom_mpz_t &op2);
  bool operator<(const Custom_mpz_t &op2);
  bool operator<=(const Custom_mpz_t &op2);
};

typedef std::vector<interval_t> vect_of_intervalles_t;

struct interval_t
{
  Custom_mpz_t intervalle_haut;
  Custom_mpz_t intervalle_bas;
};

struct param_thread_t
{
  int inputNumeroThread;
  interval_t intervalle;
  std::vector<Custom_mpz_t> outputList;
};
#endif
