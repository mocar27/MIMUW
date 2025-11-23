// Solves the n-quees puzzle on an n x x checkerboard.
//
// This sequential implementation is to be extended with TBB to get a
// parallel implementation.
//
// HPC course, MIM UW
// Krzysztof Rzadca, LGPL

#include "tbb/tbb.h"
#include <iostream>
#include <list>
#include <vector>
#include <cmath>


// Indexed by column. Value is the row where the queen is placed,
// or -1 if no queen.
typedef std::vector<int> Board;


void pretty_print(const Board& board) {
    for (int row = 0; row < (int) board.size(); row++) {
        for (const auto& loc : board) {
            if (loc == row)
                std::cout << "*";
            else
                std::cout << " ";
        }
        std::cout << std::endl;
    }
}


// Checks the location of queen in column 'col' against queens in cols [0, col).
bool check_col(Board& board, int col_prop) {
    int row_prop = board[col_prop];
    int col_queen = 0;
    for (auto i = board.begin();
         (i != board.end()) && (col_queen < col_prop);
         ++i, ++col_queen) {
        int row_queen = *i;
        if (row_queen == row_prop) {
            return false;
        }
        if (abs(row_prop - row_queen) == col_prop - col_queen) {
            return false;
        }
    }
    return true;
}


void initialize(Board& board, int size) {
    board.reserve(size);
    for (int col = 0; col < size; ++col)
        board.push_back(-1);
}


// Solves starting from a partially-filled board (up to column col).
void recursive_solve(Board& partial_board, int col, std::list<Board>& solutions) {
    // std::cout << "rec solve col " << col << std::endl;
    // pretty_print(b_partial);
    
    int b_size = partial_board.size();
    if (col == b_size) {
        solutions.push_back(partial_board);
    }
    else {
        for (int tested_row = 0; tested_row < b_size; tested_row++) {
            partial_board[col] = tested_row;
            if (check_col(partial_board, col))
                recursive_solve(partial_board, col+1, solutions);
        }
    }
}

void recursive_par_for_count(Board& board, int col, std::atomic<long>& solution_count) {
    int b_size = board.size();
    if (col == b_size) {
        ++solution_count;
    } else {
        for (int tested_row = 0; tested_row < b_size; ++tested_row) {
            board[col] = tested_row;
            if (check_col(board, col)) {
                recursive_par_for_count(board, col + 1, solution_count);
            }
        }
    }
}

void recursive_solve_par(Board partial_board, int col, tbb::concurrent_queue<Board>& solutions, tbb::task_group& g) {
    // std::cout << "rec solve col " << col << std::endl;
    // pretty_print(b_partial);
    
    int b_size = partial_board.size();
    if (col == b_size) {
        solutions.push(partial_board);
    }
    else {
        for (int tested_row = 0; tested_row < b_size; tested_row++) {
                partial_board[col] = tested_row;
                if (check_col(partial_board, col)) {
                    g.run([partial_board, col, &g, &solutions] { 
                        recursive_solve_par(partial_board, col + 1, solutions, g);
                    });
                }
        }
    }
}

void parallel_for_nqueens(int board_size, std::atomic<long>& solution_count) {
    tbb::parallel_for(0, board_size, [&](int row0) {
        Board board(board_size, -1);
        board[0] = row0;
        if (!check_col(board, 0)) return;

        for (int row1 = 0; row1 < board_size; ++row1) {
            board[1] = row1;
            if (!check_col(board, 1)) continue;

            for (int row2 = 0; row2 < board_size; ++row2) {
                board[2] = row2;
                if (!check_col(board, 2)) continue;

                Board board_copy = board;
                recursive_par_for_count(board_copy, 3, solution_count);
            }
        }
    });
}

int main() {
    const int board_size = 13;
    Board board{};
    initialize(board, board_size);
    std::list<Board> solutions{};

    // Sequential solution
    tbb::tick_count seq_start_time = tbb::tick_count::now();
    recursive_solve(board, 0, solutions);
    tbb::tick_count seq_end_time = tbb::tick_count::now();
    double seq_time = (seq_end_time-seq_start_time).seconds();
    std::cout << "seq time: " << seq_time << "[s]" <<std::endl;
    std::cout << "solution count: "<<solutions.size()<<std::endl;

    // Parallel TBB solution
    tbb::concurrent_queue<Board> solutions_par;
    tbb::task_group g;
    tbb::tick_count par_start_time = tbb::tick_count::now();
    recursive_solve_par(board, 0, solutions_par, g);
    g.wait();  // Wait for all tasks to finish
    tbb::tick_count par_end_time = tbb::tick_count::now();
    double par_time = (par_end_time - par_start_time).seconds();

    std::cout << "Parallel time: " << par_time << "[s]" << std::endl;
    std::cout << "Solution count: " << solutions_par.unsafe_size() << std::endl;

    std::atomic<long> solution_count{0};
    tbb::tick_count par_for_start_time = tbb::tick_count::now();
    parallel_for_nqueens(board_size, solution_count);
    tbb::tick_count par_for_end_time = tbb::tick_count::now();
    double par_for_time = (par_for_end_time - par_for_start_time).seconds();

    std::cout << "Parallel_for time: " << par_for_time << " [s]" << std::endl;
    std::cout << "Solution count: " << solution_count << std::endl;

    // for (const auto& sol : solutions) {
    //     pretty_print(sol);
    //     std::cout << std::endl;
    // }
}
