#include <cstdio>
#include "common/errors.h"

#define TILE_DIM 32
#define BLOCK_ROWS 8

__global__ void transpose(float *odata, const float *idata)
{
  // (x, y) are coordinates inside the global matrix
  int x = blockIdx.x * TILE_DIM + threadIdx.x;
  int y = blockDim.x * blockIdx.y * TILE_DIM + threadIdx.y;
  int width = gridDim.x * TILE_DIM;

  for (int j = 0; j < TILE_DIM; j+= BLOCK_ROWS) {
    odata[x*width + (y+j)] = idata[(y+j)*width + x];
  }
}

int main()
{
  const int nx = 8192;
  const int ny = 8192;

  const int mem_size = nx * ny * sizeof(float);

  dim3 dimGrid(nx/TILE_DIM, ny/TILE_DIM, 1);
  // Each block is responsible for transposing one TILE_DIM x TILE_DIM submatrix
  dim3 dimBlock(TILE_DIM, BLOCK_ROWS, 1);

  if (nx % TILE_DIM || ny % TILE_DIM) {
    printf("nx and ny must be a multiple of TILE_DIM\n");
    return 1;
  }

  if (TILE_DIM % BLOCK_ROWS) {
    printf("TILE_DIM must be a multiple of BLOCK_ROWS\n");
    return 1;
  }

  float *host_input = (float*)malloc(mem_size), *host_correct = (float*)malloc(mem_size), *host_output = (float*)malloc(mem_size);
  float *dev_input, *dev_output;

  HANDLE_ERROR(cudaMalloc(&dev_input, mem_size));
  HANDLE_ERROR(cudaMalloc(&dev_output, mem_size));

  for (int j = 0; j < ny; j++)
    for (int i = 0; i < nx; i++)
      host_input[j*nx + i] = host_correct[i * nx + j] = j*nx + i;

  HANDLE_ERROR(cudaMemcpy(dev_input, host_input, mem_size, cudaMemcpyHostToDevice));

  cudaEvent_t startEvent, stopEvent;
  HANDLE_ERROR(cudaEventCreate(&startEvent));
  HANDLE_ERROR(cudaEventCreate(&stopEvent));
	HANDLE_ERROR(cudaEventRecord(startEvent, 0));

  transpose<<<dimGrid, dimBlock>>>(dev_output, dev_input);

  HANDLE_ERROR(cudaEventRecord(stopEvent, 0));
  HANDLE_ERROR(cudaEventSynchronize(stopEvent));

	float elapsedTime;
	HANDLE_ERROR(cudaEventElapsedTime(&elapsedTime, startEvent, stopEvent));
	printf("Time to generate: %3.1f ms\n", elapsedTime);

  HANDLE_ERROR(cudaMemcpy(host_output, dev_output, mem_size, cudaMemcpyDeviceToHost));

  for (int j = 0; j < ny; j++)
    for (int i = 0; i < nx; i++)
      if (host_output[j * nx + i] != host_correct[j * nx + i]) {
        printf("Wrong value at (%d, %d), got %f, expected %f\n", i, j, host_output[j * nx + i], host_correct[j * nx + i]);
        return 1;
      }
}
