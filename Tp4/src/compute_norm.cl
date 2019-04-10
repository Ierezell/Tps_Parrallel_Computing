__kernel void compute_norm(__global int* nb_cols,__global int* idx_line, __global double *outputMat)
{
    const unsigned int idx_worker = get_global_id(0);
    const unsigned int group_size = get_global_size(0);
    int nombre_cols = *nb_cols;
    int idx_ligne = *idx_line;
    int idx_worker_line;
    double val_pivot;

    unsigned int worker_copy = idx_worker;
    idx_worker_line = idx_worker+idx_ligne*nombre_cols;

    val_pivot = outputMat[(idx_ligne*nombre_cols)+idx_ligne];
    while((idx_worker_line<(idx_ligne+1)*nombre_cols) && (idx_worker_line>=(idx_ligne)*nombre_cols))
    {
        outputMat[(idx_ligne*nombre_cols)+(idx_worker_line%nombre_cols)] /= val_pivot;
        idx_worker_line+=group_size;
    }
    barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);

};
