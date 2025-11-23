#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <mpi.h>

int main(int argc, char *argv[])
{

    MPI_Init(&argc, &argv);

    int numProcesses, myRank;
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    
    if (myRank == 0) {
        uint64_t msg = 1;
        MPI_Send(&msg, sizeof(msg), MPI_BYTE, myRank + 1, 0, MPI_COMM_WORLD);
        MPI_Recv(&msg, sizeof(msg), MPI_BYTE, numProcesses - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Received final number: %lu\n", msg);
    } 
    else {
        uint64_t msg;
        MPI_Recv(&msg, sizeof(msg), MPI_BYTE, myRank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        msg *= myRank;
        MPI_Send(&msg, sizeof(msg), MPI_BYTE, (myRank + 1) % numProcesses, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}
