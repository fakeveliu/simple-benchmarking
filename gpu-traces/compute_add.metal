#include <metal_stdlib>
using namespace metal;

kernel void add_arrays(device const float* arr1,
                       device const float* arr2,
                       device float* result,
                       uint index [[thread_position_in_grid]])
{
    result[index] = arr1[index] + arr2[index];
}
