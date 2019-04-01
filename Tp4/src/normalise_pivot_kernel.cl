__kernel void normalise_pivot(
   __global float* line_pivot,
   __global float* coeff_pivot)
{
   int j = get_global_id(0);
   line_pivot[j] = line_pivot[j] / coeff_pivot;
}
