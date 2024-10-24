#include <cuda_runtime.h>
#include <iostream>

__global__ void addKernel(int* c, const int* a, const int* b) {
    size_t i = threadIdx.x;  
    c[i] = a[i] + b[i];
}


void addWithCuda(int* c, const int* a, const int* b, size_t size) {
    int* dev_a = 0;
    int* dev_b = 0;
    int* dev_c = 0;

    cudaMalloc((void**)&dev_c, size * sizeof(int));
    cudaMalloc((void**)&dev_a, size * sizeof(int));
    cudaMalloc((void**)&dev_b, size * sizeof(int));

    cudaMemcpy(dev_a, a, size * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(dev_b, b, size * sizeof(int), cudaMemcpyHostToDevice);

    addKernel << < 1, static_cast<unsigned int>(size) >> > (dev_c, dev_a, dev_b);

    cudaMemcpy(c, dev_c, size * sizeof(int), cudaMemcpyDeviceToHost);

    cudaFree(dev_c);
    cudaFree(dev_a);
    cudaFree(dev_b);
}
