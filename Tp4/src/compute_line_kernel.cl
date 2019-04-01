__kernel void compute_line(
    __global float* line_pivot,
    __global float* line,
    __global float* coeff_ligne)
{
    int i = get_global_id(0);
    int j = get_local_id(1);
    line[i] = line[i] - line_pivot[j] * coeff_ligne;
}
