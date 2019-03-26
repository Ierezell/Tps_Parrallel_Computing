//
//  main.cpp
//

#include "Matrix.hpp"
#include "mpi.h"
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <mpi.h>
#include "Chrono.hpp" // Classe chronomètre pour le temps d'éxécution

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
    // Concatener la matrice à la matrice Identitée pour donner [ A I ]
    MatrixConcatCols matrice_et_id(matrice, MatrixIdentity(matrice.rows()));

    // Debut de l'algorithme
    for (size_t idx_ligne = 0; idx_ligne < matrice.rows(); ++idx_ligne)
    {
        // Répartit les indices des lignes dans les processeurs grâce à un map (dictionnaire)
        map<double, int> dic_Values_col;
        for (size_t idx_ligne_rest = idx_ligne; idx_ligne_rest < matrice.rows(); ++idx_ligne_rest)
            if ((world_rank == (idx_ligne_rest % world_size)))
                dic_Values_col.insert(pair<double, int>(matrice_et_id(idx_ligne_rest, idx_ligne), idx_ligne_rest));
            else if ((matrice.rows() - idx_ligne) < world_size)
                dic_Values_col.insert(pair<double, int>(-1000000, 0));

        struct ind_and_val
        {
            double value;
            int index;
        } pivot;

        // Chaque processus ayant ses lignes dans un map celles-ci sont ordonées.
        // On réduit donc sur le max de chaque process qui est le 1er element de son dictionnnaire
        MPI::COMM_WORLD.Allreduce(&dic_Values_col.rbegin()->first, &pivot, 1, MPI_DOUBLE_INT, MPI_MAXLOC);

        //Vérifier que la matrice n'est pas singulière : Erreur à 1e-14 près
        if ((pivot.value < 0.00000000000001) && (-0.00000000000001 < pivot.value))
            throw runtime_error("Matrix not invertible");

        // Échanger la ligne courante avec celle du pivot
        if (pivot.index != idx_ligne)
        {
            // Si l'indice de la ligne et celui du pivot appartiennent au meme processeur, operation immediate...
            if ((idx_ligne % world_size == pivot.index % world_size) && (pivot.index % world_size == world_rank))
            {
                // On échange et on normalise
                matrice_et_id.swapRows(pivot.index, idx_ligne);
                for (size_t j = 0; j < matrice_et_id.cols(); ++j)
                    matrice_et_id(idx_ligne, j) /= pivot.value;
            }
            // ... sinon, communication point à point entre les 2 processus concernés
            else
            {
                if ((idx_ligne % world_size) == world_rank)
                {
                    valarray<double> ligne_pas_pivot(matrice_et_id.getRowSlice(idx_ligne));
                    valarray<double> ligne_pivot_norm(ligne_pas_pivot);
                    MPI::COMM_WORLD.Send((void *)&ligne_pas_pivot[0], ligne_pas_pivot.size(), MPI::DOUBLE, pivot.index % world_size, 42);
                    MPI::COMM_WORLD.Recv((void *)&ligne_pivot_norm[0], ligne_pivot_norm.size(), MPI::DOUBLE, pivot.index % world_size, 666);
                    // On remplace les lignes (comme un matrice.swapRows)
                    matrice_et_id.getRowSlice(idx_ligne) = ligne_pivot_norm;
                    matrice_et_id.getRowSlice(pivot.index) = ligne_pas_pivot;
                }
                else if ((pivot.index % world_size) == world_rank)
                {
                    // On normalise le pivot avant de l'envoyer
                    for (size_t j = 0; j < matrice_et_id.cols(); ++j)
                        matrice_et_id(pivot.index, j) /= pivot.value;
                    valarray<double> ligne_pivot_norm(matrice_et_id.getRowSlice(pivot.index));
                    valarray<double> ligne_pas_pivot(ligne_pivot_norm);
                    MPI::COMM_WORLD.Recv((void *)&ligne_pas_pivot[0], ligne_pas_pivot.size(), MPI::DOUBLE, idx_ligne % world_size, 42);
                    MPI::COMM_WORLD.Send((void *)&ligne_pivot_norm[0], ligne_pivot_norm.size(), MPI_DOUBLE, idx_ligne % world_size, 666);
                    // On remplace les lignes (comme un matrice.swapRows)
                    matrice_et_id.getRowSlice(pivot.index) = ligne_pas_pivot;
                    matrice_et_id.getRowSlice(idx_ligne) = ligne_pivot_norm;
                }
            }
        }
        else
        {
            if ((pivot.index % world_size) == world_rank)
                for (size_t j = 0; j < matrice_et_id.cols(); ++j)
                    matrice_et_id(idx_ligne, j) /= pivot.value;
        }
        // Broadcast de la ligne idx_ligne (soit la ligne du pivot) à tous les autres processeurs...
        valarray<double> ligne_pivot_broadcast(matrice_et_id.getRowSlice(idx_ligne));
        MPI::COMM_WORLD.Bcast((void *)&ligne_pivot_broadcast[0], ligne_pivot_broadcast.size(), MPI_DOUBLE, idx_ligne % world_size);

        // Fait les opérations de gauss sur les lignes
        for (size_t idx_ligne_rest = 0; idx_ligne_rest < matrice_et_id.rows(); idx_ligne_rest++)
        {
            if (((idx_ligne_rest % world_size) == world_rank) && (idx_ligne_rest != idx_ligne))
            {
                //...qui calculent le facteur d'élimination l(ik) = A(ik)/A(kk)...  (equation 7.2)
                double fact_elim = matrice_et_id(idx_ligne_rest, idx_ligne);
                //...et effectuent l'élimination A(ij) = A(ij)-l(ik)A(kj)  (equation 7.3)
                for (size_t j = 0; j < matrice_et_id.cols(); ++j)
                    matrice_et_id(idx_ligne_rest, j) -= ligne_pivot_broadcast[j] * fact_elim;
            }
        }
    }
    // A la fin on broadcast to les résultats individuels pour avoir la matrice finalle totale
    for (size_t idx_ligne_rest = 0; idx_ligne_rest < matrice_et_id.rows(); idx_ligne_rest++)
    {
        valarray<double> ligne_modified = matrice_et_id.getRowSlice(idx_ligne_rest);
        MPI::COMM_WORLD.Bcast((void *)&ligne_modified[0], ligne_modified.size(), MPI_DOUBLE, idx_ligne_rest % world_size);
        matrice_et_id.getRowSlice(idx_ligne_rest) = ligne_modified;
    }
    MPI::Finalize();
    // On copie la partie droite de la matrice [ A I ] qui est devenue [I A^-1] ainsi transformée dans la matrice courante.
    for (size_t i = 0; i < matrice_et_id.rows(); ++i)
    {
        matrice.getRowSlice(i) = matrice_et_id.getDataArray()[slice(i * matrice_et_id.cols() + matrice.cols(), matrice.cols(), 1)];
    }
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

    unsigned int taille_mat = 5;
    if (argc == 2)
    {
        taille_mat = atoi(argv[1]);
    }

    MatrixRandom matrice(taille_mat, taille_mat);

    Matrix mat_Inv_Seq(matrice);
    Chrono chron = Chrono();
    float tic_seq = chron.get();
    invertSequential(mat_Inv_Seq);
    float tac_seq = chron.get();
    // cout << "Matrice inverse sequentielle:\n"
    //      << mat_Inv_Seq.str() << endl
    //      << endl;

    Matrix res_Seq = multiplyMatrix(matrice, mat_Inv_Seq);
    // cout << "Produit des deux matrices sequentielle:\n"
    //      << res_Seq.str() << endl
    //      << endl;

    cout << "Erreur sequentielle: " << res_Seq.getDataArray().sum() - taille_mat << endl
         << endl;
    cout << "Temps sequentiel : " << tac_seq - tic_seq << "secondes" << endl;

    Matrix mat_Inv_Par(matrice);
    float tic_par = chron.get();
    invertParallel(mat_Inv_Par);
    float tac_par = chron.get();
    // cout << "Matrice inverse parallele:\n"
    //      << mat_Inv_Par.str() << endl
    //      << endl;

    Matrix res_Par = multiplyMatrix(matrice, mat_Inv_Par);
    // cout << "Produit des deux matrices parallele:\n"
    //      << res_Par.str() << endl
    //      << endl;

    cout << "Erreur parallèle: " << res_Par.getDataArray().sum() - taille_mat << endl
         << endl;
    cout << "Temps parallele : " << tac_par - tic_par << "secondes" << endl;
    return 0;
}
