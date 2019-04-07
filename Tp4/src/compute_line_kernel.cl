__kernel void compute_line(__global int* nb_cols, __global double *inputMat, __global double *outputMat)
{
    int rows_size = get_global_size(0); // NDRange(0) == lignes
    int idx_row = get_global_id(0); // Indice du groupe qui soccupe de la ligne
    int lidx_ligne = get_local_id(0);
    int idx_col = get_global_id(1);
    int nombre_cols = *nb_cols;
    for (int col=0; col<nombre_cols; col++) {
        outputMat[(idx_row*nombre_cols)+col] = inputMat[(idx_row*nombre_cols)+col];
    }
    // traiter chaque rangée
    // __local double ma_ligne[10];
    __local int idx_pivot;
    __local double val_pivot;
    __private double factElim;
    // for (int col_idx = 0; col_idx < nombre_cols; ++col_idx)
    //     ma_ligne[(idx_row*nombre_cols)+col_idx] = inputMat[(idx_row*nombre_cols)+col_idx] ;
    // barrier(CLK_LOCAL_MEM_FENCE);
    for (int idx_ligne = 0; idx_ligne < rows_size; ++idx_ligne)
    {
        val_pivot = outputMat[(idx_ligne*nombre_cols)+idx_ligne];
        idx_pivot = idx_ligne;
        // trouver l'index pivot du plus grand pivot de la colonne idx_ligne en valeur absolue
        // (pour une meilleure stabilité numérique).

        // for(int i=idx_ligne; i<rows_size; i++)
        // {
        //     if (outputMat[(i*nombre_cols)+idx_ligne] > val_pivot)
        //     {
        //         val_pivot = outputMat[(i*nombre_cols)+idx_ligne];
        //         idx_pivot = i ;
        //     }
        // }
        // printf("**Ligne %d, Proc %d Indice : %d, Valeur : %lf\n ",idx_ligne,idx_row, idx_pivot, val_pivot);
        // barrier(CLK_LOCAL_MEM_FENCE);

        // Pivot
        if (idx_row == idx_pivot)
        {
            for(int col=0 ; col<nombre_cols; col++)
                outputMat[(idx_pivot*nombre_cols)+col] /= val_pivot;
        }
        barrier(CLK_LOCAL_MEM_FENCE);
        if(idx_row != idx_pivot)
        {
            factElim = outputMat[(idx_row*nombre_cols)+idx_ligne];
            printf("**Ligne %d, Proc %d FactElim : %lf\n ",idx_ligne,idx_row,factElim);
            for(int col=0 ; col<nombre_cols; col++) {
                printf("%lf - %lf * %lf\n", outputMat[(idx_row*nombre_cols)+col],factElim,outputMat[(idx_pivot*nombre_cols)+col]);
                outputMat[(idx_row*nombre_cols)+col] -=factElim * outputMat[(idx_pivot*nombre_cols)+col];
            }

        }
        barrier(CLK_LOCAL_MEM_FENCE);


    }

    //     // AUTRES
    //     // Coefficient i, j              -=
    //     outputMat[(idx_row*nombre_cols)+idx_col] -= outputMat[(idx_row*nombre_cols)+idx_ligne] * inputMat[(idx_pivot*cols_size)+idx_col];
    //     barrier(CLK_LOCAL_MEM_FENCE);

// }
} ;


// __kernel void hello(__global double *inputMat, __global double *outputMat)
// {
//     // traiter chaque rangée
//     for (size_t k = 0; k < iA.rows(); ++k)
//     {
//         // trouver l'index p du plus grand pivot de la colonne k en valeur absolue
//         // (pour une meilleure stabilité numérique).
//         size_t p = k;
//         double lMax = fabs(lAI(k, k));
//         for (size_t i = k; i < lAI.rows(); ++i)
//         {
//             if (fabs(lAI(i, k)) > lMax)
//             {
//                 lMax = fabs(lAI(i, k));
//                 p = i;
//             }
//         }
//         // vérifier que la matrice n'est pas singulière
//         if (lAI(p, k) == 0)
//             throw runtime_error("Matrix not invertible");

//         // échanger la ligne courante avec celle du pivot
//         if (p != k)
//             lAI.swapRows(p, k);

//         double lValue = lAI(k, k);
//         for (size_t j = 0; j < lAI.cols(); ++j)
//         {
//             // On divise les éléments de la rangée k
//             // par la valeur du pivot.
//             // Ainsi, lAI(k,k) deviendra égal à 1.
//             lAI(k, j) /= lValue;
//         }

//         // Pour chaque rangée...
//         for (size_t i = 0; i < lAI.rows(); ++i)
//         {
//             if (i != k)
//             {   // ...différente de k
//                 // On soustrait la rangée k
//                 // multipliée par l'élément k de la rangée courante
//                 double lValue = lAI(i, k);
//                 lAI.getRowSlice(i) -= lAI.getRowCopy(k) * lValue;
//             }
//         }
//     }
// } ;
