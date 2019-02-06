#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <typeinfo>
#include <algorithm> //std::sort
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

bool mpz_cmp2(const mpz_t op1, const mpz_t op2)
{
    if (mpz_cmp(op1, op2)<0){ return true;}
    else{ return false;}
}
int main(int argc, char *argv[])
{
    //check number of arguments; must be 3
    if (argc < 3 || argc >= 4)
    {
        cerr << "Usage : " << argv[0] << "<nb_threads> <fichier.txt>.\n";
        return EXIT_FAILURE;
    }
    //open txt file
    ifstream prime_nb_file;
    prime_nb_file.open(argv[2]);

    int nb_threads = atoi(argv[1]);
    if (nb_threads < 1)
    {
        cerr << "nb_threads must be strictly positive." << endl;
        return EXIT_FAILURE;
    }

    string line;
    std::vector<int> nb_prime_nb;
    mpz_t interbas, interhaut, nb, inc;
    mpz_inits(interbas, interhaut, nb, inc, NULL);
    mpz_set_str(inc, "1", 10); //inc prend la valeur 1, interprété en base 10
    Chrono chron = Chrono();
    float tic = chron.get();
    if (prime_nb_file.is_open())
    {
        vector<mpz_class> nb_prime_nb;
        while (getline(prime_nb_file, line))
        {
            gmp_sscanf(line.c_str(), "%Zd %Zd", interbas, interhaut); //lis les intervalles haut et bas d'une ligne
            mpz_set(nb, interbas); //nb prend la valeur de interbas
            while (mpz_cmp(nb, interhaut) < 0)
            {
                int is_prime = mpz_probab_prime_p(nb, 50); //determine if nb is prime. probability of error < 4^(-50)
                if (is_prime == 1 || is_prime == 2) //number is certainly prime or probably prime
                {
                    nb_prime_nb.push_back((mpz_class)nb); //add nb to vector nb_prime_nb
                    //cout << nb << endl;   //debug
                }
                mpz_add(nb, nb, inc); //nb = nb + 1
            }
        }
        //std::sort(nb_prime_nb.begin(),nb_prime_nb.end(), mpz_cmp2);  //sort vector nb_prime_nb.   Not functionnal yet
        //print every element in vector nb_prime_nb
        for (auto i : nb_prime_nb)
        { 
            cout << i << ' ';
        }
        
        mpz_clears(interbas, interhaut, NULL); //free the space used by variables
        prime_nb_file.close();
    }
    else
    {
        cout << "Impossible d'ouvrir le fichier.\n";
        return EXIT_FAILURE;
    }
    float tac = chron.get();
    cerr << tac - tic << " secondes" << endl;
}
