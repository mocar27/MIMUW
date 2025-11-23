#include <math.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <math.h>
#include <complex>
#include <omp.h>

#include "utils/bmp.cpp"


void compress(const uint32_t valuesCount, const int accuracy,
    const uint8_t *values, float *Xreal, float *Ximag) {
  // values, Xreal and Ximag are values describing single color of single row of bitmap. 
  // This function will be called once per each (color, row) combination.
  for (int k = 0; k < accuracy; k++) {
      for (int i = 0; i < valuesCount; i++) {
          float theta = (2 * M_PI * k * i) / valuesCount;
          Xreal[k] += values[i] * cos(theta);
          Ximag[k] -= values[i] * sin(theta);
      }
  }
}

void decompress(const uint32_t valuesCount, const int accuracy,
    uint8_t *values, const float *Xreal, const float *Ximag) {
  // values, Xreal and Ximag are values describing single color of single row of bitmap.
  // This function will be called once per each (color, row) combination.
  std::vector<float> rawValues(valuesCount, 0);

  for (int k = 0; k < accuracy; k++) {
      for (int i = 0; i < valuesCount; i++) {
          float theta = (2 * M_PI * k * i) / valuesCount;
          rawValues[i] += Xreal[k] * cos(theta) + Ximag[k] * sin(theta);
      }
  }

  for (int i = 0; i < valuesCount; i++) {
    values[i] = rawValues[i] / valuesCount;
  }
}

void compressPar(const uint32_t valuesCount, const int accuracy,
    const uint8_t *values, float *Xreal, float *Ximag) {
  // PUT YOUR IMPLEMENTATION HERE
  
}

void compressParNaive(const uint32_t valuesCount, const int accuracy,
  const uint8_t *values, float *Xreal, float *Ximag) {
  // PUT YOUR IMPLEMENTATION HERE
  // #pragma omp parallel
  // {
    #pragma omp for collapse(2) reduction(+:Xreal[:accuracy]) reduction(-:Ximag[:accuracy])
    for (int k = 0; k < accuracy; k++) {
        for (int i = 0; i < valuesCount; i++) {
            float theta = (2 * M_PI * k * i) / valuesCount;
            #pragma omp atomic update
            Xreal[k] += values[i] * cos(theta);
            #pragma omp atomic update
            Ximag[k] -= values[i] * sin(theta);
        }
    }
  // }
}

void decompressPar(const uint32_t valuesCount, const int accuracy,
    uint8_t *values, const float *Xreal, const float *Ximag) {
  // PUT YOUR IMPLEMENTATION HERE
  // std::vector<float> rawValues(valuesCount, 0);

  // #pragma omp parallel for // schedule(dynamic)
  // for (int k = 0; k < accuracy; k++) {
  //     for (int i = 0; i < valuesCount; i++) {
  //         float theta = (2 * M_PI * k * i) / valuesCount;
  //         float val = Xreal[k] * cos(theta) + Ximag[k] * sin(theta);
  //         #pragma omp atomic
  //         rawValues[i] += val;
  //     }
  // }

  // #pragma omp master
  // for (int i = 0; i < valuesCount; i++) {
  //   values[i] = rawValues[i] / valuesCount;
  // }
  std::vector<float> rawValues(valuesCount, 0);
  std::vector<std::vector<float>> sines(2, std::vector<float>(valuesCount, 0));
  std::vector<std::vector<float>> cosines(2, std::vector<float>(valuesCount, 1));
  
}

int main() {
  BMP bmp;
  size_t accuracy = 8; // We are interested in values from range [8; 64]
  
  // bmp.{compress,decompress} will run provided function on every bitmap row and color.
  
  // Sequential execution
  bmp.read("example.bmp");
  float compressTime = bmp.compress(compress, accuracy);
  float decompressTime = bmp.decompress(decompress);

  printf("Compress time: %.2lfs\nDecompress time: %.2lfs\nTotal: %.2lfs\n\n", 
    compressTime, decompressTime, compressTime + decompressTime);

  bmp.write("example_result.bmp");

  // Naive parallel execution
  bmp.read("example.bmp");
  float compressTimeParNaive = bmp.compress(compressParNaive, accuracy);
  float decompressTimeParNaive = bmp.decompress(decompressPar);

  printf("Naive Parallel Compress time: %.2lfs\nDecompress Prallel time: %.2lfs\nTotal Prallel Naive: %.2lfs\n\n", 
    compressTimeParNaive, decompressTimeParNaive, compressTimeParNaive + decompressTimeParNaive);

  bmp.write("example_result_naive.bmp");

  // Parallel execution
  bmp.read("example.bmp");
  float compressTimePar = bmp.compress(compressPar, accuracy);
  float decompressTimePar = bmp.decompress(decompressPar);

  printf("Compress Parallel time: %.2lfs\nDecompress Parallel time: %.2lfs\nTotal Parallel: %.2lfs\n\n", 
    compressTimePar, decompressTimePar, compressTimePar + decompressTimePar);

  bmp.write("example_result_par.bmp");

  return 0;
}
