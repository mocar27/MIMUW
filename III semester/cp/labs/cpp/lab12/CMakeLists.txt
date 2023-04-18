cmake_minimum_required (VERSION 3.1)
project (MIMUW-CPCPP-lab1 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-g -Wall")

set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

function(add_example_program target_name)
  add_executable(${target_name} "${target_name}.cpp")
  target_link_libraries(${target_name} Threads::Threads)
endfunction()

add_example_program(grep-seq)
add_example_program(grep-multit)
