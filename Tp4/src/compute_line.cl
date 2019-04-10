__kernel void compute_line(__global int* nb_cols,__global int* idx_line,__global double* fact_elims,__global double *outputMat)
{
    unsigned int idx_worker = get_global_id(0);
    unsigned int group_size = get_global_size(0);
    int nombre_cols = *nb_cols;
    int idx_worker_line;
    int idx_ligne = *idx_line;
    double val_pivot;
    printf("BWAAA\n");
    idx_worker_line = idx_worker+(idx_ligne*nombre_cols);
    // while(idx_worker_line%nombre_cols==1) {
    //     // printf("%lf  ", outputMat[idx_worker_line]);
    //     idx_worker_line+=group_size;
    // }
    // if((get_global_id(0)==1) || (get_global_id(0)==0)  )
    //     printf("\n");
    while(idx_worker_line<nombre_cols*(nombre_cols/2))
    {
        if(idx_worker_line%nombre_cols == idx_ligne)
            fact_elims[(int)(idx_worker_line/nombre_cols)] = outputMat[idx_worker_line];
        idx_worker_line+=group_size;
    }
    barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);

    idx_worker_line = idx_worker;
    while(idx_worker_line<nombre_cols*(nombre_cols/2))
    {
        if((int)(idx_worker_line/nombre_cols) != idx_ligne)
        {
            // printf("\nETAPE AV: %d, WORKER : %d : VALEUR : %lf ** SOUS : %lf( %lf  *  %lf  )\n",idx_ligne,idx_worker_line,outputMat[idx_worker_line],fact_elims[(int)(idx_worker_line/nombre_cols)] * outputMat[(idx_ligne*nombre_cols)+(idx_worker_line%nombre_cols)],fact_elims[(int)(idx_worker_line/nombre_cols)], outputMat[((idx_ligne)*nombre_cols)+(idx_worker_line%nombre_cols)] );
            outputMat[idx_worker_line] -= fact_elims[(int)(idx_worker_line/nombre_cols)] * outputMat[((idx_ligne)*nombre_cols)+(idx_worker_line%nombre_cols)];
            // printf("\nETAPE AP: %d, WORKER : %d : RES :%lf\n",idx_ligne,idx_worker_line,outputMat[idx_worker_line]);
        }
        barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);

        idx_worker_line+=group_size;

    }
    idx_worker_line = idx_worker;
    barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);

    if(get_local_id(0)==0)
        *idx_line +=1;
};
