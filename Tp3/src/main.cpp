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
    // Structure pour trouver le maximum
    typedef struct ind_and_val
    {
        MPI::FLOAT_INT value;
        MPI::DOUBLE_INT index;
    } ind_and_val;

    MPI::Init();
    MPI::Status status;
    // Nombre de processus
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Rang du processus
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    // Vérifier que la matrice est carrée
    assert(matrice.rows() == matrice.cols());
    // Concatener la matrice à la matrice Identitée [ A I ]
    MatrixConcatCols matrice_et_id(matrice, MatrixIdentity(matrice.rows()));
    ind_and_val pivot_index_val, ligne_ind_val;

    for (size_t idx_ligne = 0; idx_ligne < matrice.rows(); ++idx_ligne)
    {
        // Renvoie l'indice du maximum et sa valeur pour les lignes sous la ligne courante
        ligne_ind_val.index = idx_ligne;
        ligne_ind_val.value = matrice(idx_ligne, idx_ligne);
        MPI::COMM_WORLD.Allreduce(&pivot_index_val, &ligne_ind_val, sizeof(double), MPI::DOUBLE_INT, MPI::MAXLOC);
        int idx_pivot = pivot_index_val.index;
        double val_pivot = pivot_index_val.value;

        // Vérifier que la matrice n'est pas singulière
        if (matrice_et_id(idx_pivot, idx_ligne) == 0)
            throw runtime_error("Matrix not invertible");

        // Échanger la ligne courante avec celle du pivot
        if (idx_pivot != idx_ligne)
        {
            // Si l'indice de la ligne et celui du pivot appartiennent au meme processeur, operation immediate...
            if (idx_ligne % world_size == world_rank) //effectuée par le processus
                matrice_et_id.swapRows(idx_pivot, idx_ligne);
            // ... sinon, communication point à point entre les 2 processus concernés
            else
            {
                rank_pivot = idx_pivot % world_size;
                rank_ligne = idx_ligne % world_size;
                mData line_to_swap = matrice.getRowCopy(idx_ligne);
                MPI::COMM_WORLD.Sendrecv_replace((void *)line_to_swap, sizeof(mData), mData, rank_pivot, 0, rank_ligne, 0, MPI::COMM_WORLD, status);
                matrice.getRowSlice(idx_ligne) = line_to_swap;
            }
        }

        //broadcast de la ligne idx_ligne à tous les processeurs...
        if (idx_ligne % world_size == world_rank)
        {
            mData ligne_bcast = matrice.getRowCopy(idx_ligne);
            MPI::COMM_WORLD.Bcast((void *)ligne_bcast, 1, mData, world_rank);
        }
        int i = 0;
        for (i = 0; i < matrice.rows(); i++)
        {
            if (i % world_size == world_rank || i != idx_ligne)
            {
                //...qui calculent le facteur d'élimination l(ik) = A(ik)/A(kk)...  (equation 7.2)
                double fact_elim = matrice(idx_ligne, idx_ligne) / matrice(idx_ligne, idx_ligne);
                //...et effectuent l'élimination A(ij) = A(ij)-l(ik)A(kj)  (equation 7.3)
                matrice.getRowSlice(i) = matrice.getRowCopy(i) - fact_elim * matrice.getRowCopy(idx_ligne);
            }
            else if (i % world_size == world_rank || i == idx_ligne)
                matrice.getRowSlice(i) = matrice.getRowCopy(i) / matrice(idx_ligne, idx_ligne);
        }

        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //

        // On copie la partie droite de la matrice AI ainsi transformée
        // dans la matrice courante (this).
        for (unsigned int i = 0; i < matrice.rows(); ++i)
        {
            matrice.getRowSlice(i) = matrice_et_id.getDataArray()[slice(i * matrice_et_id.cols() + matrice.cols(), matrice.cols(), 1)];
        }

        //chaque processeur se voit assigner les lignes d'indice k tel que k%world_rank == 0
        int k;

        for (k = 0; k < matrix_size; k++)
        { //on parcourt les lignes ...
            //identifier parmi les lignes >= k la ligne q qui correspond au pivot maximal
            //le processus principal (n°1) récupère, pour toutes les lignes i>=k,
            //l'élément A(i,k) avec un reduce et détermine le maximum parmi ces éléments (MAXLOC)

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

    // if (idx_ligne % world_size == world_rank)
    // {
    //     mData ligne_pas_pivot = matrice.getRowCopy();
    //     mData ligne_pivot;
    //     MPI::COMM_WORLD.Send((void *)ligne_pas_pivot, sizeof(mData), mData, idx_pivot % world_size);
    //     MPI::COMM_WORLD.Recv((void *)ligne_pivot, sizeof(mData), mData, idx_pivot % world_size);
    // }
    // matrice_et_id.swapRows(idx_pivot, idx_ligne);
    // if (idx_pivot % world_size == world_rank)
    // {
    //     mData ligne_pivot;
    //     ;
    //     mData ligne_pas_pivot = matrice.getRowCopy();
    //     MPI::COMM_WORLD.Recv((void *)ligne_rcv, sizeof(mData), mData, idx_pivot % world_size);
    //     MPI::COMM_WORLD.Send((void *)ligne_send, sizeof(mData), mData, idx_ligne % world_size);
    // }
