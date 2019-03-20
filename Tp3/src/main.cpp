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
void invertParallel(Matrix &matrice)
{
    // vérifier que la matrice est carrée
    assert(matrice.rows() == matrice.cols());

    //concatener la matrice iA à la matrice MatrixIdentity(iA.rows())
    // ==> construire la matrice [A I]
    MatrixConcatCols matrice_et_id(matrice, MatrixIdentity(matrice.rows()));

    //Pour chaque ligne "ligne" <=> traiter chaque rangée
    for (size_t ligne = 0; ligne < matrice.rows(); ++ligne)
    {
        // trouver l'index ligne_pivot du plus grand pivot.
        // Pour ce faire on regarde les lignes sous la précédentes.
        // Celles d'au dessus ont déjà été traitées et sont plus grandes.
        size_t ligne_pivot = ligne;
        double lMax_elt = fabs(matrice_et_id(ligne, ligne));
        for (size_t i = ligne; i < matrice_et_id.rows(); ++i)
        {
            if (fabs(matrice_et_id(i, ligne)) > lMax_elt)
            {
                lMax_elt = fabs(matrice_et_id(i, ligne));
                ligne_pivot = i;
            }
        }
        // vérifier que la matrice n'est pas singulière
        if (matrice_et_id(ligne_pivot, ligne) == 0)
            throw runtime_error("Matrix not invertible");

        // échanger la ligne courante avec celle du pivot
        if (ligne_pivot != ligne)
            matrice_et_id.swapRows(ligne_pivot, ligne);

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
    // trouver le maximum A(i,j); la colonne correspondante au max est Ci
    //Si ce maximum est nul, la matrice n'est pas inversible
    //En parallèle sur toutes les lignes en dessous (ligne k>i), effectuer l'operation Lk = Lk-(A(k,j)/A(i,j))Li
    //Une réorganisation des colonnes dans l'ordre C1, C2, ... Cn donne une matrice triangulaire
    //remontée: pour m allant de (n-1) à 1,
    //Lm = Lm- [A(m,m+1)L(m+1) - ... -A(m,n)L(n)]
    //On copie la partie droite dans la matrice courante

    //besoin d'adapter la plupart des méthodes de Matrix pour une version à p processeurs

    MPI::Init();
    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

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
    invertParallel();
    return 0;
}
