__kernel void compute_line(__global int* nb_cols,__global double* fact_elims,__global double *inputMat, __global double *outputMat)
{
    const unsigned int idx_worker = get_global_id(0);
    const unsigned int group_size = get_global_size(0);
    int nombre_cols = *nb_cols;
    __private int idx_worker_line;
    __private double val_pivot;

    unsigned int worker_copy = idx_worker;
    barrier(CLK_LOCAL_MEM_FENCE);
    barrier(CLK_GLOBAL_MEM_FENCE);
    idx_worker;
    while(worker_copy < nombre_cols*(nombre_cols/2))
    {
        outputMat[worker_copy] = inputMat[worker_copy];
        barrier(CLK_LOCAL_MEM_FENCE);
        barrier(CLK_GLOBAL_MEM_FENCE);
        worker_copy += group_size;
    }
    barrier(CLK_LOCAL_MEM_FENCE);
    barrier(CLK_GLOBAL_MEM_FENCE);
    for (int idx_ligne = 0; idx_ligne < nombre_cols/2; ++idx_ligne)
    {
        // outputMat[idx_worker] = inputMat[idx_worker];
        idx_worker_line = idx_worker+idx_ligne*nombre_cols;
        barrier(CLK_LOCAL_MEM_FENCE);
        barrier(CLK_GLOBAL_MEM_FENCE);
        val_pivot = outputMat[(idx_ligne*nombre_cols)+idx_ligne];
        barrier(CLK_LOCAL_MEM_FENCE);
        barrier(CLK_GLOBAL_MEM_FENCE);
        while((idx_worker_line<(idx_ligne+1)*nombre_cols) && (idx_worker_line>=(idx_ligne)*nombre_cols))
        {
            outputMat[(idx_ligne*nombre_cols)+(idx_worker_line%nombre_cols)] /= val_pivot;
            barrier(CLK_LOCAL_MEM_FENCE);
            barrier(CLK_GLOBAL_MEM_FENCE);
            idx_worker_line+=group_size;
            barrier(CLK_LOCAL_MEM_FENCE);
            barrier(CLK_GLOBAL_MEM_FENCE);
        }
        barrier(CLK_LOCAL_MEM_FENCE);
        barrier(CLK_GLOBAL_MEM_FENCE);
        idx_worker_line = idx_worker;
        barrier(CLK_LOCAL_MEM_FENCE);
        barrier(CLK_GLOBAL_MEM_FENCE);
        for(int i=0; i< nombre_cols/2;i++)
            fact_elims[i] = 0; 
        barrier(CLK_LOCAL_MEM_FENCE);
        barrier(CLK_GLOBAL_MEM_FENCE);
        while(idx_worker_line<nombre_cols*(nombre_cols/2))
        {
            if(idx_worker_line%nombre_cols == idx_ligne)
                fact_elims[(int)(idx_worker_line/nombre_cols)] = outputMat[((int)(idx_worker_line/nombre_cols)*nombre_cols)+idx_ligne];
            barrier(CLK_LOCAL_MEM_FENCE);
            barrier(CLK_GLOBAL_MEM_FENCE);
            idx_worker_line+=group_size;
            barrier(CLK_LOCAL_MEM_FENCE);
            barrier(CLK_GLOBAL_MEM_FENCE);
        }
        barrier(CLK_LOCAL_MEM_FENCE);
        barrier(CLK_GLOBAL_MEM_FENCE);
        idx_worker_line = idx_worker;
        barrier(CLK_LOCAL_MEM_FENCE);
        barrier(CLK_GLOBAL_MEM_FENCE);
        while(idx_worker_line<nombre_cols*(nombre_cols/2))
        {
            if((int)(idx_worker_line/nombre_cols) != idx_ligne)
                outputMat[((int)(idx_worker_line/nombre_cols)*nombre_cols)+(idx_worker_line%nombre_cols)] -= fact_elims[(int)(idx_worker_line/nombre_cols)] * outputMat[(idx_ligne*nombre_cols)+(idx_worker_line%nombre_cols)];
            barrier(CLK_LOCAL_MEM_FENCE);
            barrier(CLK_GLOBAL_MEM_FENCE);
            idx_worker_line+=group_size;
            barrier(CLK_LOCAL_MEM_FENCE);
            barrier(CLK_GLOBAL_MEM_FENCE);
        }
        barrier(CLK_LOCAL_MEM_FENCE);
        barrier(CLK_GLOBAL_MEM_FENCE);
    }
};




// __kernel void compute_line(__global int* nb_cols, __global double *inputMat, __global double *outputMat)
// {
//     int rows_size = get_global_size(0); // NDRange(0) == lignes
//     int idx_row = get_global_id(0); // Indice du groupe qui soccupe de la ligne
//     int idx_col = get_global_id(1);
//     int nombre_cols = *nb_cols;
//     for (int col=0; col<nombre_cols; col++) {
//         outputMat[(idx_row*nombre_cols)+col] = inputMat[(idx_row*nombre_cols)+col];
//     }
//     __local int idx_pivot;
//     __local double val_pivot;
//     __private double factElim ;
//     for (int idx_ligne = 0; idx_ligne < rows_size; ++idx_ligne)
//     {
//         val_pivot = outputMat[(idx_ligne*nombre_cols)+idx_ligne];


//         // val_pivot = -1000000;
//         // idx_pivot = 0;
//         // for(int i=idx_ligne; i<rows_size; i++)
//         // {
//         //     if (outputMat[(idx_ligne+i)*nombre_cols] > val_pivot)
//         //     {
//         //         val_pivot = outputMat[(idx_ligne+i)*cols_size];
//         //         idx_pivot = i ;
//         //     }
//         // }
//         // Pivot
//         if (idx_row == idx_ligne)
//         {
//             for(int col=0 ; col<nombre_cols; col++)
//                 outputMat[(idx_ligne*nombre_cols)+col] /= val_pivot;
//         }
//         // barrier(CLK_LOCAL_MEM_FENCE);
//         if(idx_row != idx_ligne)
//         {
//             factElim = outputMat[(idx_row*nombre_cols)+idx_ligne];
//             for(int col=0 ; col<nombre_cols; col++) {
//                 outputMat[(idx_row*nombre_cols)+col] -=factElim * outputMat[(idx_ligne*nombre_cols)+col];
//             }

//         }
//         // barrier(CLK_LOCAL_MEM_FENCE);
//     }
// };
