cmake_minimum_required(VERSION 3.10)

project(OSLab2 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

add_executable(main src/main.c src/qpsort.c)
target_link_libraries(main pthread)

# add_executable(only_sort src/only_sort.c src/qpsort.c)
# target_link_libraries(only_sort pthread)

