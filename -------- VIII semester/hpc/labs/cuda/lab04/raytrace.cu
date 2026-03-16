#include "cuda.h"
#include "common/errors.h"
#include "common/cpu_bitmap.h"

#define DIM 1024
#define rnd(x) (x * rand() / RAND_MAX)
#define INF 2e10f
#define SPHERES 1000

struct Sphere {
	float red, green, blue;
	float radius;
	float x, y, z;

	__device__ float hit(float bitmapX, float bitmapY, float *colorFalloff) {
		float distX = bitmapX - x;
		float distY = bitmapY - y;

		if (distX * distX + distY * distY < radius * radius) {
			float distZ = sqrtf(radius * radius - distX * distX - distY * distY);
			*colorFalloff = distZ / sqrtf(radius * radius);
			return distZ + z;
		}

		return -INF;
	}
};

__global__ void kernel(Sphere *spheres, unsigned char* bitmap) {
	__shared__ Sphere sharedSpheres[SPHERES];
	int idx = threadIdx.x + threadIdx.y * blockDim.x;
	int stride = blockDim.x * blockDim.y;

	for(int i = idx; i < SPHERES; i += stride) {
		sharedSpheres[i] = spheres[i];
	}

	__syncthreads();

	int x = threadIdx.x + blockIdx.x * blockDim.x;
	int y = threadIdx.y + blockIdx.y * blockDim.y;
	int offset = x + y * blockDim.x * gridDim.x;

	float bitmapX = (x - DIM / 2);
	float bitmapY = (y - DIM / 2);

	float red = 0, green = 0, blue = 0;
	float maxDepth = -INF;

	for (int i = 0; i < SPHERES; i++) {
		float colorFalloff;
		float depth = sharedSpheres[i].hit(bitmapX, bitmapY, &colorFalloff);

		if (depth > maxDepth) {
			red = sharedSpheres[i].red * colorFalloff;
			green = sharedSpheres[i].green * colorFalloff;
			blue = sharedSpheres[i].blue * colorFalloff;
			maxDepth = depth;
		}
  }

	bitmap[offset * 4 + 0] = (int) (red * 255);
	bitmap[offset * 4 + 1] = (int) (green * 255);
	bitmap[offset * 4 + 2] = (int) (blue * 255);
	bitmap[offset * 4 + 3] = 255;
}

struct DataBlock {
	unsigned char *hostBitmap;
	Sphere *spheres;
};

int main(void) {
	DataBlock data;
	cudaEvent_t start, stop;
	HANDLE_ERROR(cudaEventCreate(&start));
	HANDLE_ERROR(cudaEventCreate(&stop));
	HANDLE_ERROR(cudaEventRecord(start, 0));

	CPUBitmap bitmap(DIM, DIM, &data);
	unsigned char *devBitmap;
	Sphere *devSpheres;

	HANDLE_ERROR(cudaMalloc((void**)&devBitmap, bitmap.image_size()));
	HANDLE_ERROR(cudaMalloc((void**)&devSpheres, sizeof(Sphere) * SPHERES));

	Sphere *hostSpheres = (Sphere*)malloc(sizeof(Sphere) * SPHERES);

	for (int i = 0; i < SPHERES; i++) {
		hostSpheres[i].red = rnd(1.0f);
		hostSpheres[i].green = rnd(1.0f);
		hostSpheres[i].blue = rnd(1.0f);
		hostSpheres[i].x = rnd(1000.0f) - 500;
		hostSpheres[i].y = rnd(1000.0f) - 500;
		hostSpheres[i].z = rnd(1000.0f) - 500;
		hostSpheres[i].radius = rnd(100.0f) + 20;
	}

	HANDLE_ERROR(cudaMemcpy(devSpheres, hostSpheres, sizeof(Sphere) * SPHERES, cudaMemcpyHostToDevice));
	free(hostSpheres);

	dim3 grids(DIM / 16, DIM / 16);
	dim3 threads(16, 16);
	kernel<<<grids,threads>>>(devSpheres, devBitmap);

	HANDLE_ERROR(cudaEventRecord(stop, 0));
	HANDLE_ERROR(cudaEventSynchronize(stop));

	float elapsedTime;
	HANDLE_ERROR(cudaEventElapsedTime(&elapsedTime, start, stop));
	printf("Time to generate: %3.1f ms\n", elapsedTime);

	HANDLE_ERROR(cudaEventDestroy(start));
	HANDLE_ERROR(cudaEventDestroy(stop));

	HANDLE_ERROR(cudaMemcpy(bitmap.get_ptr(), devBitmap, bitmap.image_size(), cudaMemcpyDeviceToHost));

  bitmap.dump_ppm("image2.ppm");


	cudaFree(devBitmap);
	cudaFree(devSpheres);
}
