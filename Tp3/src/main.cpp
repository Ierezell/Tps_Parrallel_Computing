//
//  main.cpp
//

#include "Matrix.hpp"
#include "mpi.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <mpi.h>

using namespace std;

// Inverser la matrice par la méthode de Gauss-Jordan; implantation séquentielle.
void invertSequential(Matrix &iA)
{

    // vérifier que la matrice est carrée
    assert(iA.rows() == iA.cols());
    // construire la matrice [A I]
    MatrixConcatCols lAI(iA, MatrixIdentity(iA.rows()));

    // traiter chaque rangée
    for (size_t k = 0; k < iA.rows(); ++k)
    {
        // trouver l'index p du plus grand pivot de la colonne k en valeur absolue
        // (pour une meilleure stabilité numérique).
        size_t p = k;
        double lMax = fabs(lAI(k, k));
        for (size_t i = k; i < lAI.rows(); ++i)
        {
            if (fabs(lAI(i, k)) > lMax)
            {
                lMax = fabs(lAI(i, k));
                p = i;
            }
        }
        // vérifier que la matrice n'est pas singulière
        if (lAI(p, k) == 0)
            throw runtime_error("Matrix not invertible");

        // échanger la ligne courante avec celle du pivot
        if (p != k)
            lAI.swapRows(p, k);

        double lValue = lAI(k, k);
        for (size_t j = 0; j < lAI.cols(); ++j)
        {
            // On divise les éléments de la rangée k
            // par la valeur du pivot.
            // Ainsi, lAI(k,k) deviendra égal à 1.
            lAI(k, j) /= lValue;
        }

        // Pour chaque rangée...
        for (size_t i = 0; i < lAI.rows(); ++i)
        {
            if (i != k)
            { // ...différente de k
                // On soustrait la rangée k
                // multipliée par l'élément k de la rangée courante
                double lValue = lAI(i, k);
                lAI.getRowSlice(i) -= lAI.getRowCopy(k) * lValue;
            }
        }
    }

    // On copie la partie droite de la matrice AI ainsi transformée
    // dans la matrice courante (this).
    for (unsigned int i = 0; i < iA.rows(); ++i)
    {
        iA.getRowSlice(i) = lAI.getDataArray()[slice(i * lAI.cols() + iA.cols(), iA.cols(), 1)];
    }
}

// Inverser la matrice par la méthode de Gauss-Jordan; implantation MPI parallèle.
void invertParallel(Matrix &iA)
{
    // vérifier que la matrice est carrée
    assert(iA.rows() == iA.cols());

    unsigned int matrix_size = iA.rows();

    //concatener la matrice iA à la matrice MatrixIdentity(iA.rows())
    Matrix lI = MatrixIdentity(matrix_size);
    MatrixConcatCols lIA(iA, lI);

    cout << "debug:\n"
        << lIA.str() << endl;

    MPI::Init();
    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    //chaque processeur se voit assigner les lignes d'indice k tel que k%world_rank == 0
    int k;
    struct maxloc{double value; int location;} maxloc_principal, truc;  //TODO: changer le nom de truc 


    for(k=0; k<matrix_size; k++ ){  //on parcourt les lignes ...
        //identifier parmi les lignes >= k la ligne q qui correspond au pivot maximal
            //le processus principal (n°1) récupère, pour toutes les lignes i>=k, 
            //l'élément A(i,k) avec un reduce et détermine le maximum parmi ces éléments (MAXLOC)
        truc.value = //?
        truc.location = //?
        MPI::COMM_WORLD.Reduce(&maxloc_principal,&truc,sizeof(double),MPI::DOUBLE_INT,MPI::MAXLOC,1);  

        //echange de la ligne k courante avec la ligne q du pivot
            //Si les lignes q et k appartiennent au meme processeur, operation immediate...

            //...sinon, communication point à point entre les 2 processus concernés

        //broadcast de la ligne k à tous les processeurs...

        //...qui calculent le facteur d'élimination l(ik) = A(ik)/A(kk)...  (equation 7.2)

        //...et effectuent l'élimination A(ij) = A(ij)-l(ik)A(kj)  (equation 7.3)

    }
    //On recontruit la matrice AI transformée à partir de toutes les lignes récupérées chez tous les processus...

    //Et on en extrait la partie droite qui correspond à l'inverse de A.

    cout << "coucou  " << world_rank << "  " << world_size << endl;
    MPI::Finalize();
}

// Multiplier deux matrices.
Matrix multiplyMatrix(const Matrix &iMat1, const Matrix &iMat2)
{

    // vérifier la compatibilité des matrices
    assert(iMat1.cols() == iMat2.rows());
    // effectuer le produit matriciel
    Matrix lRes(iMat1.rows(), iMat2.cols());
    // traiter chaque rangée
    for (size_t i = 0; i < lRes.rows(); ++i)
    {
        // traiter chaque colonne
        for (size_t j = 0; j < lRes.cols(); ++j)
        {
            lRes(i, j) = (iMat1.getRowCopy(i) * iMat2.getColumnCopy(j)).sum();
        }
    }
    return lRes;
}

int main(int argc, char **argv)
{

    srand((unsigned)time(NULL));

    unsigned int lS = 5;
    if (argc == 2)
    {
        lS = atoi(argv[1]);
    }

    MatrixRandom lA(lS, lS);
    // cout << "Matrice random:\n"
    //  << lA.str() << endl;

    Matrix lB(lA);
    invertSequential(lB);
    // cout << "Matrice inverse:\n"
    //  << lB.str() << endl;

    Matrix lRes = multiplyMatrix(lA, lB);
    // cout << "Produit des deux matrices:\n"
    //      << lRes.str() << endl;

    cout << "Erreur: " << lRes.getDataArray().sum() - lS << endl;
    invertParallel(lA);
    return 0;
}
