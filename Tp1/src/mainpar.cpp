#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <gmp.h>
#include <typeinfo>
#include <string>
#include "Types.hpp"
#include "Compute.hpp"
using namespace std;
int main(int argc, char *argv[])
{
    // Check for correct usage
    if (argc <= 1 || argc > 2)
    {
        cout << "Usage : " << argv[0] << "<fichier.txt>.\n";
        return EXIT_FAILURE;
    }
    ifstream prime_nb_file;
    prime_nb_file.open(argv[2]);
    if (prime_nb_file.is_open() == 0)
    {
        cout << "Impossible d'ouvrir le fichier.\n";
        return EXIT_FAILURE;
    }
    string line;

    vect_of_intervals_t intervals;
    while (getline(prime_nb_file, line))
    {
        intervalle_t buffer;
        gmp_sscanf(line.c_str(), "%Zd %Zd", buffer[0], buffer[1]);
        intervals.push_back(buffer)
    }
    sort_and_prune(intervals);
}
