#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <typeinfo>
#include <gmp.h>
#include <stdio.h>
#include "check_prime.hpp"
#include "Chrono.hpp"
#include <vector>
extern "C"
{
#include "get_prime.h"
}
using namespace std;

void insert(std::vector<int> &cont, int value)
{
    std::vector<int>::iterator iter = std::lower_bound(cont.begin(), cont.end(), value, std::greater<int>()); // find proper position in descending order
    cont.insert(iter, value);                                                                                 // insert before iterator it
}
int main(int argc, char *argv[])
{
    if (argc < 3 || argc >= 4)
    {
        cout << "Usage : " << argv[0] << "<nb_threads> <fichier.txt>.\n";
        return EXIT_FAILURE;
    }
    ifstream prime_nb_file;
    prime_nb_file.open(argv[2]);
    string line;
    std::vector<int> nb_prime_nb;
    mpz_t interbas, interhaut, nb, inc;
    mpz_inits(interbas, interhaut, nb, inc, NULL);
    mpz_set_str(inc, "1", 10);
    Chrono chron = Chrono();
    float tic = chron.get();
    if (prime_nb_file.is_open())
    {
        while (getline(prime_nb_file, line))
        {
            gmp_sscanf(line.c_str(), "%Zd %Zd", interbas, interhaut);
            mpz_set(nb, interbas);
            vector<mpz_class> nb_prime_nb;
            while (mpz_cmp(nb, interhaut) < 0)
            {
                int is_prime = mpz_probab_prime_p(nb, 50);
                if (is_prime == 1 || is_prime == 2)
                {
                    nb_prime_nb.push_back((mpz_class)nb);
                    cout << nb << endl;
                }
                mpz_add(nb, nb, inc);
            }
            for (auto i : nb_prime_nb)
                cout << i << ' ';
        }
        mpz_clears(interbas, interhaut, NULL);
        prime_nb_file.close();
    }
    else
    {
        cout << "Impossible d'ouvrir le fichier.\n";
        return EXIT_FAILURE;
    }
    float tac = chron.get();
    cout << tac - tic << "secondes" << endl;
}
