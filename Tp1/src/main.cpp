#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <typeinfo>
#include <gmp.h>
#include <stdio.h>

//#include "get_prime.hpp"
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
    mpz_init(interbas);
    mpz_init(interhaut);
    //salutation();
    if (prime_nb_file.is_open())
    {
        while (getline(prime_nb_file, line))
        {
            cout << "coucou" << endl;
            cout << "j'ai la ligne  " << line << endl;
            //stringstream linestream(line);
            //cout << line << endl;
            // mpz_set_str
            cout << "Je vais choper les nombres" << endl;
            gmp_sscanf(line.c_str(), "%Zd %Zd", interbas, interhaut);
            //gmp_fscanf("", interbas, interhaut);
            //gmp_scanf("", interbas, interhaut);
            cout << "j'ai les nombres" << endl;
            cout << interbas << endl;
            cout << interhaut << endl;
            //mpz_clears(interbas, interhaut, NULL);
        }
    }
    else
    {
        cout << "Impossible d'ouvrir le fichier.\n";
        return EXIT_FAILURE;
    }
    prime_nb_file.close();
}
