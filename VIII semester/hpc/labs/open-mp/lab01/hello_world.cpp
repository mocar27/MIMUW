#include <iostream>
#include <omp.h>

int main(int argc, char *argv[]) {
  int threadCount, threadId;
  #pragma omp parallel private(threadCount, threadId) 
  {
    threadId = omp_get_thread_num();

    #pragma omp for
    for (int i = 0; i < 10; i++) {
        #pragma omp critical
        std::cout << "Hello world from thread " << threadId << " and loop iteration " << i << std::endl;
    }
    
    if (threadId == 0) {
      threadCount = omp_get_num_threads();
      std::cout << "Number of threads = " << threadCount << std::endl;
    }
  }

  return 0;
}