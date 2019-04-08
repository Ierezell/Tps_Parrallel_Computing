// __kernel void compute_line(__global int* nb_cols, __global double *inputMat, __global double *outputMat)
// {
//     int rows_size = get_global_size(0); // NDRange(0) == lignes
//     int idx_row = get_global_id(0); // Indice du groupe qui soccupe de la ligne
//     int idx_col = get_global_id(1);
//     int nombre_cols = get_global_size(1);
//     outputMat[(idx_row*nombre_cols)+idx_col] = inputMat[(idx_row*nombre_cols)+idx_col];
//     barrier(CLK_LOCAL_MEM_FENCE);
//     __private double val_pivot;
//     __private double factElim;
//     __private int idx_ligne;
//     for (idx_ligne = 0; idx_ligne < rows_size; ++idx_ligne)
//     {
//         barrier(CLK_LOCAL_MEM_FENCE);

//         val_pivot = outputMat[(idx_ligne*nombre_cols)+idx_ligne];
//         barrier(CLK_LOCAL_MEM_FENCE);

//         // Pivot
//         if (idx_row == idx_ligne)
//             outputMat[(idx_ligne*nombre_cols)+idx_col] /= val_pivot;

//         barrier(CLK_LOCAL_MEM_FENCE);

//         if(idx_row != idx_ligne)
//         {
//             factElim = outputMat[(idx_row*nombre_cols)+idx_ligne];
//             barrier(CLK_LOCAL_MEM_FENCE);

//             printf("%d, %d, %lf - %lf* %lf\n",idx_row,idx_col,outputMat[(idx_row*nombre_cols)+idx_col],factElim, outputMat[(idx_ligne*nombre_cols)+idx_col]);

//             outputMat[(idx_row*nombre_cols)+idx_col] -= factElim * outputMat[(idx_ligne*nombre_cols)+idx_col];
//             barrier(CLK_LOCAL_MEM_FENCE);

//         }
//         barrier(CLK_LOCAL_MEM_FENCE);
//         printf("\n");
//     }
// };




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
    __local int idx_pivot;
    __local double val_pivot;
    __private double factElim;
    for (int idx_ligne = 0; idx_ligne < rows_size; ++idx_ligne)
    {
        val_pivot = outputMat[(idx_ligne*nombre_cols)+idx_ligne];

        // Pivot
        if (idx_row == idx_ligne)
        {
            for(int col=0 ; col<nombre_cols; col++)
                outputMat[(idx_ligne*nombre_cols)+col] /= val_pivot;
        }
        // barrier(CLK_LOCAL_MEM_FENCE);
        if(idx_row != idx_ligne)
        {
            factElim = outputMat[(idx_row*nombre_cols)+idx_ligne];
            for(int col=0 ; col<nombre_cols; col++) {
                outputMat[(idx_row*nombre_cols)+col] -=factElim * outputMat[(idx_ligne*nombre_cols)+col];
            }

        }
        // barrier(CLK_LOCAL_MEM_FENCE);
    }
};
