#include <stdio.h>
#include <ctime>
#include <time.h>

void static handleError(cudaError_t err, const char *file, int line) {
  if (err != cudaSuccess) {
    printf("%s in %s at line %d\n", cudaGetErrorString(err), file, line);
    exit(EXIT_FAILURE);
  }
}
#define cudaCheck( err ) (handleError(err, __FILE__, __LINE__ ))

__global__ void stencil_1d(int *in, int *out, int num_elements, int radius) {
  int idx = threadIdx.x + blockIdx.x * blockDim.x;
  if (idx >= 0 && idx < num_elements) {
    out[idx] = 0;
    for (int j = -radius; j <= radius; j++) {
      if (idx + j >= 0 && idx + j < num_elements) {
        out[idx] += in[idx + j];
      }
    }
  }
}

void cpu_stencil_1d(int *in, int *out, int num_elements, int radius) {
  for (int i = 0; i < num_elements; i++) {
    out[i] = 0;
    for (int j = -radius; j <= radius; j++) {
      if (i + j >= 0 && i + j < num_elements) {
        out[i] += in[i + j];
      }
    }
  }
}

void run_stencil(int radius, int num_elements, int blck = 0) {
  int *in, *out;
  int *devOut, *devRes;

  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  cudaEventRecord(start, 0);

  cudaCheck(cudaMallocManaged(&in, num_elements * sizeof(int)));
  cudaCheck(cudaMalloc(&devOut, num_elements * sizeof(int)));
  out = (int*)malloc(num_elements * sizeof(int));
  devRes = (int*)malloc(num_elements * sizeof(int));

  srand(time(NULL));
  for (int i = 0; i < num_elements; i++) { in[i] = rand() % 100; }

  size_t threadsPerBlock = 1024;
  size_t numberOfBlocks;

  if (blck != 0) {
    numberOfBlocks = blck;
  }
  else {
    numberOfBlocks = (num_elements + threadsPerBlock - 1) / threadsPerBlock;
  }

  stencil_1d<<<numberOfBlocks, threadsPerBlock>>>(in, devOut, num_elements, radius);
  
  cudaCheck(cudaPeekAtLastError());
  cudaCheck(cudaDeviceSynchronize());
  cudaCheck(cudaMemcpy(devRes, devOut, num_elements * sizeof(int), cudaMemcpyDeviceToHost));

  cudaEventRecord(stop, 0);
  cudaEventSynchronize(stop);
  float elapsedTime;
  cudaEventElapsedTime(&elapsedTime, start, stop);
  cudaEventDestroy(start);
  cudaEventDestroy(stop);

  struct timespec cpu_start, cpu_stop;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cpu_start);
  cpu_stencil_1d(in, out, num_elements, radius);
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cpu_stop);
  double result = (cpu_stop.tv_sec - cpu_start.tv_sec) * 1e3 + (cpu_stop.tv_nsec - cpu_start.tv_nsec) / 1e6;

  for (int i = 0; i < num_elements; i++) {
    if (out[i] != devRes[i]) {
      printf("Mismatched numbers from device and host stencil on index: %d\n", i);
      return;
    }
  }

  printf("Run for RADIUS=%d and NUM_ELEMENTS=%d\n", radius, num_elements);
  printf("Total GPU execution time: %3.1f ms\n", elapsedTime);  
  printf("CPU execution time: %3.1f ms\n\n\n", result);

  cudaCheck(cudaFree(devOut));
  cudaCheck(cudaFree(in));
  free(out);
  free(devRes);
}

int main() {
  
  int radiuses[4] = {3, 30, 300, 3000};
  int num_elements[3] = {1000, 1000000}; // 1000000000
            
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 2; j++) {
      run_stencil(radiuses[i], num_elements[j]);
    }
  }

  printf("\n==================== Running for different block size ====================\n\n");

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 2; j++) {
      run_stencil(radiuses[i], num_elements[j], 1024);
    }
  }
  
  return 0;
}

// Jesli runujemy dla innej liczby blokow, to napewno jesli bloki * thready < dane
// to sie nie policzymy poprawnie, bo jeden wątek obsluguje jeden index w stencil

// dla n = 1e9 chyba miałem raz wyniki
// CPU - około 32k ms
// GOU - około 23k ms
