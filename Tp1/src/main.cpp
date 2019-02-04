#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <typeinfo>
#include <gmp.h>
#include <stdio.h>
#include "check_prime.hpp"
extern "C"
{
#include "get_prime.h"
}
using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 3 || argc >= 4)
    {
        cout << "Usage : " << argv[0] << "<nb_threads> <fichier.txt>.\n";
        return EXIT_FAILURE;
    }
    //Ouvrir le .txt
    ifstream prime_nb_file;
    prime_nb_file.open(argv[2]);
    string line;
    mpz_t interbas, interhaut;
    mpz_t nb;
    mpz_t inc;
    mpz_inits(interbas, interhaut, nb, inc, NULL);
    mpz_set_str(inc, "1", 10);

    //salutation();
    if (prime_nb_file.is_open())
    {
        while (getline(prime_nb_file, line))
        {
            cout << "WHiiiiiiiiiA" << endl;
            gmp_sscanf(line.c_str(), "%Zd %Zd", interbas, interhaut);
            cout << "oooooooooooo" << endl;
            mpz_set(nb, interbas);
            cout << "WHAAuuuuuuuuuuA" << endl;
            while (mpz_cmp(nb, interhaut) < 0)
            {
                cout << "WHAAAAAAAAAAAA" << endl;
                int is_prime = mpz_probab_prime_p(nb, 50);
                cout << nb << endl;
                cout << is_prime << endl;
                mpz_add(nb, nb, inc);
            }

            cout << "j'ai les nombres" << endl;
            cout << interbas << endl;
            cout << interhaut << endl;
        }
        mpz_clears(interbas, interhaut, NULL);
        prime_nb_file.close();
    }
    else
    {
        cout << "Impossible d'ouvrir le fichier.\n";
        return EXIT_FAILURE;
    }
}

// int main(int, char**)
// {
//   using namespace std;

//   for (size_t bits = 1; ; bits *= 2) {
//     const size_t rounds = (bits < 4) + bits / 4;

//     cout << "Finding " << bits << "-bit prime w/"
//       << rounds << " rounds ... " << flush;

//     mpz_class n = find_prime(bits, rounds);
//     cout << endl << n << endl << endl;
//   }

//   return 0;
// }
