#include "Matrix.hpp"
#include <cstdio>
#include <ctime>
#include <iostream>
#include "Chrono.hpp" // Classe chronomètre pour le temps d'éxécution

using namespace std;

#define N 20000


// Inverser la matrice par la méthode de Gauss-Jordan; implantation séquentielle.
void invertMatrix(Matrix &iA)
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
        #pragma acc data //copyout(lAI, lValue)
        {
        #pragma acc parallel loop        
        for (size_t j = 0; j < lAI.cols(); ++j)
        {
            // On divise les éléments de la rangée k
            // par la valeur du pivot.
            // Ainsi, lAI(k,k) deviendra égal à 1.
            lAI(k, j) /= lValue;
        }

        // Pour chaque rangée...
        #pragma acc parallel loop 
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
    }

    // On copie la partie droite de la matrice AI ainsi transformée
    // dans la matrice courante (this).
    for (unsigned int i = 0; i < iA.rows(); ++i)
    {
        iA.getRowSlice(i) = lAI.getDataArray()[slice(i * lAI.cols() + iA.cols(), iA.cols(), 1)];
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

void dummy_function(){
    double a[N];
    double b[N];
    int i, j;

    #pragma acc parallel
    {
        #pragma acc loop independent
        for(i=0;i<N;i++){
            a[i] = i+1;
            b[i] = 2*i+0.1;
        }

        #pragma acc loop independent
        for(i=0;i<N;i++){
            for(j=0;j<N;j++){
                a[i] += 2*a[i]/b[j]+3;
            }
        }
    }
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
    Chrono chron = Chrono();
    Matrix mat_Inv(matrice);
    float tic = chron.get();
    //invertMatrix(mat_Inv);
    dummy_function();
    float tac = chron.get();
    // cout << "Matrice inverse sequentielle:\n"
    //      << mat_Inv.str() << endl
    //      << endl;

    cout << "Temps sequentiel : " << tac - tic << "secondes" << endl;

    // Matrix res = multiplyMatrix(matrice, mat_Inv);
    // cout << "Erreur sequentielle: " << res.getDataArray().sum() - taille_mat << endl
    //      << endl;
    

    // cout << "Produit des deux matrices:\n"
    //      << res.str() << endl
    //      << endl;

    return 0;
}


