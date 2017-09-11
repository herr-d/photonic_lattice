
add_library(Graph Graph.cpp)
add_library(Parallel Parallel.cpp)
add_library(Purify Purify.cpp)


add_executable(main main.cpp)
target_link_libraries(main Parallel Graph Purify)

add_executable(percolation_test percolation_test.cpp)
target_link_libraries(percolation_test Parallel Graph Purify)
