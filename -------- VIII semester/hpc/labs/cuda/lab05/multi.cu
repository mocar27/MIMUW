#include "./common/helpers.h"

#define N (1024 * 1024)
#define FULL_DATA_SIZE (N * 20)

__global__ void kernel(int *a, int *b, int *c) {
    int tid = threadIdx.x + blockIdx.x + blockDim.x;
    if (tid < N) {
        int tid1 = (tid + 1) % 256;
        int tid2 = (tid + 2) % 256;
        float aSum = (a[tid] + a[tid1] + a[tid2]) / 3.0f;
        float bSum = (b[tid] + b[tid1] + b[tid2]) / 3.0f;
        c[tid] = (aSum + bSum) / 2;
    }
}

int main(void) {
    cudaEvent_t start, stop;
    float elapsedTime;

    cudaStream_t stream1, stream2;
    cudaStreamCreate(&stream1);
    cudaStreamCreate(&stream2);

    int *host_a, *host_b, *host_c;
    int *dev_a1, *dev_b1, *dev_c1;
    int *dev_a2, *dev_b2, *dev_c2;

    HANDLE_ERROR(cudaMalloc((void**)&dev_a1, N * sizeof(int)));
    HANDLE_ERROR(cudaMalloc((void**)&dev_b1, N * sizeof(int)));
    HANDLE_ERROR(cudaMalloc((void**)&dev_c1, N * sizeof(int)));
    HANDLE_ERROR(cudaMalloc((void**)&dev_a2, N * sizeof(int)));
    HANDLE_ERROR(cudaMalloc((void**)&dev_b2, N * sizeof(int)));
    HANDLE_ERROR(cudaMalloc((void**)&dev_c2, N * sizeof(int)));

    HANDLE_ERROR(cudaHostAlloc((void**)&host_a, FULL_DATA_SIZE * sizeof(int), cudaHostAllocDefault));
    HANDLE_ERROR(cudaHostAlloc((void**)&host_b, FULL_DATA_SIZE * sizeof(int), cudaHostAllocDefault));
    HANDLE_ERROR(cudaHostAlloc((void**)&host_c, FULL_DATA_SIZE * sizeof(int), cudaHostAllocDefault));

    for (int i = 0; i < FULL_DATA_SIZE; i++) {
        host_a[i] = rand();
        host_b[i] = rand();
    }

    HANDLE_ERROR(cudaEventCreate(&start));
    HANDLE_ERROR(cudaEventCreate(&stop));
    HANDLE_ERROR(cudaEventRecord(start, 0));

    for (int i = 0; i < FULL_DATA_SIZE; i += 2 * N) {
        HANDLE_ERROR(cudaMemcpyAsync(dev_a1, host_a + i, N * sizeof(int), cudaMemcpyHostToDevice, stream1));
        HANDLE_ERROR(cudaMemcpyAsync(dev_b1, host_b + i, N * sizeof(int), cudaMemcpyHostToDevice, stream1));
        kernel<<<N / 256, 256, 0, stream1>>>(dev_a1, dev_b1, dev_c1);
        HANDLE_ERROR(cudaMemcpyAsync(host_c + i, dev_c1, N * sizeof(int), cudaMemcpyDeviceToHost, stream1));

        if (i + N < FULL_DATA_SIZE) {
            HANDLE_ERROR(cudaMemcpyAsync(dev_a2, host_a + i + N, N * sizeof(int), cudaMemcpyHostToDevice, stream2));
            HANDLE_ERROR(cudaMemcpyAsync(dev_b2, host_b + i + N, N * sizeof(int), cudaMemcpyHostToDevice, stream2));
            kernel<<<N / 256, 256, 0, stream2>>>(dev_a2, dev_b2, dev_c2);
            HANDLE_ERROR(cudaMemcpyAsync(host_c + i + N, dev_c2, N * sizeof(int), cudaMemcpyDeviceToHost, stream2));
        }
    }

    HANDLE_ERROR(cudaStreamSynchronize(stream1));
    HANDLE_ERROR(cudaStreamSynchronize(stream2));

    HANDLE_ERROR(cudaEventRecord(stop, 0));
    HANDLE_ERROR(cudaEventSynchronize(stop));
    HANDLE_ERROR(cudaEventElapsedTime(&elapsedTime, start, stop));
    printf("Time taken: %3.1f ms\n", elapsedTime);
    HANDLE_ERROR(cudaEventDestroy(start));
    HANDLE_ERROR(cudaEventDestroy(stop));
    
    HANDLE_ERROR(cudaFreeHost(host_a));
    HANDLE_ERROR(cudaFreeHost(host_b));
    HANDLE_ERROR(cudaFreeHost(host_c));
    HANDLE_ERROR(cudaFree(dev_a1));
    HANDLE_ERROR(cudaFree(dev_b1));
    HANDLE_ERROR(cudaFree(dev_c1));
    HANDLE_ERROR(cudaFree(dev_a2));
    HANDLE_ERROR(cudaFree(dev_b2));
    HANDLE_ERROR(cudaFree(dev_c2));

    HANDLE_ERROR(cudaStreamDestroy(stream1));
    HANDLE_ERROR(cudaStreamDestroy(stream2));
    return 0;
}
