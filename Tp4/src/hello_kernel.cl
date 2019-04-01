__kernel void hello(__global double *inputMat, __global double *outputMat)
{
    // int i = get_global_id(0);
    // int j = get_global_id(1);
    // int s = get_global_size(0);
    int j = get_local_id(0);

    outputMat[j] = 2*inputMat[j];
} ;
