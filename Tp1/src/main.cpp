#include <iostream>
#include <fstream>
#include <string>
//#include "get_prime.hpp"
extern "C"
{
  #include "get_prime.h"
}
using namespace std;


int main(int argc, char *argv[]){
    if(argc < 3 || argc >= 4){
        cout << "Usage : "<<argv[0]<<"<nb_threads> <fichier.txt>.\n";
        return EXIT_FAILURE;
    }
    //Ouvrir le .txt
    ifstream prime_nb_file;
    prime_nb_file.open(argv[2]);
    string line;
    if(prime_nb_file.is_open()){
        while(getline(prime_nb_file, line)){
            cout << line << endl;
            salutation();
            //get_prime(inter_bas, inter_haut);
        }
    }
    else{
        cout << "Impossible d'ouvrir le fichier.\n";
        return EXIT_FAILURE;
    }
    prime_nb_file.close();
}
