__kernel void compute_line(__global int mat_size, __global double *inputMat, __global double *outputMat)
{
    int rows_size = get_global_size(0); // NDRange(0) == lignes
    int cols_size = get_global_size(1); //NDRange(1) == colonnes
    int idx_row = get_global_id(0); // Indice du groupe qui soccupe de la ligne
    int idx_col = get_global_id(1); // Indice du groupe qui soccupe de la colonne
    int lidx_ligne = get_local_id(0);
    int lidx_cols = get_local_id(1);
    printf("COUCOU %d : %d\n",idx_row, idx_col);
    // traiter chaque rangée
    __local double ma_ligne[10];
    __local int idx_pivot;
    __local double val_pivot;
    // for (int col_idx = 0; col_idx < cols_size; ++col_idx)
    //     ma_ligne[(idx_row*cols_size)+col_idx] = inputMat[(idx_row*cols_size)+col_idx] ;
    barrier(CLK_LOCAL_MEM_FENCE);

    for (int idx_ligne = 0; idx_ligne < rows_size; ++idx_ligne)
    {
        // printf("LIGNE %d\n", idx_ligne);
        // trouver l'index pivot du plus grand pivot de la colonne idx_ligne en valeur absolue
        // (pour une meilleure stabilité numérique).

        val_pivot = -1000000;
        idx_pivot = 0;
        for(int i=idx_ligne; i<rows_size; i++)
            if (outputMat[(idx_ligne+i)*cols_size] > val_pivot) {
                val_pivot = outputMat[(idx_ligne+i)*cols_size];
                idx_pivot = i ;
            }
        barrier(CLK_LOCAL_MEM_FENCE);
        printf("%d : %d\n",idx_row,val_pivot);
        printf("%d : %d\n",idx_row,idx_pivot);
        // Pivot
        if (get_global_id(0) == idx_pivot)
            outputMat[(idx_row*cols_size)+idx_col] /= val_pivot;
        barrier(CLK_LOCAL_MEM_FENCE);

        // AUTRES
        // Coefficient i, j              -=
        outputMat[(idx_row*cols_size)+idx_col] -= outputMat[(idx_row*cols_size)+idx_ligne] * inputMat[(idx_pivot*cols_size)+idx_col];
        barrier(CLK_LOCAL_MEM_FENCE);

    }
} ;
