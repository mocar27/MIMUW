/*
 * A template for the 2019 MPI lab at the University of Warsaw.
 * Copyright (C) 2016, Konrad Iwanicki.
 * Refactoring 2019, Łukasz Rączkowski
 */

#include <iostream>
#include <string>
#include <cassert>
#include <mpi.h>
#include "graph-base.h"
#include "graph-utils.h"


static void runFloydWarshallParallel(Graph* graph, int numProcesses, int myRank) {
    assert(numProcesses <= graph->numVertices);

    int n = graph->numVertices;
    int myFirstRow = graph->firstRowIdxIncl;
    int myLastRow = graph->lastRowIdxExcl;
    int myRowCount = myLastRow - myFirstRow;

    int ownerRank = -1;
    int ownerLastRow = -1;

    for (int k = 0; k < n; k++) {
        // Determine which process owns the k-th row, as this will be the 
        // process currently broadcasting the k-th row
        if (k >= ownerLastRow) {
            ownerRank++;
            ownerLastRow = getFirstGraphRowOfProcess(n, numProcesses, ownerRank + 1);
        }

        int *kRow = graph->extraRow;
        // If this process owns row k, copy it into the buffer
        if (myFirstRow <= k && k < myLastRow) {
            kRow = graph->data[k - myFirstRow];
        }

        // Broadcast the k-th row from the owner to all processes
        // Root (sending) procces won't overwrite the data from graph->data[k - myFirstRow], as he is sending,
        // otherwise upon receiving the data in graph->extraRow will be overwritten
        MPI_Bcast(kRow, n, MPI_INT, ownerRank, MPI_COMM_WORLD);

        // Update local rows using received k-th row
        for (int i = 0; i < myRowCount; i++) {
            for (int j = 0; j < n; ++j) {
                int pathSum = graph->data[i][k] + kRow[j];
                if (graph->data[i][j] > pathSum) {
                    graph->data[i][j] = pathSum;
                }
            }
        }
    }
}


int main(int argc, char *argv[]) {
    int numVertices = 0;
    int numProcesses = 0;
    int myRank = 0;
    int showResults = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

#ifdef USE_RANDOM_GRAPH
#ifdef USE_RANDOM_SEED
    srand(USE_RANDOM_SEED);
#endif
#endif

    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]).compare("--show-results") == 0) {
            showResults = 1;
        } else {
            numVertices = std::stoi(argv[i]);
        }
    }

    if (numVertices <= 0) {
        std::cerr << "Usage: " << argv[0] << "  [--show-results] <num_vertices>" << std::endl;
        MPI_Finalize();
        return 1;
    }

    if (numProcesses > numVertices) {
        numProcesses = numVertices;

        if (myRank >= numProcesses) {
            MPI_Finalize();
            return 0;
        }
    }

    std::cerr << "Running the Floyd-Warshall algorithm for a graph with " << numVertices << " vertices." << std::endl;

    auto graph = createAndDistributeGraph(numVertices, numProcesses, myRank);
    if (graph == nullptr) {
        std::cerr << "Error distributing the graph for the algorithm." << std::endl;
        MPI_Finalize();
        return 2;
    }

    if (showResults) {
        collectAndPrintGraph(graph, numProcesses, myRank);
    }

    double startTime = MPI_Wtime();

    runFloydWarshallParallel(graph, numProcesses, myRank);

    double endTime = MPI_Wtime();

    std::cerr
            << "The time required for the Floyd-Warshall algorithm on a "
            << numVertices
            << "-node graph with "
            << numProcesses
            << " process(es): "
            << endTime - startTime
            << std::endl;

    if (showResults) {
        collectAndPrintGraph(graph, numProcesses, myRank);
    }

    destroyGraph(graph, numProcesses, myRank);

    MPI_Finalize();

    return 0;
}
