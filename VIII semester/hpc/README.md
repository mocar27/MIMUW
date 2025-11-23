# HPC-CUDA Assignment

The Traveling Salesman Problem (TSP) is a classical NP-hard optimization problem where a salesman must visit each city exactly once and return to the starting point, minimizing the total travel distance. Even small instances of TSP can be computationally challenging, making it an excellent target for parallel and GPU-based computation.

One popular metaheuristic to solve TSP is Ant Colony Optimization (ACO). It simulates a swarm of artificial ants that explore paths through the graph using both random exploration and accumulated pheromone trails to reinforce better solutions. Over many iterations, ants collectively converge toward near-optimal solutions.

The TSP was the first problem solved by Ant Colony Optimisation (ACO). 
This method uses a number of simulated “ants” (or agents), which perform distributed search on a graph. 
Each ant moves on the graph until it completes a tour, and then oﬀers this tour as its suggested solution.

## Objectives of the task

- Implement the Worker Ant baseline version where one thread simulates one ant independently.
- Implement the Queen Ant version, where each block builds a tour cooperatively using one thread per city (see paper below).
- In both versions calculate cycle tour length (with reasonable error) and list the elements of the cycle. 
- Implement solution with mutliple kernels and run then in a loop by NUM_ITER value. All core computations must happen on GPU.
- Wrap the core loop of both implementations using CUDA Graphs for efficient execution.
- Explore memory and warp-level optimizations where appropriate.
- Compare the correctness, performance, and scalability of all implementations.

Implementation based on: 
Enhancing Data Parallelism for Ant Colony Optimisation on GPUs
by Jose M. Cecilia, Jose M. Garcıa

## Compilation 

```bash
./acotsp <input_file> <output_file> <TYPE> <NUM_ITER> <ALPHA> <BETA> <EVAPORATE> <SEED>
```
whereas:

- Input file is an instance of TSP from the repo below.
- Output file contans two lines. In the first line there is single number that is the length of the shortest cycle found, 
answering the TSP problem and the second line are citites numbers on the shortest cycle, starting at 1. Each city appears exactly once on this list.
- TYPE will be one of WORKER, QUEEN running proper implementation depending on this value.
- NUM_ITER is the number of iterations that program runs.
- ALPHA and BETA are doubles used in the pheromone value transformation (equation 1 from Paper), 
wherease EVAPORATE is a factor used to diminish pheromones in (equation 2 from Paper).
- SEED is a randomness seed, used for curand initialization.

## Technical Details

As the problem is a heuristic, there is no definite answer to the given input. 
We use solutions from the repo below to the known datasets along with Figure 9. from the article to estimate the expected range. 
Solutions may not exactly be accurate 100%, but they are as described in the task "reasonably close to the expected value".

Let's assume that for the tests mentioned on the Figure 9 in the article, 
your program has relative error up to 50% more than the the worst shown result rounded up to 0.05 
(i.e. for d198 acceptable quality is 1.5 * (1.15 - 1) + 1 = 1.23) compared to the solutions.

City count is limited to 1024 to fit within a single CUDA thread block in the Queen Ant model. 
Thanks to that we ignore the tiling concept introduced in 3.2.2.

## Tests

Taken from [this repo](https://github.com/mastqe/tsplib#) mentioned in the task.
