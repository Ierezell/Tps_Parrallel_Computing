#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <typeinfo>
#include <algorithm> //std::sort
#include <gmp.h>
#include <stdio.h>
#include "Chrono.hpp"
#include <vector>
#include <pthread.h>
#include <gmpxx.h>
#include "Types.hpp"
extern "C"
{
}
using namespace std;
void insert(vector<int> &cont, int value)
{
    vector<int>::iterator iter = lower_bound(cont.begin(), cont.end(), value, greater<int>()); // find proper position in descending order
    cont.insert(iter, value);                                                                  // insert before iterator it
}

bool mpz_cmp2(mpz_t op1, mpz_t op2)
{
    if (mpz_cmp(op1, op2) < 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}
int main(int argc, char *argv[])
{
    if (argc <= 1 || argc > 2)
    {
        cout << "Usage : " << argv[0] << "<fichier.txt>.\n";
        return EXIT_FAILURE;
    }
    //open txt file
    ifstream prime_nb_file;
    prime_nb_file.open(argv[1]);
    string line;
    mpz_t interbas, interhaut, nb, inc;
    mpz_inits(interbas, interhaut, nb, inc, NULL);
    mpz_set_str(inc, "1", 10); //inc prend la valeur 1, interprété en base 10
    Chrono chron = Chrono();
    float tic = chron.get();
    float tac;
    if (prime_nb_file.is_open())
    {
        vector<Custom_mpz_t> nb_prime_nb;
        while (getline(prime_nb_file, line))
        {
            gmp_sscanf(line.c_str(), "%Zd %Zd", interbas, interhaut); //lis les intervalles haut et bas d'une ligne
            mpz_set(nb, interbas);                                    //nb prend la valeur de interbas
            while (mpz_cmp(nb, interhaut) < 0)
            {
                int is_prime = mpz_probab_prime_p(nb, 20); //determine if nb is prime. probability of error < 4^(-50)
                if (is_prime == 1 || is_prime == 2)        //number is certainly prime or probably prime
                {
                    Custom_mpz_t tmp = Custom_mpz_t(nb);
                    nb_prime_nb.push_back(tmp);
                }
                mpz_add(nb, nb, inc); //nb = nb + 1
            }
        }
        sort(nb_prime_nb.begin(), nb_prime_nb.end()); //sort vector nb_prime_nb.   Not functionnal yet
        //print every element in vector nb_prime_nb
        tac = chron.get();
        for (auto i : nb_prime_nb)
        {
            cout << i.value << ' ';
        }

        mpz_clears(interbas, interhaut, NULL); //free the space used by variables
        prime_nb_file.close();
    }
    else
    {
        cerr << "Impossible d'ouvrir le fichier.\n";
        return EXIT_FAILURE;
    }
    
    cerr << tac - tic << " secondes" << endl;
}
// Il y a plusieurs petites erreurs dans votre code.

// Essayez d 'utiliser le Visualisateur pas-à-pas,
// cela va fort probablement vous faire comprendre vos erreurs(voir le fil #217)
//  !Si vous n ' y parvenez toujours pas, n ' hésitez pas à poser d' autres questions.
